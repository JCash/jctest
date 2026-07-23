#!/usr/bin/env bash

rm -rf ../docs

hugo --minify

if command -v pngquant >/dev/null 2>&1; then
    ./optimize_images.sh
fi

git add ../docs
