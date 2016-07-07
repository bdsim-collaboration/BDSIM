import pylab as _pl
import numpy as _np

# http://pdg.lbl.gov/2015/hadronic-xsections/hadron.html

def compareHadronic(dir="./") :
    plot(dir+"01_hydrogen.dat", 100000,0.001, 8.96,  1.00,"b","H");
    plot(dir+"02_carbon.dat",   100000,0.01 , 1.88, 12.00,"g","C");
    plot(dir+"03_aluminium.dat",100000,0.01 , 2.70, 26.98,"r","Al");
    plot(dir+"04_copper.dat",   100000,0.001, 8.96, 63.55,"c","Cu");
    plot(dir+"05_tungsten.dat", 100000,0.001,19.30,183.84,"m","W");
    plot(dir+"06_lead.dat",     100000,0.001,11.35,207.20,"y","Pb");
    
    # elastic
    pdgEle= loadPdgCrossSection("./rpp2014-pp_elastic.dat")
    _pl.loglog(pdgEle['PLAB(GEV/C)'],pdgEle['SIG(MB)'],"bx",label="PDG el")
    
    # total xsec 
    pdgTot = loadPdgCrossSection("./rpp2014-pp_total.dat")
    _pl.loglog(pdgTot['PLAB(GEV/C)'],pdgTot['SIG(MB)'],"b+", label="PDG tot")    

    _pl.xlim(0.1,1e7)
    _pl.legend(loc=1)


def plot(filename = "hydrogen.dat", nTotProton = 100000,thickness = 0.001, densitygcm3 = 1.0, atomicweight = 1, lc = "b", leg = "Target") :
    barn        = 1E-28
    mbarn       = barn*1E-3
    d           = _np.loadtxt(filename)
    eTot        = d[:,0]
    eBeam       = _np.sqrt(d[:,0]**2-0.938270**2)
    nElastic    = 2*d[:,1]
    nInelastic  = 2*d[:,2]
    nTotal      = nElastic+nInelastic

    amu             = 1.660539040E-27
    densitykgm3     = densitygcm3*1e-3/(0.01)**3
    numberdensitym3 = densitykgm3/(atomicweight*amu)
    numberdensitym2 = thickness*numberdensitym3
    eleXSec         = nElastic/nTotProton/numberdensitym2/mbarn
    totXSec         = nTotal/nTotProton/numberdensitym2/mbarn

    print densitygcm3, densitykgm3, numberdensitym3, numberdensitym2


#    _pl.clf()
    _pl.loglog(eBeam,totXSec,"-"+lc,label=leg+" tot")
    _pl.loglog(eBeam,eleXSec,"--"+lc,label=leg +" el")


    if 0 :
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
