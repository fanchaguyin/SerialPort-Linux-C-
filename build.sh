#!/bin/bash
set -e

cd Build

cmake ..
make -j$(nproc)
