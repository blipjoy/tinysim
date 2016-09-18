#!/usr/bin/env python

# Convert raw RGBA image data to TinyScreen 16-bit BGR image data
# GIMP is capable of writing the raw RGBA format that this script expects:
# * File -> Export As... -> Raw image data (*.data)

import argparse
import os.path
import struct

parser = argparse.ArgumentParser(description="TinyScreen Image Converter")
parser.add_argument("-o", "--output", help="Destination directory or file", required=True)
parser.add_argument("file", help="Raw image file")
args = parser.parse_args()

with open(args.file, "rb") as fin:
    data = fin.read()
    bin = struct.unpack("4s" * (len(data) / 4), data)

    out = []
    for color in bin:
        # 16-bit color BigEndian BGR (5:6:5)
        out.append(
            ((ord(color[2]) & 0xF8) << 8) |
            ((ord(color[1]) & 0xFC) << 3) |
            ((ord(color[0]) & 0xF8) >> 3)
        )

    dest = args.output
    if os.path.isdir(dest):
        (name, ext) = os.path.splitext(os.path.basename(args.file))
        dest = os.path.join(dest, name + ".tfb")

    with open(dest, "wb") as fout:
        fout.write(struct.pack(">" + "H" * len(out), *out))
