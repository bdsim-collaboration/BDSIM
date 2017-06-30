#!/usr/bin/python

import os

os.system("rm -rf output_*")
os.system("bdsim --file=io.gmad --batch --output=rootevent --ngenerate=1")
    
