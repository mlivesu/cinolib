#!/bin/bash

echo "Lines of code in " $1

( find $1 \( -name '*.h' -o -name '*.cpp' \) -print0 | xargs -0 cat ) | wc -l
