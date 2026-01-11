"""
build_simple_storage.py
V16.1.3-2026-01-09T05:10:00Z
Creates SIMPLE storage format that ContentManager can read
NO filesystem structures - just indexed files
"""
import os
import sys
import struct
import hashlib
import argparse

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

def build_simple_storage(entries, output_file, max_size):
    """
    Build simple storage format:
    [4 bytes: file count]
    For each file:
        [2 bytes: path length]
        [N bytes: path UTF-8]
        [4 bytes: content length]
        [N bytes: content]
        [padding to 512-byte boundary]
    """
    print(f"\nBuilding simple storage format...")
    print(f"  Max size: {max_size} bytes ({max_size/1024:.1f} KB)")
    print(f"  Files: {len(entries)}")
    
    # Create image
    image = bytearray(max_size)
    offset = 0
    
    # Write file count (4 bytes, little-endian)
    struct.pack_into('<I', image, offset, len(entries))
    offset += 4
    
    print(f"  File count written: {len(entries)}")
    
    # Write each file
    for path, content in entries:
        path_bytes = path.encode('utf-8')
        path_len = len(path_bytes)
        content_len = len(content)
        
        # Check if we have space
        needed = 2 + path_len + 4 + content_len
        if offset + needed > max_size:
            print(f"  ERROR: Out of space at file: {path}")
            break
        
        # Write path length (2 bytes)
        struct.pack_into('<H', image, offset, path_len)
        offset += 2
        
        # Write path
        image[offset:offset + path_len] = path_bytes
        offset += path_len
        
        # Write content length (4 bytes)
        struct.pack_into('<I', image, offset, content_len)
        offset += 4
        
        # Write content
        image[offset:offset + content_len] = content
        offset += content_len
        
        # Align to 512-byte boundary
        padding = (512 - (offset % 512)) % 512
        offset += padding
        
        print(f"  Wrote: {path} ({content_len} bytes) @ offset {offset - content_len - padding}")
    
    # Write to file
    with open(output_file, 'wb') as f:
        f.write(image)
    
    print(f"\nStorage image created: {output_file}")
    print(f"Total size: {len(image)} bytes")
    print(f"Used: {offset} bytes ({offset*100/max_size:.1f}%)")
    
    return True

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("input_dir", help="Source directory")
    parser.add_argument("output_file", help="Output image file")
    parser.add_argument("max_size", type=int, help="Max size in bytes")
    parser.add_argument("--manifest", help="Manifest file")
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()
    
    # Collect files
    print("Collecting files...")
    entries = collect_files(args.input_dir)
    
    if len(entries) == 0:
        print("ERROR: No files found!")
        sys.exit(1)
    
    print(f"Found {len(entries)} files")
    
    total_size = sum(len(content) for _, content in entries)
    print(f"Total content: {total_size} bytes ({total_size/1024:.1f} KB)")
    
    # Write manifest
    if args.manifest:
        write_manifest(entries, args.manifest)
    
    # Create image
    if not args.dry_run:
        success = build_simple_storage(entries, args.output_file, args.max_size)
        if success:
            print("\nSUCCESS! Ready to flash.")
            sys.exit(0)
        else:
            sys.exit(1)
    else:
        print("\nDry-run complete")

if __name__ == "__main__":
    main()
