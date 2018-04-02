#!/bin/bash

echo "Lines of code in CinoLib/include: "

( find ../include \( -name '*.h' -o -name '*.cpp' \) -print0 | xargs -0 cat ) | wc -l
