import numpy as _np
from scipy import constants as _con
import os as _os
import glob as _glob
from pybdsim import Beam as _Beam
from pybdsim import Writer as _Writer
from pybdsim import Builder as _Builder
import pybdsimTest
import Globals

multiEntryTypes = [tuple, list, _np.ndarray]

GlobalData = Globals.Globals()


class Writer:
    def __init__(self):
        self._beamFilename = ''
        self._optionsFilename = ''
        self._numFilesWritten = 0
        self._fileNamesWritten = {}

    def calcBField(self, length, angle, energy, particle):
        # Calculate the magnetic field for a dipole
        if angle == 0:
            return 0
        else:
            energies = self.calcEnergy(energy, particle)
            rho = length / angle
            b = energies['brho'] / rho
            return b

    def calcEnergy(self, total_energy, particle='e-'):
        # Calculate the energy & momentum of a proton and electron at a given total energy.
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

        res={'e-'     : {'KE': electronKinetic,
                         'TE': electronEnergy,
                         'P' : electronMomentum},
             'proton' : {'KE': protonKinetic,
                         'TE': protonEnergy,
                         'P' : protonMomentum},
             'brho'   : brho}
        return res

    def _getBeam(self, test):
        """ Function to create and return a pybdsim.Beam.Beam instance.
            The beam is set to PTC."""
        beam = _Beam.Beam()
        beam.SetParticleType(particletype=test.Particle)
        beam.SetEnergy(test.Energy)
        beam.SetDistributionType(distrtype='ptc')
        beam._SetSigmaE(sigmae=0)
        return beam

    def _getMachine(self, particle, robust=False):
        if particle == 'e-':
            colLength = 0.01
        elif particle == 'proton':
            colLength = 0.5
        machine = _Builder.Machine()
        if robust:
            machine.AddDrift(name='precr1', length=0.1)
            machine.AddRCol(name='FeCol', length=colLength, xsize=0, ysize=0)
            machine.AddDrift(name='precr2', length=0.1)
        return machine

    def _mkdirs(self, test):
        """ Function to make the directories that the tests will be written in.
            If the number of tests for the component is > 1000, the files will
            be split into multiple subdirectories. """
        component = test.Component  # component type from test object
        if not _os.path.exists(component):
            _os.system("mkdir -p " + component)
        if test._numFiles > 1000:
            _os.chdir(component)
            numdirs = test._numFiles - _np.mod(test._numFiles, 1000)
            dirs = _np.linspace(0, numdirs, (numdirs/1000)+1)
            for direc in dirs:
                _os.system("mkdir -p " + _np.str(_np.int(direc)))
            _os.chdir('../')

    def _writeToDisk(self, component, filename, machine, test):
        """ Function that writes the test machine to disk."""
        self._numFilesWritten += 1
        
        writer = _Writer.Writer()
        writer.Components.WriteInMain()
        writer.Sequence.WriteInMain()
        writer.Samplers.WriteInMain()
        writer.Beam.WriteInMain()
        _os.chdir(component)

        if test._beamFilename[:6] == 'Tests/':
            self.SetBeamFilename(test._beamFilename[6:])
        else:
            self.SetBeamFilename(test._beamFilename)


        if self._optionsFilename == '':
            self._optionsFilename = 'trackingTestOptions.gmad'
        
        # set beam and options path depending on file location.
        # only write files to disk if they don't already exist,
        # pybdsim adds a -1 to the name which means the written file
        # won't be used in the testing.
        if test._numFiles > 1000:
            direc = _np.str(_np.int(self._numFilesWritten - _np.mod(self._numFilesWritten, 1000)))
            _os.chdir(direc)
            files = _glob.glob('*')
            if not files.__contains__(filename+'.gmad'):
                machine.beam._SetDistribFileName('../../' + test._beamFilename)
                writer.Options.CallExternalFile('../../' + self._optionsFilename)
                writer.WriteMachine(machine, filename, verbose=False)
            _os.chdir('../')
        else:
            files = _glob.glob('*')
            if not files.__contains__(filename+'.gmad'):
                machine.beam._SetDistribFileName('../' + self._beamFilename)
                writer.Options.CallExternalFile('../' + self._optionsFilename)
                writer.WriteMachine(machine, filename, verbose=False)
        if not self._fileNamesWritten.keys().__contains__(component):
            self._fileNamesWritten[component] = []
        self._fileNamesWritten[component].append(component + "/" + filename + ".gmad")
        _os.chdir('../')

    def SetBeamFilename(self, beamFilename=''):
        if isinstance(beamFilename, _np.str):
            self._beamFilename = beamFilename
        else:
            raise TypeError("beamFilename must be a string")
        # correct extensions
        if (self._beamFilename[-5:] != '.gmad') and (self._beamFilename[-5:] != '.madx'):
            self._beamFilename += '.madx'

    def SetOptionsFilename(self, optionsFilename=''):
        if isinstance(optionsFilename, _np.str):
            self._optionsFilename = optionsFilename
        else:
            raise TypeError("optionsFilename must be a string")
        # correct extension
        if self._optionsFilename[-5:] != '.gmad':
            self._optionsFilename += '.gmad'

    def WriteTests(self, test):
        """ Function to write all parameter combinations in a Test instance.
            """
        if not isinstance(test, pybdsimTest.Test):
            raise TypeError("test is not a bdsimtesting.pybdsim.Test instance.")

        component = test.Component
        
        for keys, values in test.iteritems():
            if values == []:
                test[keys].append(0)
        
        # make directories and loop over components, particles, and energies.
        self._mkdirs(test)
        if component == 'sbend' or component == 'rbend':
            self.WriteDipoleTests(test)
        elif component == 'hkick' or component == 'vkick':
            self.WriteKickerTests(test)
        elif component == 'rcol' or component == 'ecol':
            self.WriteCollimatorTests(test)
        elif component == 'quadrupole':
            self.WriteQuadrupoleTests(test)
        elif component == 'sextupole':
            self.WriteSextupoleTests(test)
        elif component == 'octupole':
            self.WriteOctupoleTests(test)
        elif component == 'decapole':
            self.WriteDecapoleTests(test)
        elif component == 'multipole':
            self.WriteMultipoleTests(test)
        elif component == 'thinmultipole':
            self.WriteThinMultipoleTests(test)
        elif component == 'drift':
            self.WriteDriftTests(test)
        elif component == 'rfcavity':
            self.WriteRFCavityTests(test)
        elif component == 'degrader':
            self.WriteDegraderTests(test)
        elif component == 'muspoiler':
            self.WriteMuSpoilerTests(test)
        elif component == 'solenoid':
            self.WriteSolenoidTests(test)
        elif component == 'shield':
            self.WriteShieldTests(test)
        elif component == 'laser':
            self.WriteLaserTests(test)

    def WriteDriftTests(self, test):
        component = 'drift'
        filename = component + '__' + test.Particle + '__energy_' + _np.str(test.Energy)
    
        for length in test['length']:
            lenName = '__length_' + _np.str(length)
            lenFileName = filename + lenName

            machine = self._getMachine(test.Particle, test._testRobustness)
            machine.AddDrift(name='dr', length=length)
            machine.AddSampler('all')
            machine.AddBeam(self._getBeam(test))
            self._writeToDisk(component, lenFileName, machine, test)

    def WriteDipoleTests(self, test):
        # function to loop over remaining params (kwargs) to save duplication.
        def loopOverDipoleKwargs(componentName, filename, component, length, angle=None, field=None):
            for e1 in test['e1']:
                e1Name = '__e1_'+_np.str(e1)
                e1FileName = filename + e1Name
                for e2 in test['e2']:
                    e2Name = '__e2_'+_np.str(e2)
                    e2FileName = e1FileName + e2Name
                    for fint in test['fint']:
                        fintName = '__fint_'+_np.str(fint)
                        fintFileName = e2FileName + fintName
                        for fintx in test['fintx']:
                            fintxName = '__fintx_'+_np.str(fintx)
                            fintxFileName = fintFileName + fintxName
                            for hgap in test['hgap']:
                                hgapName = '__hgap_'+_np.str(hgap)
                                hgapFileName = fintxFileName + hgapName
    
                                machine = self._getMachine(test.Particle, test._testRobustness)
                                machine.AddDrift(name='dr1', length=0.2)
                                
                                if angle is not None:
                                    machine.AddDipole(name=componentName, category=component, length=length, angle=angle, e1=e1, e2=e2, fint=fint, fintx=fintx, hgap=hgap)
                                elif field is not None:
                                    machine.AddDipole(name=componentName, category=component, length=length, b=field, e1=e1, e2=e2, fint=fint, fintx=fintx, hgap=hgap)
                                
                                machine.AddDrift(name='dr2', length=0.2)
                                machine.AddSampler('dr2')
                                machine.AddBeam(self._getBeam(test))
                                self._writeToDisk(component, hgapFileName, machine, test)
    
        component = test.Component
        if component == 'rbend':
            componentName = 'rb1'
        elif component == 'sbend':
            componentName = 'sb1'

        filename = component + '__' + test.Particle + '__energy_' + _np.str(test.Energy)
        
        for length in test['length']:
            lenName = '__length_'+_np.str(length)
            lenFileName = filename + lenName
            if test['angle'][0] is not None:
                for angle in test['angle']:
                    angleName = '__angle_' + _np.str(angle)
                    angleFileName = lenFileName + angleName
                    loopOverDipoleKwargs(componentName, angleFileName, component, length, angle)
                    
                    # if full test range wanted, calc field from angle
                    if test._useDefaults:
                        bfield = self.calcBField(length, angle, test.Energy, test.Particle)
                        fieldName = '__field_' + _np.str(bfield)
                        fieldFileName = lenFileName + fieldName
                        loopOverDipoleKwargs(componentName, fieldFileName, component, length, field=bfield)
            elif test['field'] is not None:
                for bfield in test['field']:
                    fieldName = '__field_' + _np.str(bfield)
                    fieldFileName = lenFileName + fieldName
                    loopOverDipoleKwargs(componentName, fieldFileName, component, length, field=bfield)

    def WriteQuadrupoleTests(self, test):
        component = 'quadrupole'
        filename = component + '__' + test.Particle + '__energy_' + _np.str(test.Energy)

        for length in test['length']:
            lenName = '__length_' + _np.str(length)
            lenFileName = filename + lenName
            for k1 in test['k1']:
                k1Name = '__k1_' + _np.str(k1)
                k1FileName = lenFileName + k1Name

                machine = self._getMachine(test.Particle, test._testRobustness)
                machine.AddQuadrupole(name='qd', length=length, k1=k1)
                machine.AddSampler('all')
                machine.AddBeam(self._getBeam(test))
                self._writeToDisk(component, k1FileName, machine, test)
  
    def WriteSextupoleTests(self, test):
        component = 'sextupole'
        filename = component + '__' + test.Particle + '__energy_' + _np.str(test.Energy)
        for length in test['length']:
            lenName = '__length_' + _np.str(length)
            lenFileName = filename + lenName
            for k2 in test['k2']:
                k2Name = '__k2_' + _np.str(k2)
                k2FileName = lenFileName + k2Name

                machine = self._getMachine(test.Particle, test._testRobustness)
                machine.AddSextupole(name='sx', length=length, k2=k2)
                machine.AddSampler('all')
                machine.AddBeam(self._getBeam(test))
                self._writeToDisk(component, k2FileName, machine, test)

    def WriteOctupoleTests(self, test):
        component = 'octupole'
        filename = component + '__' + test.Particle + '__energy_' + _np.str(test.Energy)
        for length in test['length']:
            lenName = '__length_' + _np.str(length)
            lenFileName = filename + lenName
            for k3 in test['k3']:
                k3Name = '__k3_' + _np.str(k3)
                k3FileName = lenFileName + k3Name

                machine = self._getMachine(test.Particle, test._testRobustness)
                machine.AddOctupole(name='oc', length=length, k3=k3)
                machine.AddSampler('all')
                machine.AddBeam(self._getBeam(test))
                self._writeToDisk(component, k3FileName, machine, test)

    def WriteDecapoleTests(self, test):
        component = 'decapole'
        filename = component + '__' + test.Particle + '__energy_' + _np.str(test.Energy)
        for length in test['length']:
            lenName = '__length_' + _np.str(length)
            lenFileName = filename + lenName
            for k4 in test['k4']:
                k4Name = '__k4_' + _np.str(k4)
                k4FileName = lenFileName + k4Name

                machine = self._getMachine(test.Particle, test._testRobustness)
                machine.AddOctupole(name='dc', length=length, k4=k4)
                machine.AddSampler('all')
                machine.AddBeam(self._getBeam(test))
                self._writeToDisk(component, k4FileName, machine, test)

    def WriteKickerTests(self, test):
        component = test.Component
        if component == 'hkick':
            componentName = 'hk1'
        elif component == 'vkick':
            componentName = 'vk1'
        filename = component + '__' + test.Particle + '__energy_' + _np.str(test.Energy)
        for length in test['length']:
            lenName = '__length_' + _np.str(length)
            lenFileName = filename + lenName
            for kickangle in test['kickangle']:
                kickAngleName = '__kickangle_'+_np.str(kickangle)
                kickAngleFileName = lenFileName + kickAngleName

                machine = self._getMachine(test.Particle, test._testRobustness)
                if component == 'hkick':
                    machine.AddHKicker(name=componentName, length=length, angle=kickangle)
                elif component == 'vkick':
                    machine.AddVKicker(name=componentName, length=length, angle=kickangle)
                machine.AddSampler('all')
                machine.AddBeam(self._getBeam(test))
                self._writeToDisk(component, kickAngleFileName, machine, test)

    def WriteThinMultipoleTests(self, test):
        component = 'thinmultipole'
        filename = component + '__' + test.Particle + '__energy_' + _np.str(test.Energy)
        self._multipoleStrengthComponentLoop(0, component, filename, test)

    def WriteMultipoleTests(self, test):
        component = 'multipole'
        filename = component + '__' + test.Particle + '__energy_' + _np.str(test.Energy)
        for length in test['length']:
            lenName = '__length_' + _np.str(length)
            lenFileName = filename + lenName
            self._multipoleStrengthComponentLoop(length, component, lenFileName, test)
            
    def _multipoleStrengthComponentLoop(self, length, component, filename, test):
        """ Function for looping over the multipole components.
            """
        for knl in test['knl']:
            knlComponentsName = '__KNL_'
            if multiEntryTypes.__contains__(type(knl)):
                for knlArray in knl:
                    for knOrder, knValue in enumerate(knlArray):
                        if knValue != 0:
                            knlComponentsName += 'K' + _np.str(knOrder + 1) + '_' + _np.str(knValue)
                    knlName = filename + knlComponentsName
                    for ksl in test['ksl']:
                        kslComponentsName = '__KSL_'
                        for kslArray in ksl:
                            for ksOrder, ksValue in enumerate(kslArray):
                                if ksValue != 0:
                                    kslComponentsName += 'K' + _np.str(ksOrder + 1) + '_' + _np.str(ksValue)
                            kslName = knlName + kslComponentsName
            else:  # must be k1 if only one entry
                if knl != 0:
                    knlComponentsName += 'K1' + '_' + _np.str(knl)
                knlName = filename + knlComponentsName
                for ksl in test['ksl']:
                    kslComponentsName = '__KSL_'
                    if multiEntryTypes.__contains__(type(ksl)):
                        for kslArray in ksl:
                            for ksOrder, ksValue in enumerate(kslArray):
                                if ksValue != 0:
                                    kslComponentsName += 'K' + _np.str(ksOrder + 1) + '_' + _np.str(ksValue)
                            kslName = knlName + kslComponentsName
                    else:
                        ksValue = test['ksl'][0]
                        if ksValue != 0:
                            kslComponentsName += 'K1' + '_' + _np.str(ksValue)
                            kslName = knlName + kslComponentsName
                        else:
                            kslName = knlName
        def writemachine(knArray, ksArray, length):
            machine = self._getMachine(test.Particle, test._testRobustness)
            if component == 'thinmultipole':
                machine.AddDrift(name='dr1', length=0.5)
                machine.AddThinMultipole(name='mp1', knl=knArray, ksl=ksArray)
                machine.AddDrift(name='dr2', length=0.5)
                machine.AddSampler('dr2')
            elif component == 'multipole':
                machine.AddMultipole(name='mp1', length=length, knl=knArray, ksl=ksArray)
                machine.AddSampler('mp1')
            machine.AddBeam(self._getBeam(test))
            self._writeToDisk(component, kslName, machine, test)

        for knl in test['knl']:
            if multiEntryTypes.__contains__(type(knl)):
                for knArray in knl:
                    for ksl in test['ksl']:
                        if multiEntryTypes.__contains__(type(ksl)):
                            for ksArray in ksl:
                                writemachine(knArray, ksArray, length)
                        else:
                            ksArray = (ksl)
                            writemachine(knArray, ksArray, length)
            else:
                knArray = (knl)
                for ksl in test['ksl']:
                    if multiEntryTypes.__contains__(type(ksl)):
                        for ksArray in ksl:
                            writemachine(knArray, ksArray, length)
                    else:
                        ksArray = (ksl)
                        writemachine(knArray, ksArray, length)

    def WriteCollimatorTests(self, test):
        component = test.Component
        filename = component + '__' + test.Particle + '__energy_' + _np.str(test.Energy)
        for length in test['length']:
            lenName = '__length_' + _np.str(length)
            lenFileName = filename + lenName
            xsize = GlobalData.paramValues['xcol'][0]
            ysize = GlobalData.paramValues['ycol'][0]
            collFileName = lenFileName + '_x_' + _np.str(xsize) + '_y_' + _np.str(ysize)

            machine = self._getMachine(test.Particle, test._testRobustness)
            if component == 'rcol':
                machine.AddRCol(name='rc1', length=length, xsize=xsize, ysize=ysize)
            if component == 'ecol':
                machine.AddECol(name='ec1', length=length, xsize=xsize, ysize=ysize)
            machine.AddSampler('all')
            machine.AddBeam(self._getBeam(test))
            self._writeToDisk(component, collFileName, machine, test)

    def WriteSolenoidTests(self, test):
        component = 'solenoid'
        filename = component + '__' + test.Particle + '__energy_' + _np.str(test.Energy)
        for length in test['length']:
            lenName = '__length_' + _np.str(length)
            lenFileName = filename + lenName
            for ks in test['ks']:
                ksName = '__ks_'+_np.str(ks)
                ksFileName = lenFileName + ksName

                machine = self._getMachine(test.Particle, test._testRobustness)
                machine.AddSolenoid(name='sn1', length=length, ks=ks)
                machine.AddSampler('all')
                machine.AddBeam(self._getBeam(test))
                self._writeToDisk(component, ksFileName, machine, test)

    def WriteRFCavityTests(self, test):
        component = 'rfcavity'
        filename = component + '__' + test.Particle + '__energy_' + _np.str(test.Energy)
        for length in test['length']:
            lenName = '__length_' + _np.str(length)
            lenFileName = filename + lenName
            for gradient in test['gradient']:
                gradientName = '__field_' + _np.str(gradient)
                gradientFileName = lenFileName + gradientName

                machine = self._getMachine(test.Particle, test._testRobustness)
                machine.AddRFCavity(name='rc1', length=length, gradient=gradient)
                machine.AddSampler('all')
                machine.AddBeam(self._getBeam(test))
                self._writeToDisk(component, gradientFileName, machine, test)

    def WriteDegraderTests(self, test):
        component = 'degrader'
        filename = component + '__' + test.Particle + '__energy_' + _np.str(test.Energy)
        for length in test['length']:
            lenName = '__length_' + _np.str(length)
            lenFileName = filename + lenName
            for numWedges in test['numWedges']:
                numWedgesName = '__numWedges_' + _np.str(numWedges)
                numWedgesFileName = lenFileName + numWedgesName
                for thickness in test['thickness']:
                    thicknessName = '__thickness_' + _np.str(thickness)
                    thicknessFileName = numWedgesFileName + thicknessName
                  
                    machine = self._getMachine(test.Particle, test._testRobustness)
                    # thickness is fraction of length
                    machine.AddDegrader(name='deg1', length=length, nWedges=numWedges, materialThickness=thickness*length)
                    machine.AddSampler('all')
                    machine.AddBeam(self._getBeam(test))
                    self._writeToDisk(component, thicknessFileName, machine, test)

    def WriteMuSpoilerTests(self, test):
        component = 'muspoiler'
        filename = component + '__' + test.Particle + '__energy_' + _np.str(test.Energy)
        for length in test['length']:
            lenName = '__length_' + _np.str(length)
            lenFileName = filename + lenName
            for angle in test['angle']:
                bfield = self.calcBField(length, angle, test.Energy, test.Particle)
                fieldName = '__field_' + _np.str(bfield)
                fieldFileName = lenFileName + fieldName

                machine = self._getMachine(test.Particle, test._testRobustness)
                machine.AddLaser(name='mu1', length=length, b=bfield)
                machine.AddSampler('all')
                machine.AddBeam(self._getBeam(test))
                self._writeToDisk(component, fieldFileName, machine, test)

    def WriteLaserTests(self, test):
        component = 'laser'
        filename = component + '__' + test.Particle + '__energy_' + _np.str(test.Energy)
        for length in test['length']:
            lenName = '__length_' + _np.str(length)
            lenFileName = filename + lenName
            
            machine = self._getMachine(test.Particle, test._testRobustness)
            machine.AddLaser(name='las', length=length)
            machine.AddSampler('all')
            machine.AddBeam(self._getBeam(test))
            self._writeToDisk(component, lenFileName, machine, test)

    def WriteShieldTests(self, test):
        component = 'shield'
        filename = component + '__' + test.Particle + '__energy_' + _np.str(test.Energy)
        for length in test['length']:
            lenName = '__length_' + _np.str(length)
            lenFileName = filename + lenName
            
            machine = self._getMachine(test.Particle, test._testRobustness)
            machine.AddShield(name='sh', length=length)
            machine.AddSampler('all')
            machine.AddBeam(self._getBeam(test))
            self._writeToDisk(component, lenFileName, machine, test)


#    def _writeCMakeLists(self):
#        ''' Function to write a CMakeLists.txt file with simple_testing for
#            a test file and add_subdirectory where appropriate.'''
#        
#        def getCMakeString(filename):
#            fname = filename.split('.gmad')[0]
#            cmstring = 'simple_testing(' + fname + ' "--file=' + filename + '" '
#            cmstring += '${OVERLAP_CHECK})\r\n'
#            return cmstring
#
#        #cmakelist file for main tracking test directory
#        allTestsCMakeList = open('CMakeLists.txt','w')
#        
#        for element in elements:
#            allTestsCMakeList.write('add_subdirectory('+element+')\r\n')
#            #cmakelist file for this element.
#            elementCMakeList = open('CMakeLists.txt','w')
#            
#            #if tests split into multiple sub-dirs, create cmakelist file in those too.
#            testDirs = _glob.glob('*/')
#            if len(testDirs) > 0:
#                for dir in testDirs:
#                    elementCMakeList.write('add_subdirectory('+dir+')\r\n')
#                    _os.chdir(dir)
#                    
#                    testFiles = _glob.glob('*.gmad')
#                    testCMakeList = open('CMakeLists.txt','w')
#                    for file in testFiles:
#                        testCMakeList.write(getCMakeString(file))
#                    testCMakeList.close()
#                    _os.chdir('../')
#                elementCMakeList.close()
#            else:
#                testFiles = _glob.glob('*.gmad')
#                for file in testFiles:
#                    elementCMakeList.write(getCMakeString(file))
#                elementCMakeList.close()
#        allTestsCMakeList.close()


