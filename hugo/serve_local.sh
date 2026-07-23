#!/usr/bin/env bash

hugo server \
    --buildDrafts \
    --disableFastRender \
    --renderToMemory \
    --bind 127.0.0.1 \
    --port 1313
