#! /usr/bin/env bash

files=(src/include/*.hh src/lib/*.cc)
if [ "$1" = check ]
then
    clang-format --dry-run -Werror ${files[@]}
else
    clang-format -i ${files[@]}
fi
