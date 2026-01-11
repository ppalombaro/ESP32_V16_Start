"""
generate_manifest.py
V16.1.3-2026-01-09T03:30:00Z
Generate SHA256 manifest from data directory
"""
import os
import sys
import hashlib

def generate_manifest(src_dir, manifest_file):
    """Generate manifest: path,size,sha256"""
    entries = []
    
    for root, _, files in os.walk(src_dir):
        for f in files:
            full_path = os.path.join(root, f)
            rel_path = os.path.relpath(full_path, src_dir).replace("\\", "/")
            
            with open(full_path, "rb") as fh:
                content = fh.read()
            
            size = len(content)
            sha = hashlib.sha256(content).hexdigest()
            entries.append((rel_path, size, sha))
    
    # Write manifest
    with open(manifest_file, "w") as f:
        for path, size, sha in entries:
            f.write(f"{path},{size},{sha}\n")
    
    print(f"Manifest written: {manifest_file}")
    print(f"Total files: {len(entries)}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: generate_manifest.py <source_dir> <manifest_file>")
        sys.exit(1)
    
    generate_manifest(sys.argv[1], sys.argv[2])
