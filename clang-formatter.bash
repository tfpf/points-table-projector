#! /usr/bin/env bash

shopt -s globstar

cd "${0%/*}"
files=(**/*.cc **/*.hh)
if [ "$1" = check ]
then
    clang-format --dry-run -Werror ${files[@]}
else
    clang-format -i ${files[@]}
fi
