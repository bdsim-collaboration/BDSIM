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


