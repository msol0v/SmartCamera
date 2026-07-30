"""
makeFSdata.py — Generate a LwIP fsdata.c file from a directory of web files.

Usage:
    python makeFSdata.py [options]

Options:
    --fs-dir    Directory containing the web-server files 
    --out       Output fsdata.c path
    --line-size Hex bytes per line in output    (default: 16)
    --no-gzip   Disable gzip compression of file content
"""

import argparse
import gzip
import os
import sys

# ---------------------------------------------------------------------------
# C-code templates
# ---------------------------------------------------------------------------

_STATIC_HEADER = """\
#include "lwip/apps/fs.h"
#include "lwip/def.h"

#define file_NULL (struct fsdata_file *) NULL

#ifndef FS_FILE_FLAGS_HEADER_INCLUDED
#define FS_FILE_FLAGS_HEADER_INCLUDED 1
#endif
#ifndef FS_FILE_FLAGS_HEADER_PERSISTENT
#define FS_FILE_FLAGS_HEADER_PERSISTENT 0
#endif
/* FSDATA_FILE_ALIGNMENT: 0=off, 1=by variable, 2=by include */
#ifndef FSDATA_FILE_ALIGNMENT
#define FSDATA_FILE_ALIGNMENT 0
#endif
#ifndef FSDATA_ALIGN_PRE
#define FSDATA_ALIGN_PRE
#endif
#ifndef FSDATA_ALIGN_POST
#define FSDATA_ALIGN_POST
#endif
#if FSDATA_FILE_ALIGNMENT==2
#include "fsdata_alignment.h"
#endif

"""

_DATA_ARRAY_DECL = """\
#if FSDATA_FILE_ALIGNMENT==1
static const unsigned int dummy_align_{name} = 0;
#endif
static const unsigned char FSDATA_ALIGN_PRE data_{name}[] FSDATA_ALIGN_POST = {{
"""

_FILE_STRUCT = """\
const struct fsdata_file file_{path}[] = {{{{
    file_{next_file},
    data_{path},
    data_{path} + {path_size},
    sizeof(data_{path}) - {path_size},
    FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT,
}}}};

"""

_FILE_END = """\
#define FS_ROOT file__index_html
#define FS_NUMFILES {num_files}"""


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def bytes_to_hex_array(data: bytes, line_size: int) -> str:
    """Convert raw bytes to a comma-separated hex literal block."""
    parts = [f"0x{b:02x}" for b in data]
    lines = []
    for i in range(0, len(parts), line_size):
        lines.append(", ".join(parts[i : i + line_size]))
    return ",\n".join(lines)


def path_to_c_name(relative_path: str) -> str:
    """Turn a UNIX-style relative path into a valid C identifier."""
    return relative_path.replace("/", "_").replace(".", "_")


def encode_path(relative_path: str, line_size: int) -> str:
    """Encode the null-terminated path string as a hex literal block."""
    raw = relative_path.encode("ascii") + b"\x00"
    return bytes_to_hex_array(raw, line_size)


# ---------------------------------------------------------------------------
# Core generator
# ---------------------------------------------------------------------------

def generate_fsdata(fs_dir: str, out_path: str, line_size: int, compress: bool) -> None:
    base_dir = os.path.abspath(fs_dir)
    if not os.path.isdir(base_dir):
        sys.exit(f"ERROR: fs directory not found: {base_dir}")

    # Collect all files first so we can build the linked-list in one pass.
    all_files = []
    for root, _dirs, files in os.walk(base_dir):
        for filename in sorted(files):          # sorted for deterministic output
            all_files.append(os.path.join(root, filename))

    if not all_files:
        sys.exit(f"ERROR: no files found in {base_dir}")

    # Ensure the output directory exists
    os.makedirs(out_path, exist_ok=True)
    final_out_file = os.path.join(out_path, "fsdata.c")

    with open(final_out_file, "w", encoding="ascii") as out:
        out.write(_STATIC_HEADER)

        struct_blocks = []
        next_file_name = "NULL"

        for file_path in all_files:
            relative_path = "/" + os.path.relpath(file_path, base_dir).replace("\\", "/")
            c_name = path_to_c_name(relative_path)
            path_bytes_len = len(relative_path) + 1  # include null terminator

            print(f"  Converting  {relative_path}")

            # --- data array declaration ---
            out.write(_DATA_ARRAY_DECL.format(name=c_name))

            # --- path bytes ---
            out.write(encode_path(relative_path, line_size))
            out.write(",\n")  # separator between path and file content

            # --- file content ---
            with open(file_path, "rb") as f:
                raw = f.read()

            payload = gzip.compress(raw, compresslevel=9) if compress else raw
            out.write(bytes_to_hex_array(payload, line_size))
            out.write("\n};\n\n")

            # Accumulate struct for this file
            struct_blocks.append(
                _FILE_STRUCT.format(
                    path=c_name,
                    next_file=next_file_name,
                    path_size=path_bytes_len,
                )
            )
            next_file_name = c_name

        # Write all structs then the footer
        out.writelines(struct_blocks)
        out.write(_FILE_END.format(num_files=len(all_files)))

    print(f"\nDone — {len(all_files)} file(s) written to {final_out_file}")


# ---------------------------------------------------------------------------
# CLI entry-point
# ---------------------------------------------------------------------------

def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Generate a LwIP fsdata.c from a directory of web files.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        "--fs-dir",
        required=True,
        help="Directory that contains the web-server files.",
    )
    parser.add_argument(
        "--out",
        required=True,
        help="Output directory where fsdata.c will be created.",
    )
    parser.add_argument(
        "--line-size",
        type=int,
        default=16,
        help="Number of hex bytes per line in the generated C arrays.",
    )
    parser.add_argument(
        "--no-gzip",
        action="store_true",
        help="Disable gzip compression (embed raw file bytes).",
    )
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()
    generate_fsdata(
        fs_dir=args.fs_dir,
        out_path=args.out,
        line_size=args.line_size,
        compress=not args.no_gzip,
    )
