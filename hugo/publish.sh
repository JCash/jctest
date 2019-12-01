#!/usr/bin/env bash

rm -rf ../docs

./process_images.sh

hugo

./optimize_images.sh

git add ../docs
