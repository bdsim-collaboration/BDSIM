#!/bin/bash
cat $1 | while read f ; do printf "$RANDOM\t%s\n" "$f"; done | sort -n | cut -f2- | head -n $2 > $3
