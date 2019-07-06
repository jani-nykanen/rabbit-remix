#!/bin/sh
cd "$(dirname "$0")"

# Make folders
if [ ! -d "../assets" ]; then
    mkdir ../assets
fi
if [ ! -d "../assets/bitmaps" ]; then
    mkdir ../assets/bitmaps
fi
if [ ! -d "../assets/maps" ]; then
    mkdir ../assets/maps
fi

# Bitmaps
../tools/bin/png2bin bitmaps/font.png ../assets/bitmaps/font.bin
../tools/bin/png2bin bitmaps/face.png ../assets/bitmaps/face.bin -dither

