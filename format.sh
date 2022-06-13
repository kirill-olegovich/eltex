#!/bin/bash

git ls-files *.{c,cpp,h} | xargs clang-format -i -style="{IndentWidth: 4,TabWidth: 4}"
