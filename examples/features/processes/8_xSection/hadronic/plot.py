import pylab as _pl
import numpy as _np

# http://pdg.lbl.gov/2015/hadronic-xsections/hadron.html

def plot() :
    nTotProton  = 100000
#    thickness   = 0.01
    thickness   = 0.001
    barn        = 1E-28
    mbarn       = barn*1E-3
    d           = _np.loadtxt("output.dat")
    eTot        = d[:,0]
    eBeam       = _np.sqrt(d[:,0]**2-0.938270**2)
    nElastic    = 2*d[:,1]
    nInelastic  = 2*d[:,2]
    nTotal      = nElastic+nInelastic

    atomicweight    = 1.0
    amu             = 1.660539040E-27
    densitygcm3     = 8.96
    densitykgm3     = densitygcm3*1e-3/(0.01)**3
    numberdensitym3 = densitykgm3/(atomicweight*amu)
    numberdensitym2 = thickness*numberdensitym3
    eleXSec         = nElastic/nTotProton/numberdensitym2/mbarn
    totXSec         = nTotal/nTotProton/numberdensitym2/mbarn

    print densitygcm3, densitykgm3, numberdensitym3, numberdensitym2


    _pl.clf()
    _pl.loglog(eBeam,eleXSec,"-b")
    _pl.loglog(eBeam,totXSec,"-g")

    # elastic xsec
    pdgEle= loadPdgCrossSection("./rpp2014-pp_elastic.dat")
    _pl.loglog(pdgEle['PLAB(GEV/C)'],pdgEle['SIG(MB)'],".b")

    # total xsec 
    pdgTot = loadPdgCrossSection("./rpp2014-pp_total.dat")
    _pl.loglog(pdgTot['PLAB(GEV/C)'],pdgTot['SIG(MB)'],".g")

    _pl.grid(True,which="both");
    _pl.xlabel("$P_\mathrm{lab} \;\; [\mathrm{GeV}/{c}]$")
    _pl.ylabel("$\sigma \;\; [\mathrm{mb}]$")

def loadPdgCrossSection(file) :
    import fortranformat as _ff

    f = open(file,'r')
        
    # load header text
    header = True
    headerText = [] 
    while(header) :
        l = f.readline()
        if l.find("---") != -1:
            header = False
        else :
            headerText.append(l.strip('\n\r '))

    # decode header
    nheader = len(headerText) 
    keys    = headerText[4].split()
    ffmt    = headerText[5][headerText[5].find('('):headerText[5].find(')')+1]
#    print nheader
#    print headerText
#    print keys
#    print ffmt

    # read preable
    preamble = {}
    for i in range(0,nheader-2,1) :
        l = f.readline().strip('\r\n ')
        preamble[headerText[i]] = l
#        print headerText[i],l
#    print preamble

    # read data table
    fr = _ff.FortranRecordReader(ffmt)
    data = []
    for i in range(0,int(preamble['NUMBER_OF_DATA_POINTS']),1) :        
        data.append(fr.read(f.readline()))

    data = _np.array(data)
    dataDict = {}

    # format into dict
    for k in keys[0:-1] :     # FLAG not present
        idx = keys.index(k)
        dataDict[k] = data[:,idx]
        
    # close file 
    f.close()
    
    return dataDict
