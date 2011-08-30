#!/bin/bash

./cpplint.py --verbose=4 --filter=-whitespace/tab,-whitespace/end_of_line,-build/include,-readability/streams,-whitespace/braces,-whitespace/comments,-whitespace/labels,-runtime/rtti,-whitespace/line_length,-runtime/explicit $@
