class Tester : 
    def __init__(self) : 
        print 'Tester.Tester.__init__>'
        self.optionDict = dict() 
        self.optionDict['beampipeRadius']      = '10 * cm'
        self.optionDict['boxSize']             = '1.9 * m'
        self.optionDict['tunnelRadius']        = '2.0 * m'
        self.optionDict['beampipeThickness']   = '0.5 * cm'
        self.optionDict['chordStepMinimum']    = '0.0000000001 * m'
        self.optionDict['deltaIntersection']   = '0.00000001 * m'
        self.optionDict['deltaChord']          = '0.001 * m'
        self.optionDict['lengthSafety']        = '0.00001 * m'
        self.optionDict['thresholdCutCharged'] = '100 * MeV'
        self.optionDict['thresholdCutPhotons'] = '1000 * KeV'
        self.optionDict['physicsList']         = '"standard"'
        self.optionDict['ngenerate']           = '10'

        self.beamDict = dict()
        self.beamDict['particle']              = '"e-"'
        self.beamDict['energy']                = '1.5405110 * GeV'
        self.beamDict['distrType']             = '"gauss"'
        self.beamDict['sigmaX']                = '0.002 * mm'
        self.beamDict['sigmaY']                = '0.002 * mm'
        self.beamDict['sigmaXp']               = '0.02*0.005'
        self.beamDict['sigamYp']               = '0.02*0.004'
        

    #######################################################################
    # optionDict setting
    #######################################################################
    def setBeamPipeRadius(self,value) : 
        ''' Set Beampipe radius in cm ''' 
        self.optionDict['beampipeRadius'] = str(value)+' * cm'
        
    def setBoxSize(self,value) : 
        ''' Set Boxsize in m '''         
        self.optionDict['boxSize'] = str(value)+' * m'

    def setTunnelRadius(self,value) : 
        ''' Set Tunnel radius in m '''         
        self.optionDict['tunnelRadius'] = str(value)+' * m'

    def setBeamPipeThickness(self,value) : 
        ''' Set Beampipe thickness in cm '''                 
        self.optionDict['beampipThickness'] = str(value)+' * cm'
    
    def setChordStepMinimum(self,value) : 
        ''' Set Chord Step Minimum in m'''                 
        self.optionDict['chordStepMinimum'] = str(value)+' * m'        

    def setDeltaIntersection(self,value) : 
        ''' Set Delta Intersection  in m '''                 
        self.optionDict['deltaIntersection'] = str(value)+' * m'        

    def setDeltaChord(self,value) : 
        ''' Set delta intersection in m '''                 
        self.optionDict['deltaIntersection'] = str(value)+' * m'        

    def setLengthSafety(self,value) : 
        ''' Set Length Safety in m'''                 
        self.optionDict['lengthSafety'] = str(value)+' * m'        

    def setThresoldCutCharged(self,value) : 
        ''' Set Threshold Cut Charged in  MeV '''                 
        self.optionDict['thresholdCutCharged'] = str(value)+' * MeV'        

    def setThresholdCutPhotons(self,value) : 
        ''' Set Beampipe thickness in cm '''                 
        self.optionDict['thresholdCutPhotons'] = str(value)+' * MeV'        

    def setPhysicsList(self,value) : 
        ''' Set Physics list'''                 
        self.optionDict['physicsList'] = '"'+value+'"'

    def setNGenerate(self,value) : 
        ''' Set number of particles to generate''' 
        self.optionDict['ngenerate'] = str(value)

    #######################################################################
    # beamDict setting
    #######################################################################
    def setParticle(self, value) : 
        ''' Set Particle type ''' 
        self.beamDict['particle'] = '"'+value+'"'
    
    def setEnergy(self, value) : 
        ''' Set energy in GeV ''' 
        self.beamDict['energy'] = str(value)+' * GeV'
        
    def setDistributionType(self,value) :
        ''' Set distribution type ''' 
        self.beamDict['distrType'] = value 

    def setSigmaX(self,value) : 
        ''' Set SigmaX in mm ''' 
        self.beamDict['sigmaX'] = str(value)+' * mm';

    def setSigmaY(self,value) : 
        ''' Set SigmaX in mm ''' 
        self.beamDict['sigmaY'] = str(value)+' * mm';

    def setSigmaXp(self,value) : 
        ''' Set SigmaX in mm ''' 
        self.beamDict['sigmaXp'] = str(value);

    def setSigmaXp(self,value) : 
        ''' Set SigmaX in mm ''' 
        self.beamDict['sigmaYp'] = str(value);
            
    #######################################################################
    # Class methods
    #######################################################################

    def writeBeam(self, fileName = 'beamTester.gmad') : 
        f = open(fileName,'w')
        
        for k in self.beamDict : 
            f.write('option, '+k+' = '+self.beamDict[k]+';\n')
        
        f.close()

    def writeOptions(self, fileName = 'optionsTester.gmad') : 
        f = open(fileName,'w')
        
        for k in self.optionDict : 
            f.write('option, '+k+' = '+self.optionDict[k]+';\n')
    
        f.close()    
    
