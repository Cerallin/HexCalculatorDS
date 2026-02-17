#!/bin/bash

opt_clean=0

while getopts "c" opt; do
    case $opt in
        c)
            opt_clean=1
            ;;
        *)
            exit 1
            ;;
    esac
done


if [ $opt_clean -eq 1 ]; then
    if [ -d "build-test" ]; then
        rm -rf build-test
    fi
fi

mkdir -p build-test
cmake -S tests -B build-test -DCMAKE_BUILD_TYPE=Debug
cmake --build build-test -j $(nproc)
