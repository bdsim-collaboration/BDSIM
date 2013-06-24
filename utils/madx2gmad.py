#! /usr/bin/env python2.7

import optparse as _optparse
import pymadx.MadX as _MadX

def main() :

    usage = "usage: %prog -i mad8.saveline -o filename.gmad"
    parser = _optparse.OptionParser(usage)
    
    parser.add_option("-i","--input",dest="inputFilename",
                      action="store_true",
                      default=False)
    parser.add_option("-o","--output",dest="outputFilename",
                      action="store_true",
                      default=False)
    parser.add_option("-m","--removeMultipole",dest="removeMultipole",
                      action="store_true",
                      default=False)
    parser.add_option("-a","--removeAperture",dest="removeAperture",
                      action="store_true",
                      default=False)
    
    (options, args) = parser.parse_args()

    if not options.inputFilename : 
        parser.error("must give input filename")
    if not options.outputFilename : 
        parser.error("must give output filename")
    
    iFilename = args[0]
    oFilename = args[1]

    LHC(iFilename,oFilename)

"""
madx2gmad

General (string inputfilename, string outputfilename)
   For converting a general MadX TFS file into a gmad beam line

LHC     (string inputfilename, string outputfilename, string markers)
   Specifically for converting the TFS files from the LHC into a gmad
   beamline.

See individual functions for documentation.

"""

def LHC(ifilename, ofilename ,markers='all') :
    """
    LHC(string inputfilename, string outputfilename, string markers='all')

    examples:
    LHC('twiss.3.5TeV.b1.tfs','lhc_b1_collisions.gmad')
    LHC('twiss.3.5TeV.b2.tfs','lhc_b2_collisions.gmad','irs')

    
    TO BE COMPLETED....
    THE ALL FUNCTIONALITY IS ALSO NOT IMPLEMENTED YET...
    markers can be one of the following and defines where to put
    sample planes in the beam line specifically for BDSIM:
    'all' - TBC - all markers are converted to samples planes (~80)
    'irs' - TBC - only the IP regions
    'ir1' - TBC - several points through IR1
    'ir2' - TBC - several points through IR2
    'ir3' - TBC - several points through IR3
    'ir4' - TBC - several points through IR4
    'ir5' - TBC - several points through IR5
    'ir6' - TBC - several points through IR6
    'ir7' - TBC - several points through IR7
    'ir8' - TBC - several points through IR8
    
    Converts a MadX TFS file into a gmad beam line.
    Elements supported by BDSIM:
    SBEND, QUADRUPOLE, SEXTUPOLE, VKICKER, HKICKER, MARKER, DRIFT

    NOTE: SOLENOIDS & MULIPOLES are currently not supported

    If there are more than 100 elements in a beam line, the beam line
    is split up into chunks of 100 and place in sequence in a line called
    'all'

    To use the output in gmad / BDSIM, use the following lines:

    include outputfilename.gmad
    use, period=all;

    """
    
    o = open(ofilename,'w',0)
    t = _MadX.Twiss(ifilename)
    s = t.data['S'] 
    
    kw    = t.data['KEYWORD'] 
    na    = t.data['NAME'] 
    l     = t.data['L']
    angle = t.data['ANGLE']
    k0l   = t.data['K0L']
    k1l   = t.data['K1L']
    k2l   = t.data['K2L'] 

    i                  = 0 
    elementcount       = 0
    missedelementcount = 0
    names = []
    masternamedict = {}
    
    stw = '' 
    #iterate over each element in the twiss file
    #remember element sequence comes from order of lines in 
    #twiss file so have to deal with everything in order at once
    #to avoid nasty fidling and insertion later...
    for v in s : 
        #prepare string to write for that type of element
        stw = ''
        elementname = na[i].replace('.','_').replace('$','_')
        names.append(elementname)

        #To prevent name degeneracy, add integer suffix if name as appeared
        #before.  If it hasn't, don't bother.  Names held in dictionary
        #check if it's come up before
        if masternamedict.has_key(elementname) == False:
            masternamedict[elementname] = [1]
        else:
            masternamedict[elementname][0] += 1
        #prepare necessary suffix
        if masternamedict[elementname][0] > 1:
            elementname = elementname + '_' + str(masternamedict[elementname][0]-1)
        

        if kw[i] == 'SBEND':
            stw = elementname+': sbend, l='+str(l[i])+'*m, angle='+str(angle[i])
        elif kw[i] == 'RBEND':
            print 'madx2gmad.LHC> warning RBEND not implemented :',elementname
            stw = elementname+': sbend, l='+str(l[i])+'*m, angle='+str(angle[i])
        elif kw[i] == 'QUADRUPOLE':
            #stw = elementname+': quadrupole, l='+str(l[i])+'*m, k1='+str(k1l[i])
            #temporary test of l normalisation - k or K???
            stw = elementname+': quadrupole, l='+str(l[i])+'*m, k1='+str(k1l[i]/l[i])
        elif kw[i] == 'SEXTUPOLE':
            #stw = elementname+': sextupole, l='+str(l[i])+'*m, k2='+str(k2l[i])
            stw = elementname+': sextupole, l='+str(l[i])+'*m, k2='+str(k2l[i]/l[i])
        #elif kw[i] == 'MULTIPOLE':
        #    stw = elementname+': quadrupole, l='+str(l[i])+'*m, k1='+str(k1l[i])
        elif kw[i] == 'VKICKER':
            stw = elementname+': vkick, l='+str(l[i])+'*m'
        elif kw[i] == 'HKICKER':
            stw = elementname+': hkick, l='+str(l[i])+'*m'
        elif kw[i] == 'MARKER':
            stw = elementname+': marker'
        elif kw[i] == 'DRIFT' : 
            stw = elementname+': drift, l='+str(l[i])+'*m'
        elif l[i] != 0:
            #if it's something we can't deal with, but has a length
            #put a drift in
            stw = elementname+': drift, l='+str(l[i])
        
        #tally things up
        if len(stw) != 0:
            stw += ';\n'
            o.write(stw)
            elementcount += 1
        else:
            missedelementcount += 1
        i += 1
        

    linelist = []
    
    ti = 0
    for ministring in Chunks(names,100):
        stw2 = 'l'+str(ti)+': line = ('+', '.join(ministring)+');\n'
        o.write(stw2)
        linelist.append('l'+str(ti))
        ti += 1

    linestring = 'all: line = ('+', '.join(linelist)+');\n'
    o.write(linestring)

    o.write('use, period=all;\n') 
    
    for n in names : 
        o.write('sample, range='+n+';\n')

    o.close()

    print 'madx2gmad.LHC> All done'
    print 'madx2gmad.LHC>',elementcount,'elements written to ',ofilename
    print 'madx2gmad.LHC>',missedelementcount,'elements omitted'
    print 'madx2gmad.LHC> line written in ',len(linelist),' line-lets'

def General(ifilename, ofilename) :
    """
    General(string inputfilename, string outputfilename)

    example:
    General('twiss.3.5Tev.b1.tfs','lhc_b1_collisions.gmad')
    
    Converts a MadX TFS file into a gmad beam line.
    Elements supported by BDSIM:
    SBEND, QUADRUPOLE, SEXTUPOLE, VKICKER, HKICKER, MARKER, DRIFT

    If there are more than 100 elements in a beam line, the beam line
    is split up into chunks of 100 and place in sequence in a line called
    'all'

    To use the output in gmad / BDSIM, use the following lines:

    include outputfilename.gmad
    use, period=all;


    """
    o     = open(ofilename,'w',0)
    t     = _MadX.Twiss(ifilename)

    s     = t.data['S']     
    kw    = t.data['KEYWORD'] 
    na    = t.data['NAME'] 
    l     = t.data['L']
    angle = t.data['ANGLE']
    k0l   = t.data['K0L']
    k1l   = t.data['K1L']
    k2l   = t.data['K2L'] 

    i                  = 0 
    elementcount       = 0
    missedelementcount = 0
    names = []
    
    stw = '' 
    #iterate over each element in the twiss file
    for v in s : 
        #prepare string to write for that type of element
        stw = ''
        elementname = na[i].replace('.','_').replace('$','_')
        names.append(elementname)
        if kw[i] == 'SBEND':
            stw = elementname+': sbend, l='+str(l[i])+', angle='+str(angle[i])
        elif kw[i] == 'RBEND':
            print 'madx2gmad.LHC> warning RBEND not implemented :',elementname
            stw = elementname+': sbend, l='+str(l[i])+', angle='+str(angle[i])
        elif kw[i] == 'QUADRUPOLE':
            stw = elementname+': quadrupole, l='+str(l[i])+', k1='+str(k1l[i])
        elif kw[i] == 'SEXTUPOLE':
            stw = elementname+': sextupole, l='+str(l[i])+', k2='+str(k2l[i])
        #elif kw[i] == 'MULTIPOLE':
        #    stw = elementname+': quadrupole, l='+str(l[i])+', k1='+str(k1l[i])
        elif kw[i] == 'VKICKER':
            stw = elementname+': vkick, l='+str(l[i])
        elif kw[i] == 'HKICKER':
            stw = elementname+': hkick, l='+str(l[i])
        elif kw[i] == 'MARKER':
            stw = elementname+': marker'
        elif kw[i] == 'DRIFT' : 
            stw = elementname+': drift, l='+str(l[i])
        elif l[i] != 0:
            #if it's something we can't deal with but has a length
            #put a drift in
            stw = elementname+': drift, l='+str(l[i])
        
        #tally things up
        if len(stw) != 0:
            stw += ';\n'
            o.write(stw)
            elementcount += 1
        else:
            missedelementcount += 1
            
        i += 1


    linelist = []
    
    ti = 0
    for ministring in Chunks(names,100):
        stw2 = 'l'+str(ti)+': line = ('+', '.join(ministring)+');\n'
        o.write(stw2)
        linelist.append('l'+str(ti))
        ti += 1

    linestring = 'all: line = ('+', '.join(linelist)+');\n'
    o.write(linestring)
    o.close()

    print 'madx2gmad.General> All done'
    print 'madx2gmad.General> ',elementcount,' elements written to ',ofilename
    print 'madx2gmad.General> ',missedelementcount,' elements omitted'
    print 'madx2gmad.General> line written in ',len(linelist),' line-lets'


def Chunks(l, n):
    """ Yield successive n-sized chunks from l.    """
    return [l[i:i+n] for i in range(0,len(l),n)]

if __name__ == "__main__":
    main()
