#!/bin/bash

THEME=${THEME:-light}

print_help() {
    cat <<EOF
Usage: $0 [options]
Options:
  -c    Clean build directory before building
  -d    Build debug (default)
  -g    Build release (no debug symbols)
  -h    Show this help message
EOF
}

opt_clean=0
opt_debug=1

while getopts "cdgh" opt; do
    case $opt in
        c)
            opt_clean=1
            ;;
        d)
            opt_debug=1
            ;;
        g)
            opt_debug=0
            ;;
        h)
            print_help
            exit 1
            ;;
        *)
            print_help
            exit 1
            ;;
    esac
done


if [ $opt_clean -eq 1 ]; then
    if [ -d "build" ]; then
        rm -rf build
    fi
fi

if [ $opt_debug -eq 1 ]; then
    build_type="Debug"
else
    build_type="Release"
fi

mkdir -p build
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=$build_type \
    -DCMAKE_TOOLCHAIN_FILE=cmake/devkitarm-toolchain.cmake \
    -DTHEME=$THEME
cmake --build build --config $build_type -j $(nproc)
