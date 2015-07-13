#! /usr/bin/env python2.7

import optparse as _optparse
import os as _os
import analysis as _anal
import pylab as _pl

def main() :
    usage = "usage: %prog -t head -b beam -v -a "
    parser = _optparse.OptionParser(usage)    
    parser.add_option("-t","--top",dest="top",
                      action="store_true",
                      default=False)
    parser.add_option("-b","--beam",dest="beam",
                      action="store_true",
                      default=False)
    parser.add_option("-v","--vis",dest="vis",
                      action="store_true",
                      default=False)
    parser.add_option("-a","--anal",dest="ana",
                      action="store_true",
                      default=False)

    (options, args) = parser.parse_args()


    dis = False 
    ana = False 

    if not options.top: 
        parser.error("must give head -h")
    if not options.beam : 
        parser.error("must give beam -b ")
    if options.vis: 
        dis = True
    if options.ana : 
        ana = True

    top  = args[0]
    beam = args[1]

    lat = top+'.gmad'
    vis = top+'.mac'
    bea = beam+'.gmad'

    print 'run.main> ',lat
    print 'run.main> ',bea
    print 'run.main> ',vis

    # link run files 
    _os.system("ln -f "+lat+" lattice.gmad")
    _os.system("ln -f "+bea+" beam.gmad")
    _os.system("ln -f "+vis+" vis.mac")
    

    if dis :
        runCmd = 'echo "exit" | ../../../bdsim-mac/bdsim --file=gmad --vis_mac=vis.mac '
    else : 
        runCmd = '../../../bdsim-mac/bdsim --file=gmad --vis_mac=vis.mac --batch'
    print runCmd 
    oFileName = top+"_"+beam+".txt"
    _os.system(runCmd+" > "+top+".log");    
    _os.system("mv output.txt "+oFileName)

    if ana : 
        a  = _anal.analysis(oFileName,True)
        a.plot(1)
        da = _anal.asciiAnalysis(a._dataList[0])
        da.plotAll(2)

        _pl.draw()
        _pl.show()
        
        
    
if __name__ == "__main__":
    main()
