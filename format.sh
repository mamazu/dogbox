#!/usr/bin/env bash
find . -iname '*.h' -o -iname '*.cpp' | xargs clang-format-3.9 -i
