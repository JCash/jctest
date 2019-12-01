#!/usr/bin/env bash

PATTERN="-fs8.png"

for f in $(find ../docs -iname "*$PATTERN")
do
    rm $f
done

for f in $(find ../docs -iname "*.png")
do
    echo "Optimizing $f"
    SIZE_BEFORE=$(stat -f %z $f)

    filename=$(basename -- "$f")
    extension="${filename##*.}"
    filename="${filename%.*}"

    NEWFILE="$(dirname -- "$f")/$filename$PATTERN"

    pngquant --quality=65-80 $f

    SIZE_AFTER=$(stat -f %z $NEWFILE)
    echo "   " $SIZE_BEFORE " -> " $SIZE_AFTER
done
