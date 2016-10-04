#! /usr/bin/env python

import re
import optparse

def main() :

    usage = "usage: %prog -i mad8.saveline -o filename.gmad"
    parser = optparse.OptionParser(usage)
    
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

    i = open(iFilename,'r')
    o = open(oFilename,'w')

    # line for analysis 
    lta = str()
    # boolian analysis or not
    ba  = False

    objDict = {}

    for l in i : 
        # check if continued line 
        l = l.strip('\n ')
        t = l.split()
        # if last token is an "&" then join the line 
        if len(t) > 0 : 
            if t[0] == '!' and t[-1] != '&' : 
                lta = l
                ba  = False 
                o.write(lta+'\n') 
                continue
            elif t[0].lower() == 'return' : 
                lta = l 
                ba = False 
                o.write(lta.lower()+'\n')
                continue
            elif l[-1] == '&' : 
                lta = lta+l[0:-1] 
                ba  = False 
            elif len(lta) != 0 : 
                lta = lta+l[0:]
                ba  = True
            else :                
                lta = l
                ba  = True
        else : 
            lta = l
            o.write(lta+'\n')
            continue

        if ba : 
#            print 'l  >',l
#            print 'lta>',lta 

            # lower case 
            lta = lta.lower()
            # append semicolon
            lta = lta+';'

            # remove mad bspecific commands (should be saveline so no 
            # problems but...)
            if lta.find('plot')     != -1 : lta = ''
            if lta.find('assign')   != -1 : lta = ''
            if lta.find('envelope') != -1 : lta = ''
            if lta.find('twiss')    != -1 : lta = ''            
            if lta.find('survey')   != -1 : lta = ''
            if lta.find('title')    != -1 : lta = ''
            if lta.find('option')   != -1 : lta = ''            
            if lta.find('optics')   != -1 : lta = ''
            if lta.find('btrns')    != -1 : lta = ''
            if lta.find('print')    != -1 : lta = ''
            if lta.find('return')   != -1 : lta = '' 
            if lta.find('saveline') != -1 : lta = ''

            # check for 4 letter commands 
            if lta.find('sben') != -1 and lta.find('sbend') == -1 :
                lta = lta.replace('sben','sbend')
            if lta.find('drif') != -1 and lta.find('drift') == -1 :
                lta = lta.replace('drif','drift')
            if lta.find('sext') != -1 and lta.find('sextupole') == -1 :
                lta = lta.replace('sext','sextupole')
            if lta.find('quad') != -1 and lta.find('quadrupole') == -1 :
                lta = lta.replace('quad','quadrupole')
            if lta.find('octu') != -1 and lta.find('octupole') == -1 :
                lta = lta.replace('octu','octupole')                
            if lta.find('mark') != -1 and lta.find('marker') == -1 :
                lta = lta.replace('mark','marker')
            if lta.find('moni') != -1 and lta.find('monitor') == -1 :
                lta = lta.replace('moni','monitor') 
                
            # change unsuported types 
            lta = lta.replace('monitor','marker');
            lta = lta.replace('wire','marker');
            lta = lta.replace('prof','marker');
                
            # remove aperture parameters
            iap = lta.find('aperture')
            if options.removeAperture and (iap != -1) : 
                lta = lta[0:iap-2]+'; ! removed aperture'
        
            # remove multipoles 
            
            # make name, type dictionary
            name = lta.split(": ")[0]
            rest = lta.split(": ")[1]
            
            # split lines on ,; and remove white space
            spec = [] 
            for s in re.split('[,;=]',rest) : 
                if s != '' : 
                    spec.append(s.strip())
            type = spec[0] 
#            def  = spec[1:]
            
            # add to object dictionary 
            objDict[name] = [type] 

            try : 
                print name,type, objDict[type] 
            except : 
                print ''
                pass 

            if options.removeMultipole and (lta.find('multipole') != -1) : 
                lta = '! removed multipole'+lta
            o.write(lta+'\n')
            # clean up
            ba = False
            lta = str()


    i.close()
    o.close()

if __name__ == "__main__":
    main()
