import os as _os
import subprocess as _subprocess

def main() : 
    energies = [0.94,0.95,0.96,0.97,0.98,0.99,
                1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9,
                2.0,2.1,2.2,2.3,2.4,2.5,2.6,2.7,2.8,2.9,
                3.0,3.5,4.0,4.5,5.0,5.5,6.0,6.5,7.0,8.0,9.0,10.0,
                100,500,1000,2000,3000,4000,5000,
                6000,6500,7000,7500,8000, 8500,9000,9500, 
                10000,12500,25000,37500,50000,75000,100000]

    f = open("output.dat","w")
    
    for energy in energies : 
        print "Running",energy 

        # make run files 
#        makeRunFiles("ftfp_bert_hp hadronic_elastic",energy,0.001,"solidhydrogen",100000)
#        makeRunFiles("ftfp_bert_hp hadronic_elastic",energy,0.01,"carbon",100000)
#        makeRunFiles("ftfp_bert_hp hadronic_elastic",energy,0.01,"aluminium",100000)
#        makeRunFiles("ftfp_bert_hp hadronic_elastic",energy,0.001,"copper",100000)
#        makeRunFiles("ftfp_bert_hp hadronic_elastic",energy,0.001,"tungsten",100000)
        makeRunFiles("ftfp_bert_hp hadronic_elastic",energy,0.001,"lead",100000)
        
#        makeRunFile("ftfp_bert_hp hadronic_elastic",energy,0.001,"solidnitrogen",100000)
#        makeRunFile("ftfp_bert_hp hadronic_elastic",energy,0.001,"solidoxygen",100000)


        # execute bdsim 
        bdsimProc = _subprocess.Popen("bdsim --file=proton.gmad --output=rootevent --batch",
                                      stdout=_subprocess.PIPE,shell=True);
#                                      stdout=_subprocess.PIPE,stderr=_subprocess.STDOUT,shell=True);
        bdsimOutput = bdsimProc.stdout.read()
#        print "-------------------------"
#        print bdsimOutput
#        print "-------------------------"
        # execute root 
        rootProc = _subprocess.Popen("root -b -l evtRates.cc",
                                     stdout=_subprocess.PIPE,stderr=_subprocess.STDOUT,shell=True);
        rootOutput = rootProc.stdout.read()
#        print "-------------------------"
#        print rootOutput
#        print "-------------------------"
        nElastic   = 0 
        nInelastic = 0
        for l in rootOutput.split('\n') : 
            splitLine = l.split();
            if len(splitLine) == 0 : 
                continue
            if splitLine[0] == "event-numbers:" :
                print l
                nElastic   = int(splitLine[1])
                nInelastic = int(splitLine[2])
        f.write(str(energy)+" "+str(nElastic)+" "+str(nInelastic)+"\n");
        f.flush()
                
        _os.system("rm -rf output.root")
    f.close();

def makeRunFiles(modelString, energy, collength,target, ngenerate) :
    optDict = {"__PHYSICSLISTS__":" "+modelString+" "}
    substituteTokens("protonOptions.tpl",optDict)
    
    runDict = {"__ENERGY__":str(energy), 
               "__COLLENGTH__":str(collength),
               "__MATERIAL__":target,
               "__NGENERATE__":str(ngenerate)};
    substituteTokens("proton.tpl",runDict)
    
def substituteTokens(fileName,subDict) :
    inFile = open(fileName)
    
    outFileName = fileName[0:fileName.rfind(".")]+".gmad"
    outFile     = open(outFileName,"w")
    for inLine in inFile : 
        line = inLine
        for key in subDict.keys() : 
            line = line.replace(key,subDict[key])
        outFile.write(line)

    inFile.close()
    outFile.close()
