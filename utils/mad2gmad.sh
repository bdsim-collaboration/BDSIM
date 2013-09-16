#!/bin/bash
# mad8  to gmad converter
# do we need to add the 

usage="Usage : mad2gmad [options] \n
       Options: \n
           -o    - add standard option command "

while getopts "haf:o:" options; do
  case $options in
    a ) include_options="1";;
    o ) output_file=$OPTARG;;
    f ) input_file=$OPTARG;;
    h ) echo -e $usage
         exit 1;;
    \? ) echo -e $usage
         exit 1;;
    * ) echo -e $usage
          exit 1;;
  
  esac
done

#echo "add options=" $include_options
#echo "input=" $input_file
#echo "output=" $output_file

LOWER='qwertyuiopasdfghjklzxcvbnm'
UPPER='QWERTYUIOPASDFGHJKLZXCVBNM'

eval cat $input_file | 
sed '
# remove FORTRAN-like line breaks
s/&//g 

# append semicolons
s/!/;!/g
s/$/;/g

s/[\r]/ /g

# convert to lower case
y/'$UPPER'/'$LOWER'/

#clear empty lines and wrong semicolons
s/^[ \t]*;//g
s/,[ \t]*;/,/g
s/+[ \t]*;/+/g
s/-[ \t]*;/-/g
s/\*[ \t]*;/\*/g

#remove mad-specific commands
/plot*/D
/assign*/D
/envelope*/D
/twiss*/D
/survey*/D
/title*/D
/option*/D
/optics*/D
/bmpm*/D
/btrns*/D
/print*/D
/return*/D

#complement commands for people who are using 4-letter abbreviations
s/sben/sbend/g
s/drif/drift/g
s/sext/sextupole/g
s/quad/quadrupole/g
s/octu/octupole/g
s/mark/marker/g

#rename unsupportes components

s/moni/marker/g
s/monitor/marker/g
s/wire/marker/g
s/prof/marker/g

' > $output_file

