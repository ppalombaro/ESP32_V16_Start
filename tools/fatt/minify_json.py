import os
import json
import sys

import hashlib

FFAT_MAX = 917504  # adjust if needed

# Accept SRC/DST as arguments for flexibility
if len(sys.argv) < 3:
    print("Usage: minify_json.py <source_folder> <destination_folder> [--dry-run]")
    sys.exit(1)

SRC = sys.argv[1]
DST = sys.argv[2]
DRY_RUN = "--dry-run" in sys.argv

total = 0
manifest = []

try:
    for root, _, files in os.walk(SRC):
        for f in files:
            if f.endswith(".json"):
                src_path = os.path.join(root, f)
                rel_path = os.path.relpath(src_path, SRC)
                dst_path = os.path.join(DST, rel_path)

                os.makedirs(os.path.dirname(dst_path), exist_ok=True)

                print(f"Processing: {rel_path}")

                try:
                    with open(src_path, "r", encoding="utf-8") as fh:
                        data = json.load(fh)
                except json.JSONDecodeError as e:
                    print(f"\nJSON ERROR in {rel_path}: line {e.lineno}, column {e.colno}: {e.msg}")
                    sys.exit(1)
                except Exception as e:
                    print(f"\nERROR reading {rel_path}: {e}")
                    sys.exit(1)

                # Minify JSON
                minified = json.dumps(data, separators=(",", ":"))

                if not DRY_RUN:
                    with open(dst_path, "w", encoding="utf-8") as fh:
                        fh.write(minified)

                size = len(minified.encode("utf-8"))
                total += size

                # Compute SHA256 for manifest
                sha256 = hashlib.sha256(minified.encode("utf-8")).hexdigest()
                manifest.append((rel_path.replace("\\","/"), size, sha256))

                print(f"Minimized: {rel_path} ({size} bytes)")

    print("\nSUMMARY")
    print(f"Files: {len(manifest)}")
    print(f"Total size: {total} bytes ({total/1024:.2f} KB)")
    
    # Optional: warn if nearing FFAT limit
    if total > FFAT_MAX:
        print("ERROR: FFAT limit exceeded")
        sys.exit(1)
    elif total > FFAT_MAX * 0.9:
        print("WARNING: FFAT usage over 90%")
    else:
        print("FFAT usage OK")

except Exception as e:
    print("ERROR during minification:", e)
    sys.exit(1)
