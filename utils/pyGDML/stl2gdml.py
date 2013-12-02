#!/usr/bin/python

import sys, getopt, io
import numpy as _n;

inputfile=''
outputfile='output.gdml'

def cmdArgs(argv):
    global inputfile
    global outputfile
    try:
        opts, args = getopt.getopt(argv,"hi:o:",["inFile=","outFile="])
    except getopt.GetoptError:
        print 'test.py -i <inputfile> -o <outputfile>'
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print 'test.py -i <inputfile> -o <outputfile>'
            sys.exit()
        elif opt in ("-i", "--inFile"):
            inputfile = arg
        elif opt in ("-o", "--outFile"):
            outputfile = arg
    print 'Input file is:', inputfile
    print 'Output file is:', outputfile

def readFile():
    f = open(inputfile,'r')
    for line in f:
        print line,

cmdArgs(sys.argv[1:])
readFile()

