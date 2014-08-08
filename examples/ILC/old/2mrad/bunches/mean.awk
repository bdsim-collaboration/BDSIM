#!/usr/bin/awk
BEGIN { res=0; n=0 }
{ res=res+$3; }
{ n=n+1; }
# optional - add something to one of the columns
#{ print "11 " $1 " " $2+0.002 " " $3+2.e-3 " " $4 " " $5 " " $6 " " $7 }
END {print "Average="res/n}
