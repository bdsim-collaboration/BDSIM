import numpy as _np
from scipy import constants as _con
import os as _os
import glob as _glob
from pybdsim import Beam as _Beam
from pybdsim import Options as _Options
from pybdsim import Writer as _Writer
from pybdsim import Builder as _Builder


elements = ['drift',
            'rbend',
            'sbend',
            'vkick',
            'hkick',
            'quadrupole',
            'sextupole',
            'octopole',
            'decapole',
            'multipole',
            'thinmultipole',
            'rfcavity',
            'rcol',
            'ecol',
            'degrader',
            'muspoiler',
            'shield',
            'solenoid',
            'laser']


class params():
    def __init__(self,includeOptional=False):
        self._includeOptional = includeOptional
        self._setParams()
    
    def _setParams(self):
        #global params applicable to all elements
        #electron energies: ATF2,   DLS,    ILC, CLIC
        #proton energies:   PSI G2, J-PARC, SPS, LHC, FCC
        #Note, PSI G2 is the 250MeV k.e. proton medical gantry 2.s
        self.globalParams = {'particle'       : ['e-','proton'],
                             'electronEnergy' : [1.282,    3,  250, 1500],
                             'protonEnergy'   : [1.188272, 50, 400, 4000, 50000]}
        
        #use same magnet strengths for multipole and thinMultipole components
        self._magStrengths = {'k1'   : 0.025,
                             'k2'   : 5.0,
                             'k3'   : 1.5e3,
                             'k4'   : 6.0e5,
                             'k5'   : 3.0e8}
        
        #tuples for multipole and thinMultipole (all components for given element)
        k1l = [(-1.0 * self._magStrengths['k1']),             (0), (1.0 * self._magStrengths['k1'])]
        k2l = [(0, -1.0 * self._magStrengths['k1']),          (0), (0, 1.0 * self._magStrengths['k1'])]
        k3l = [(0, 0, -1.0 * self._magStrengths['k1']),       (0), (0, 0, 1.0 * self._magStrengths['k1'])]
        k4l = [(0, 0, 0, -1.0 * self._magStrengths['k1']),    (0), (0, 0, 0, 1.0 * self._magStrengths['k1'])]
        k5l = [(0, 0, 0, 0, -1.0 * self._magStrengths['k1']), (0), (0, 0, 0, 0, 1.0 * self._magStrengths['k1'])]
        
        self.elementParams = {'length'    : [0.2, 2.0, 20.0],
                              'angle'     : [-0.3, -0.003, 0, 0.003, 0.3],
                              'kickangle' : [-0.003,0,0.003],
                              'field'     : [0, 0, 0, 0, 0],
                              'e1'        : [-0.3491, -0.003491, 0, 0.003491, 0.3491],
                              'e2'        : [-0.3491, -0.003491, 0, 0.003491, 0.3491],
                              'k1'        : [-1.0 * self._magStrengths['k1'], 0, self._magStrengths['k1']],
                              'k2'        : [-1.0 * self._magStrengths['k2'], 0, self._magStrengths['k2']],
                              'k3'        : [-1.0 * self._magStrengths['k3'], 0, self._magStrengths['k3']],
                              'k4'        : [-1.0 * self._magStrengths['k4'], 0, self._magStrengths['k4']],
                              'knl'       : [k1l,k2l,k3l,k4l,k5l],
                              'ksl'       : [k1l,k2l,k3l,k4l,k5l],
                              'gradient'  : [-20.0, -0.002, 0, 0.002, 20.0],
                              'x(col)'    : [0.05],
                              'y(col)'    : [0.05],
                              'numWedges' : [1, 5, 10],
                              'thickness' : [0.01, 0.1, 1.0], #fraction of element length
                              'ks'        : [-0.3, -0.003, 0, 0.003, 0.3],
                              'fint'      : [0, 1.0/6.0, 0.5],
                              'fintx'     : [0, 1.0/6.0, 0.5],
                              'hgap'      : [0.01, 0.1]}
        
        self._fileNameScheme = {'1'  : [''],
                                '2'  : ['small','large'],
                                '3'  : ['small','med','large'],
                                '3k' : ['neg','zero','pos'], #for kickangle and multipole strengths
                                '5'  : ['negLarge','negSmall','zero','posSmall','posLarge']}
        
        self.hasParams = {  'drift'         : ['length'],
                            'rbend'         : ['length','angle','field','e1','e2','fint','fintx','hgap'],
                            'sbend'         : ['length','angle','field','e1','e2','fint','fintx','hgap'],
                            'vkick'         : ['length','kickangle'],
                            'hkick'         : ['length','kickangle'],
                            'quadrupole'    : ['length','k1'],
                            'sextupole'     : ['length','k2'],
                            'octopole'      : ['length','k3'],
                            'decapole'      : ['length','k4'],
                            'multipole'     : ['length','knl','ksl'],
                            'thinmultipole' : ['knl','ksl'],
                            'rfcavity'      : ['length','gradient'],
                            'rcol'          : ['length'],
                            'ecol'          : ['length'],
                            'degrader'      : ['length','numWedges','thickness'],
                            'muspoiler'     : ['length','field'],
                            'shield'        : ['length'],
                            'solenoid'      : ['length','ks'],
                            'laser'         : ['length']}

        self.numFiles =  {}
        for element in elements:
            self.numFiles[element] = 1
        
        #calculate number of beam variations. Proton and electron calculated seperately.
        numBeamVariations = 0
        for particle in self.globalParams['particle']:
            if particle == 'e-':
                for elecEnergy in self.globalParams['electronEnergy']:
                    numBeamVariations += 1
            if particle == 'proton':
                for elecEnergy in self.globalParams['protonEnergy']:
                    numBeamVariations += 1
        self.numBeamVariations = numBeamVariations
        

        for key in self.hasParams.keys():
            numElementVariations = 1
            params = self.hasParams[key]
            for elementKey in params:
                #multiple by the number component strengths for multipole
                if elementKey == 'knl' or elementKey == 'ksl':
                    numElementVariations *= (len(k1l)*len(self.elementParams[elementKey]))
                else:
                    numElementVariations *= len(self.elementParams[elementKey])

            if key == 'rbend' or key == 'sbend':
                numElementVariations /= 5
                numElementVariations *= 2 #angle or field

            numElementVariations *= self.numBeamVariations

            self.numFiles[key] *= numElementVariations
        
        self.totalFiles = 0
        for key in self.numFiles:
            self.totalFiles += self.numFiles[key]

class _functions(params):

    def calcBField(self,length,angle,energy,particle):
        #Calculate the magnetic field for a dipole
        if angle == 0:
            return 0
        else:
            energies = self.calcEnergy(energy,particle)
            rho = length / angle
            b = energies['brho'] / rho
            return b

    def calcEnergy(self,total_energy,particle='e-'):
        #Calculate the energy & momentum of a proton and electron at a given total energy.
        eMass = _con.electron_mass * _con.c**2 / _con.e / 1e9
        pMass = _con.proton_mass * _con.c**2 / _con.e / 1e9

        if particle == 'proton' or particle == 'p':
            protonEnergy = total_energy
            protonKinetic = total_energy - pMass
            protonMomentum = _np.sqrt(total_energy**2 - pMass**2)
            brho = 3.335640951981521 * protonMomentum
            electronMomentum = protonMomentum
            electronEnergy = _np.sqrt(electronMomentum**2 + eMass**2)
            electronKinetic = electronEnergy - eMass
        elif particle == 'e' or particle == 'e-':
            electronEnergy = total_energy
            electronKinetic = total_energy - eMass
            electronMomentum = _np.sqrt(total_energy**2 - eMass**2)
            brho = 3.335640951981521 * electronMomentum
            protonMomentum = electronMomentum
            protonEnergy = _np.sqrt(protonMomentum**2 + pMass**2)
            protonKinetic = protonEnergy - pMass

        res={'e-'     : {'KE' : electronKinetic,
                         'TE' : electronEnergy,
                         'P'  : electronMomentum},
             'proton' : {'KE' : electronKinetic,
                         'TE' : electronEnergy,
                         'P'  : electronMomentum},
             'brho'   : brho}
        return res

    def _getBeam(self,particle,energy):
        ''' Function to create and return a pybdsim.Beam.Beam instance.
            The beam is set to PTC.'''
        beam = _Beam.Beam()
        beam.SetParticleType(particletype=particle)
        beam.SetEnergy(energy)
        beam.SetDistributionType(distrtype='ptc')
        beam._SetSigmaE(sigmae=0)
        return beam

    def _writeCMakeLists(self):
        ''' Function to write a CMakeLists.txt file with simple_testing for
            a test file and add_subdirectory where appropriate.'''
        
        def getCMakeString(filename):
            fname = filename.split('.gmad')[0]
            cmstring = 'simple_testing(' + fname + ' "--file=' + filename + '" '
            cmstring += '${OVERLAP_CHECK})\r\n'
            return cmstring

        #cmakelist file for main tracking test directory
        allTestsCMakeList = open('CMakeLists.txt','w')
        
        for element in elements:
            allTestsCMakeList.write('add_subdirectory('+element+')\r\n')
            #cmakelist file for this element.
            elementCMakeList = open('CMakeLists.txt','w')
            
            #if tests split into multiple sub-dirs, create cmakelist file in those too.
            testDirs = _glob.glob('*/')
            if len(testDirs) > 0:
                for dir in testDirs:
                    elementCMakeList.write('add_subdirectory('+dir+')\r\n')
                    _os.chdir(dir)
                    
                    testFiles = _glob.glob('*.gmad')
                    testCMakeList = open('CMakeLists.txt','w')
                    for file in testFiles:
                        testCMakeList.write(getCMakeString(file))
                    testCMakeList.close()
                    _os.chdir('../')
                elementCMakeList.close()
            else:
                testFiles = _glob.glob('*.gmad')
                for file in testFiles:
                    elementCMakeList.write(getCMakeString(file))
                elementCMakeList.close()
        allTestsCMakeList.close()

    def _mkdirs(self):
        ''' Function to make the directories that the tests will be written in.
            If the number of tests for a given element is > 1000, the files will
            be split into multiple subdirectories. '''
        for element in elements:
            if not _os.path.exists(element):
                _os.system("mkdir -p " + element)
            if self.numFiles[element] > 1000:
                _os.chdir(element)
                numdirs = self.numFiles[element] - _np.mod(self.numFiles[element],1000)
                dirs = _np.linspace(0,numdirs,(numdirs/1000)+1)
                for dir in dirs:
                    _os.system("mkdir -p " + _np.str(_np.int(dir)))
                _os.chdir('../')

    def _writeToDisk(self,element,filename,machine):
        ''' Function that writes the test machine to disk.'''
        self._testfileNum += 1
        
        writer = _Writer.Writer()
        writer.Components.WriteInMain()
        writer.Sequence.WriteInMain()
        writer.Samplers.WriteInMain()
        writer.Beam.WriteInMain()
        _os.chdir(element)
        #set beam and options path depending on file location.
        if self.numFiles[element] > 1000:
            dir = _np.str(_np.int(self._testfileNum - _np.mod(self._testfileNum,1000)))
            _os.chdir(dir)
            machine.beam._SetDistribFileName('../../trackingTestBeam.madx')
            writer.Options.CallExternalFile('../../trackingTestOptions.gmad')
            writer.WriteMachine(machine, filename,verbose = False)
            _os.chdir('../')
        else:
            machine.beam._SetDistribFileName('../trackingTestBeam.madx')
            writer.Options.CallExternalFile('../trackingTestOptions.gmad')
            writer.WriteMachine(machine, filename,verbose = False)
        _os.chdir('../')


class TrackingTests(_functions):
    def WriteAll(self):
        ''' Function to write all tests.'''
        
        #Element function selector.
        def _writeElement(element,particle,energy):
            ''' Element function selector'''
            if element == 'sbend' or element == 'rbend':
                self.WriteDipoleTests(element,particle,energy)
            elif element == 'hkick' or element == 'vkick':
                self.WriteKickerTests(element,particle,energy)
            if element == 'rcol' or element == 'ecol':
                self.WriteCollimatorTests(element,particle,energy)
            elif element == 'quadrupole':
                self.WriteQuadrupoleTests(particle,energy)
            elif element == 'sextupole':
                self.WriteSextupoleTests(particle,energy)
            elif element == 'octupole':
                self.WriteOctupoleTests(particle,energy)
            elif element == 'decapole':
                self.WriteDecapoleTests(particle,energy)
            elif element == 'multipole':
                self.WriteMultipoleTests(particle,energy)
            elif element == 'thinmultipole':
                self.WriteThinMultipoleTests(particle,energy)
            elif element == 'drift':
                self.WriteDriftTests(particle,energy)
            elif element == 'rfcavity':
                self.WriteRFCavityTests(particle,energy)
            elif element == 'degrader':
                self.WriteDegraderTests(particle,energy)
            elif element == 'muspoiler':
                self.WriteMuSpoilerTests(particle,energy)
            elif element == 'solenoid':
                self.WriteSolenoidTests(particle,energy)
            elif element == 'shield':
                self.WriteShieldTests(particle,energy)
            elif element == 'laser':
                self.WriteLaserTests(particle,energy)
    
        #make directories and loop over elements, particles, and energies.
        self._mkdirs()
        for element in elements:
            print("Writing tests for "+element)
            self._testfileNum = 0
            for particle in self.globalParams['particle']:
                if particle == 'proton':
                    for energy in self.globalParams['protonEnergy']:
                        _writeElement(element,particle,energy)
                if particle == 'e-':
                    for energy in self.globalParams['electronEnergy']:
                        _writeElement(element,particle,energy)
        self._writeCMakeLists() # write cmakelists files.

    def WriteDriftTests(self,particle,energy):
        element = 'drift'
        filename = element+'_'+particle+'_energy_'+_np.str(energy)
    
        for lengthIndex,length in enumerate(self.elementParams['length']):
            lenName = '_length_'+self._fileNameScheme[_np.str(len(self.elementParams['length']))][lengthIndex]
            lenFileName = filename + lenName

            machine = _Builder.Machine()
            machine.AddDrift(name='dr',length=length)
            machine.AddSampler('all')
            self._writeToDisk(element,lenFileName,machine)

    def WriteDipoleTests(self,element,particle,energy):
        #function to loop over remaining params (kwargs) to save duplication.
        def loopOverDipoleKwargs(elementName,filename,element,length,angle=None,field=None):
            for e1Index,e1 in enumerate(self.elementParams['e1']):
                e1Name = '_e1_'+self._fileNameScheme[_np.str(len(self.elementParams['e1']))][e1Index]
                e1FileName = filename + e1Name
                for e2Index,e2 in enumerate(self.elementParams['e2']):
                    e2Name = '_e2_'+self._fileNameScheme[_np.str(len(self.elementParams['e2']))][e2Index]
                    e2FileName = e1FileName + e2Name
                    for fintIndex,fint in enumerate(self.elementParams['fint']):
                        fintName = '_fint_'+self._fileNameScheme[_np.str(len(self.elementParams['fint']))][fintIndex]
                        fintFileName = e2FileName + fintName
                        for fintxIndex,fintx in enumerate(self.elementParams['fintx']):
                            fintxName = '_fintx_'+self._fileNameScheme[_np.str(len(self.elementParams['fintx']))][fintxIndex]
                            fintxFileName = fintFileName + fintxName
                            for hgapIndex,hgap in enumerate(self.elementParams['hgap']):
                                hgapName = '_hgap_'+self._fileNameScheme[_np.str(len(self.elementParams['hgap']))][hgapIndex]
                                hgapFileName = fintxFileName + hgapName
    
                                machine = _Builder.Machine()
                                machine.AddDrift(name='dr1',length=0.5)
                                
                                if angle != None:
                                    machine.AddDipole(name=elementName,category=element,angle=angle,e1=e1,e2=e2,fint=fint,fintx=fintx,hgap=hgap)
                                elif field != None:
                                    machine.AddDipole(name=elementName,category=element,b=field,e1=e1,e2=e2,fint=fint,fintx=fintx,hgap=hgap)
                                
                                machine.AddDrift(name='dr2',length=0.5)
                                machine.AddSampler('all')
                                machine.AddBeam(self._getBeam(particle,energy))
                                self._writeToDisk(element,hgapFileName,machine)
                                
        if element == 'rbend':
            elementName = 'rb1'
        elif element == 'sbend':
            elementName = 'sb1'

        filename = element +'_'+particle+'_energy_'+_np.str(energy)
            
        for lengthIndex,length in enumerate(self.elementParams['length']):
            lenName = '_length_'+self._fileNameScheme[_np.str(len(self.elementParams['length']))][lengthIndex]
            lenFileName = filename + lenName
            for angleIndex,angle in enumerate(self.elementParams['angle']):
                angleName = '_angle_'+self._fileNameScheme[_np.str(len(self.elementParams['angle']))][angleIndex]
                angleFileName = lenFileName + angleName
                loopOverDipoleKwargs(elementName,angleFileName,element,length,angle)

                bfield = self.calcBField(length,angle,energy,particle)
                fieldName = '_field_'+self._fileNameScheme[_np.str(len(self.elementParams['field']))][angleIndex]
                fieldFileName = lenFileName + fieldName
                loopOverDipoleKwargs(elementName,fieldFileName,element,length,field=bfield)

    def WriteQuadrupoleTests(self,particle,energy):
        element='quadrupole'
        filename = element +'_'+particle+'_energy_'+_np.str(energy)

        for lengthIndex,length in enumerate(self.elementParams['length']):
            lenName = '_length_'+self._fileNameScheme[_np.str(len(self.elementParams['length']))][lengthIndex]
            lenFileName = filename + lenName
            for k1Index,k1 in enumerate(self.elementParams['k1']):
                k1Name = '_k1_'+self._fileNameScheme[_np.str(len(self.elementParams['k1']))][k1Index]
                k1FileName = lenFileName + k1Name

                machine = _Builder.Machine()
                machine.AddQuadrupole(name='qd',length=length,k1=k1)
                machine.AddSampler('all')
                machine.AddBeam(self._getBeam(particle,energy))
                self._writeToDisk(element,k1FileName,machine)
  
    def WriteSextupoleTests(self,particle,energy):
        element='sextupole'
        filename = element +'_'+particle+'_energy_'+_np.str(energy)
        for lengthIndex,length in enumerate(self.elementParams['length']):
            lenName = '_length_'+self._fileNameScheme[_np.str(len(self.elementParams['length']))][lengthIndex]
            lenFileName = filename + lenName
            for k2Index,k2 in enumerate(self.elementParams['k2']):
                k2Name = '_k2_'+self._fileNameScheme[_np.str(len(self.elementParams['k2']))][k2Index]
                k2FileName = lenFileName + k2Name

                machine = _Builder.Machine()
                machine.AddSextupole(name='sx',length=length,k2=k2)
                machine.AddSampler('all')
                machine.AddBeam(self._getBeam(particle,energy))
                self._writeToDisk(element,k2FileName,machine)

    def WriteOctupoleTests(self,particle,energy):
        element='octupole'
        filename = element +'_'+particle+'_energy_'+_np.str(energy)
        for lengthIndex,length in enumerate(self.elementParams['length']):
            lenName = '_length_'+self._fileNameScheme[_np.str(len(self.elementParams['length']))][lengthIndex]
            lenFileName = filename + lenName
            for k3Index,k3 in enumerate(self.elementParams['k3']):
                k3Name = '_k3_'+self._fileNameScheme[_np.str(len(self.elementParams['k3']))][k3Index]
                k3FileName = lenFileName + k3Name

                machine = _Builder.Machine()
                machine.AddOctupole(name='oc',length=length,k3=k3)
                machine.AddSampler('all')
                machine.AddBeam(self._getBeam(particle,energy))
                self._writeToDisk(element,k3FileName,machine)

    def WriteDecapoleTests(self,particle,energy):
        element='decapole'
        filename = element +'_'+particle+'_energy_'+_np.str(energy)
        for lengthIndex,length in enumerate(self.elementParams['length']):
            lenName = '_length_'+self._fileNameScheme[_np.str(len(self.elementParams['length']))][lengthIndex]
            lenFileName = filename + lenName
            for k4Index,k4 in enumerate(self.elementParams['k4']):
                k4Name = '_k4_'+self._fileNameScheme[_np.str(len(self.elementParams['k4']))][k4Index]
                k4FileName = lenFileName + k4Name

                machine = _Builder.Machine()
                machine.AddOctupole(name='dc',length=length,k4=k4)
                machine.AddSampler('all')
                machine.AddBeam(self._getBeam(particle,energy))
                self._writeToDisk(element,k4FileName,machine)

    def WriteKickerTests(self,element,particle,energy):
        if element == 'hkick':
            elementName = 'hk1'
        elif element == 'vkick':
            elementName = 'vk1'
        filename = element +'_'+particle+'_energy_'+_np.str(energy)
        for lengthIndex,length in enumerate(self.elementParams['length']):
            lenName = '_length_'+self._fileNameScheme[_np.str(len(self.elementParams['length']))][lengthIndex]
            lenFileName = filename + lenName
            for kickAngleIndex,kickangle in enumerate(self.elementParams['kickangle']):
                kickAngleName = '_kickangle_'+self._fileNameScheme[_np.str(len(self.elementParams['kickangle']))][kickAngleIndex]
                kickAngleFileName = lenFileName + kickAngleName

                machine = _Builder.Machine()
                if element == 'hkick':
                    machine.AddHKicker(name=elementName,length=length,angle=kickangle)
                elif element == 'vkick':
                    machine.AddVKicker(name=elementName,length=length,angle=kickangle)
                machine.AddSampler('all')
                machine.AddBeam(self._getBeam(particle,energy))
                self._writeToDisk(element,kickAngleFileName,machine)

    def WriteThinMultipoleTests(self,particle,energy):
        element='thinmultipole'
        filename = element +'_'+particle+'_energy_'+_np.str(energy)
        for knlIndex,knl in enumerate(self.elementParams['knl']):
            knlName = '_knl_K'+_np.str(knlIndex+1)
            knlFileName = filename + knlName
            for knArrayIndex,knArray in enumerate(knl):
                knArrayName = '_'+self._fileNameScheme['3k'][knArrayIndex]
                knArrayFileName = knlFileName + knArrayName
                for kslIndex,ksl in enumerate(self.elementParams['ksl']):
                    kslName = '_ksl_K'+_np.str(kslIndex+1)
                    kslFileName = knArrayFileName + kslName
                    for ksArrayIndex,ksArray in enumerate(ksl):
                        ksArrayName = '_'+self._fileNameScheme['3k'][ksArrayIndex]
                        ksArrayFileName = kslFileName + ksArrayName
                        
                        machine = _Builder.Machine()
                        machine.AddDrift(name='dr1',length=0.5)
                        machine.AddThinMultipole(name='mp1',knl=knArray,ksl=ksArray)
                        machine.AddDrift(name='dr2',length=0.5)
                        machine.AddSampler('all')
                        machine.AddBeam(self._getBeam(particle,energy))
                        self._writeToDisk(element,ksArrayFileName,machine)

    def WriteMultipoleTests(self,particle,energy):
        element='multipole'
        filename = element +'_'+particle+'_energy_'+_np.str(energy)
        for lengthIndex,length in enumerate(self.elementParams['length']):
            lenName = '_length_'+self._fileNameScheme[_np.str(len(self.elementParams['length']))][lengthIndex]
            lenFileName = filename + lenName
            for knlIndex,knl in enumerate(self.elementParams['knl']):
                knlName = '_knl_K'+_np.str(knlIndex+1)
                knlFileName = lenFileName + knlName
                for knArrayIndex,knArray in enumerate(knl):
                    knArrayName = '_'+self._fileNameScheme['3k'][knArrayIndex]
                    knArrayFileName = knlFileName + knArrayName
                    for kslIndex,ksl in enumerate(self.elementParams['ksl']):
                        kslName = '_ksl_K'+_np.str(kslIndex+1)
                        kslFileName = knArrayFileName + kslName
                        for ksArrayIndex,ksArray in enumerate(ksl):
                            ksArrayName = '_'+self._fileNameScheme['3k'][ksArrayIndex]
                            ksArrayFileName = kslFileName + ksArrayName
                            
                            machine = _Builder.Machine()
                            machine.AddMultipole(name='mp1',length=length,knl=knArray,ksl=ksArray)
                            machine.AddSampler('all')
                            machine.AddBeam(self._getBeam(particle,energy))
                            self._writeToDisk(element,ksArrayFileName,machine)

    def WriteCollimatorTests(self,element,particle,energy):
        if element == 'rcol':
            elementName = 'rc1'
        elif element == 'ecol':
            elementName = 'ec1'
        filename = element +'_'+particle+'_energy_'+_np.str(energy)
        for lengthIndex,length in enumerate(self.elementParams['length']):
            lenName = '_length_'+self._fileNameScheme[_np.str(len(self.elementParams['length']))][lengthIndex]
            lenFileName = filename + lenName
            xsize = self.elementParams['x(col)'][0]
            ysize = self.elementParams['y(col)'][0]
            collFileName = lenFileName + '_x_y_open_Large'

            machine = _Builder.Machine()
            if element == 'rcol':
                machine.AddRCol(name='rc1',length=length,xsize=xsize,ysize=ysize)
            if element == 'ecol':
                machine.AddECol(name='ec1',length=length,xsize=xsize,ysize=ysize)
            machine.AddSampler('all')
            machine.AddBeam(self._getBeam(particle,energy))
            self._writeToDisk(element,collFileName,machine)

    def WriteSolenoidTests(self,particle,energy):
        element='solenoid'
        filename = element +'_'+particle+'_energy_'+_np.str(energy)
        for lengthIndex,length in enumerate(self.elementParams['length']):
            lenName = '_length_'+self._fileNameScheme[_np.str(len(self.elementParams['length']))][lengthIndex]
            lenFileName = filename + lenName
            for ksIndex,ks in enumerate(self.elementParams['ks']):
                ksName = '_ks_'+self._fileNameScheme[_np.str(len(self.elementParams['ks']))][ksIndex]
                ksFileName = lenFileName + ksName

                machine = _Builder.Machine()
                machine.AddSolenoid(name='sn1',length=length,ks=ks)
                machine.AddSampler('all')
                machine.AddBeam(self._getBeam(particle,energy))
                self._writeToDisk(element,ksFileName,machine)

    def WriteRFCavityTests(self,particle,energy):
        element='rfcavity'
        filename = element +'_'+particle+'_energy_'+_np.str(energy)
        for lengthIndex,length in enumerate(self.elementParams['length']):
            lenName = '_length_'+self._fileNameScheme[_np.str(len(self.elementParams['length']))][lengthIndex]
            lenFileName = filename + lenName
            for gradientIndex,gradient in enumerate(self.elementParams['gradient']):
                gradientName = '_field_'+self._fileNameScheme[_np.str(len(self.elementParams['gradient']))][gradientIndex]
                gradientFileName = lenFileName + gradientName

                machine = _Builder.Machine()
                machine.AddRFCavity(name='rc1',length=length,gradient=gradient)
                machine.AddSampler('all')
                machine.AddBeam(self._getBeam(particle,energy))
                self._writeToDisk(element,gradientFileName,machine)

    def WriteDegraderTests(self,particle,energy):
        element='degrader'
        filename = element +'_'+particle+'_energy_'+_np.str(energy)
        for lengthIndex,length in enumerate(self.elementParams['length']):
            lenName = '_length_'+self._fileNameScheme[_np.str(len(self.elementParams['length']))][lengthIndex]
            lenFileName = filename + lenName
            for numWedgesIndex,numWedges in enumerate(self.elementParams['numWedges']):
                numWedgesName = '_numWedges_'+self._fileNameScheme[_np.str(len(self.elementParams['numWedges']))][numWedgesIndex]
                numWedgesFileName = lenFileName + numWedgesName
                for thicknessIndex,thickness in enumerate(self.elementParams['thickness']):
                    thicknessName = '_thickness_'+self._fileNameScheme[_np.str(len(self.elementParams['thickness']))][thicknessIndex]
                    thicknessFileName = numWedgesFileName + thicknessName
                  
                    machine = _Builder.Machine()
                    machine.AddDegrader(name='deg1',length=length,nWedges=numWedges,materialThickness=thickness*length) #thickness is fraction of length
                    machine.AddSampler('all')
                    machine.AddBeam(self._getBeam(particle,energy))
                    self._writeToDisk(element,thicknessFileName,machine)

    def WriteMuSpoilerTests(self,particle,energy):
        element='muspoiler'
        filename = element +'_'+particle+'_energy_'+_np.str(energy)
        for lengthIndex,length in enumerate(self.elementParams['length']):
            lenName = '_length_'+self._fileNameScheme[_np.str(len(self.elementParams['length']))][lengthIndex]
            lenFileName = filename + lenName
            for angleIndex,angle in enumerate(self.elementParams['angle']):
                bfield = self.calcBField(length,angle,energy,particle)
                fieldName = '_field_'+self._fileNameScheme[_np.str(len(self.elementParams['field']))][angleIndex]
                fieldFileName = lenFileName + fieldName

                machine = _Builder.Machine()
                machine.AddLaser(name='mu1',length=length,b=bfield)
                machine.AddSampler('all')
                machine.AddBeam(self._getBeam(particle,energy))
                self._writeToDisk(element,fieldFileName,machine)

    def WriteLaserTests(self,particle,energy):
        element='laser'
        filename = element +'_'+particle+'_energy_'+_np.str(energy)
        for lengthIndex,length in enumerate(self.elementParams['length']):
            lenName = '_length_'+self._fileNameScheme[_np.str(len(self.elementParams['length']))][lengthIndex]
            lenFileName = filename + lenName
            
            machine = _Builder.Machine()
            machine.AddLaser(name='las',length=length)
            machine.AddSampler('all')
            machine.AddBeam(self._getBeam(particle,energy))
            self._writeToDisk(element,lenFileName,machine)

    def WriteShieldTests(self,particle,energy):
        element='shield'
        filename = element +'_'+particle+'_energy_'+_np.str(energy)
        for lengthIndex,length in enumerate(self.elementParams['length']):
            lenName = '_length_'+self._fileNameScheme[_np.str(len(self.elementParams['length']))][lengthIndex]
            lenFileName = filename + lenName
            
            machine = _Builder.Machine()
            machine.AddShield(name='sh',length=length)
            machine.AddSampler('all')
            machine.AddBeam(self._getBeam(particle,energy))
            self._writeToDisk(element,lenFileName,machine)
