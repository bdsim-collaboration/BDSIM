#!/bin/bash
if [ -e ~/.cpp2diarc ]
    then
    let DIFFS=`diff .cpp2diarc ~/.cpp2diarc | wc -l`
    if [ $DIFFS -ne 0 ]
	then
	echo "cpp2dia defaults need to be set. Overwrite existing ~/.cpp2diarc? [y/n] followed by enter:"
	read RESP
	if [ "$RESP" == "y" ]
	    then
	    cp .cpp2diarc ~/.cpp2diarc
	fi
    fi
else
    cp .cpp2diarc ~/.cpp2diarc
fi


cd ../
utils/cpp2dia.tclsh
mv dot.dot utils/bdsimGraph.dot
mv output.dia utils/bdsimUMLGraph.dia
cd utils
dia -e bdsimUMLGraph.eps bdsimUMLGraph.dia
twopi -Gnodesep=2 -Granksep=4 -Tps bdsimGraph.dot >! bdsimGraph.ps
gv bdsimGraph.ps&
gv bdsimUMLGraph.eps&

