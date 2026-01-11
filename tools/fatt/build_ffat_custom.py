"""
build_ffat_custom.py
V16.1.3-2026-01-09T04:10:00Z
Creates FAT filesystem without any external dependencies
Pure Python implementation that ESP32 FFat library can read
"""
import os
import sys
import struct
import hashlib
import argparse
from datetime import datetime

SECTOR_SIZE = 512
CLUSTER_SIZE = 4096  # 8 sectors per cluster

def collect_files(src_dir):
    """Collect all files with forward-slash paths"""
    entries = []
    for root, _, files in os.walk(src_dir):
        for f in files:
            full_path = os.path.join(root, f)
            rel_path = os.path.relpath(full_path, src_dir).replace("\\", "/")
            with open(full_path, "rb") as fh:
                content = fh.read()
            entries.append((rel_path, content))
    return entries

def write_manifest(entries, manifest_file):
    """Write manifest for verification"""
    with open(manifest_file, "w") as f:
        for path, content in entries:
            sha = hashlib.sha256(content).hexdigest()
            f.write(f"{path},{len(content)},{sha}\n")
    print(f"Manifest written: {manifest_file}")

def create_fat16_boot_sector(total_sectors):
    """Create FAT16 boot sector"""
    boot = bytearray(512)
    
    # Jump instruction
    boot[0:3] = b'\xEB\x3C\x90'
    boot[3:11] = b'MSDOS5.0'
    
    # BPB
    struct.pack_into('<H', boot, 11, 512)         # Bytes per sector
    struct.pack_into('B', boot, 13, 8)            # Sectors per cluster
    struct.pack_into('<H', boot, 14, 1)           # Reserved sectors
    struct.pack_into('B', boot, 16, 2)            # Number of FATs
    struct.pack_into('<H', boot, 17, 512)         # Root entries
    struct.pack_into('<H', boot, 19, 0)           # Small sectors (use 32-bit)
    struct.pack_into('B', boot, 21, 0xF8)         # Media descriptor
    struct.pack_into('<H', boot, 22, 8)           # Sectors per FAT
    struct.pack_into('<I', boot, 32, total_sectors)  # Large sectors
    
    # Extended BPB
    struct.pack_into('B', boot, 36, 0x80)         # Drive number
    struct.pack_into('B', boot, 38, 0x29)         # Boot signature
    struct.pack_into('<I', boot, 39, 0x12345678)  # Volume ID
    boot[43:54] = b'FFAT       '                  # Volume label
    boot[54:62] = b'FAT16   '                     # FS type
    
    # Boot signature
    struct.pack_into('<H', boot, 510, 0xAA55)
    
    return bytes(boot)

def create_fat16_table(num_clusters):
    """Create FAT16 allocation table"""
    fat_size = 8 * 512  # 8 sectors
    fat = bytearray(fat_size)
    
    # Reserved entries
    struct.pack_into('<I', fat, 0, 0xFFFFFFF8)
    struct.pack_into('<H', fat, 4, 0xFFFF)
    
    return bytes(fat)

def create_directory_entry(name, is_dir, first_cluster, file_size):
    """Create 32-byte FAT directory entry"""
    entry = bytearray(32)
    
    # 8.3 filename
    if is_dir:
        name_bytes = name.upper().ljust(11)[:11].encode('ascii')
    else:
        # Split name and extension
        if '.' in name:
            base, ext = name.rsplit('.', 1)
            name_bytes = base.upper().ljust(8)[:8].encode('ascii')
            name_bytes += ext.upper().ljust(3)[:3].encode('ascii')
        else:
            name_bytes = name.upper().ljust(11)[:11].encode('ascii')
    
    entry[0:11] = name_bytes
    
    # Attributes
    attr = 0x10 if is_dir else 0x20  # Directory or Archive
    struct.pack_into('B', entry, 11, attr)
    
    # Timestamps (use current time)
    now = datetime.now()
    fat_time = ((now.hour << 11) | (now.minute << 5) | (now.second // 2))
    fat_date = (((now.year - 1980) << 9) | (now.month << 5) | now.day)
    
    struct.pack_into('<H', entry, 14, fat_time)  # Creation time
    struct.pack_into('<H', entry, 16, fat_date)  # Creation date
    struct.pack_into('<H', entry, 22, fat_time)  # Write time
    struct.pack_into('<H', entry, 24, fat_date)  # Write date
    
    # First cluster
    struct.pack_into('<H', entry, 26, first_cluster)
    
    # File size
    struct.pack_into('<I', entry, 28, file_size)
    
    return bytes(entry)

def build_custom_fat_image(entries, output_file, max_size):
    """Build FAT16 filesystem with files"""
    total_sectors = max_size // 512
    
    print(f"\nBuilding custom FAT16 filesystem:")
    print(f"  Image size: {max_size} bytes")
    print(f"  Total sectors: {total_sectors}")
    
    # Create base structures
    boot_sector = create_fat16_boot_sector(total_sectors)
    fat_table = create_fat16_table(total_sectors // 8)
    
    # Calculate offsets
    fat1_offset = 512
    fat2_offset = fat1_offset + len(fat_table)
    root_offset = fat2_offset + len(fat_table)
    root_size = 512 * 32  # 512 entries * 32 bytes
    data_offset = root_offset + root_size
    
    print(f"  Root directory: {root_offset}")
    print(f"  Data area: {data_offset}")
    
    # Initialize image
    image = bytearray(max_size)
    
    # Write boot sector
    image[0:512] = boot_sector
    
    # Write FAT tables
    image[fat1_offset:fat1_offset + len(fat_table)] = fat_table
    image[fat2_offset:fat2_offset + len(fat_table)] = fat_table
    
    # Build directory structure
    dirs = {}  # path -> (offset, cluster)
    dirs[''] = (root_offset, 0)  # Root
    
    # Collect all directories
    for path, content in entries:
        parts = path.split('/')
        for i in range(1, len(parts)):
            dir_path = '/'.join(parts[:i])
            if dir_path not in dirs:
                dirs[dir_path] = None  # Placeholder
    
    # Assign clusters to directories and files
    current_cluster = 2  # Start after reserved clusters
    cluster_assignments = {}
    
    # Assign clusters to directories first
    sorted_dirs = sorted([d for d in dirs.keys() if d != ''])
    for dir_path in sorted_dirs:
        dirs[dir_path] = (data_offset + (current_cluster - 2) * CLUSTER_SIZE, current_cluster)
        current_cluster += 1
    
    # Assign clusters to files
    for path, content in entries:
        num_clusters = (len(content) + CLUSTER_SIZE - 1) // CLUSTER_SIZE
        cluster_assignments[path] = (current_cluster, num_clusters)
        current_cluster += num_clusters
    
    print(f"  Total clusters used: {current_cluster}")
    
    # Write directory entries
    for path, content in entries:
        parent_path = '/'.join(path.split('/')[:-1])
        filename = path.split('/')[-1]
        
        parent_offset, _ = dirs[parent_path]
        first_cluster, num_clusters = cluster_assignments[path]
        
        # Find free slot in parent directory
        for slot in range(512):  # Max 512 entries per directory
            entry_offset = parent_offset + (slot * 32)
            if image[entry_offset] == 0 or image[entry_offset] == 0xE5:  # Free slot
                entry = create_directory_entry(filename, False, first_cluster, len(content))
                image[entry_offset:entry_offset + 32] = entry
                print(f"  Entry: {path} -> cluster {first_cluster}")
                break
        
        # Write file data
        file_offset = data_offset + (first_cluster - 2) * CLUSTER_SIZE
        image[file_offset:file_offset + len(content)] = content
    
    # Write to file
    with open(output_file, 'wb') as f:
        f.write(image)
    
    print(f"\nCustom FAT16 image created: {output_file}")
    return True

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("input_dir")
    parser.add_argument("output_file")
    parser.add_argument("max_size", type=int)
    parser.add_argument("--manifest")
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()
    
    # Collect files
    print("Collecting files...")
    entries = collect_files(args.input_dir)
    print(f"Found {len(entries)} files")
    
    total_size = sum(len(content) for _, content in entries)
    print(f"Total content: {total_size} bytes ({total_size/1024:.1f} KB)")
    
    # Write manifest
    if args.manifest:
        write_manifest(entries, args.manifest)
    
    # Create image
    if not args.dry_run:
        success = build_custom_fat_image(entries, args.output_file, args.max_size)
        if success:
            print("\nSUCCESS! Ready to flash.")
            sys.exit(0)
        else:
            sys.exit(1)
    else:
        print("\nDry-run complete")

if __name__ == "__main__":
    main()
