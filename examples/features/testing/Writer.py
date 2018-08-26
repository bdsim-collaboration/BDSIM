import numpy as _np
import os as _os
import glob as _glob
from pybdsim import Writer as _Writer
import Test
import Globals
import _General

GlobalData = Globals.Globals()


class Writer:
    def __init__(self):
        self._beamFilename = ''
        self._optionsFilename = ''
        self._numFilesWritten = 0
        self._fileNamesWritten = {}

    def _writeToDisk(self, component, filename, machine, test):
        """ Function that writes the test machine to disk."""
        self._numFilesWritten += 1
        gmadname = filename+'.gmad'
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
            if gmadname not in files:
                machine.beam._SetDistribFileName('../../' + test._beamFilename)
                writer.Options.CallExternalFile('../../' + self._optionsFilename)
                writer.WriteMachine(machine, filename, verbose=False)
            _os.chdir('../')
        else:
            files = _glob.glob('*')
            if gmadname not in files:
                machine.beam._SetDistribFileName('../' + self._beamFilename)
                writer.Options.CallExternalFile('../' + self._optionsFilename)
                writer.WriteMachine(machine, filename, verbose=False)
        if component not in self._fileNamesWritten.keys():
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
        if not isinstance(test, Test.Test):
            raise TypeError("test is not a bdsim Test.Test instance.")

        component = test.Component
        
        for keys, values in test.iteritems():
            if values == []:
                test[keys].append(0)
        
        # make directories and loop over components, particles, and energies.
        _General.MakeTestDirs(test)
        if component == 'sbend' or component == 'rbend':
            # self.WriteDipoleTests(test)
            self.WriteFullDipole(test)
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
        elif component == 'gap':
            self.WriteGapTests(test)

    def WriteDriftTests(self, test):
        component = 'drift'
        filename = component + '__' + test.Particle + '__energy_' + _np.str(test.Energy)
    
        for length in test['length']:
            lenName = '__length_' + _np.str(length)
            lenFileName = filename + lenName

            machine = _General.Machine(test.Particle, test._testRobustness)
            machine.AddDrift(name='dr', length=length)
            machine.AddSampler('all')
            machine.AddBeam(_General.GetBeam(test))
            self._writeToDisk(component, lenFileName, machine, test)

    def WriteFullDipole(self, test):
        """ Write dipoles that only vary length, and angle or field. All remaining parameters
            are set to maximum values."""
        def writeDipole(componentName, filename, component, length, angle=None, field=None):
            k1 = test['k1'][-1]
            e1 = test['e1'][-1]
            e2 = test['e2'][-1]
            fint = test['fint'][-1]
            fintx = test['fintx'][-1]
            fintK2 = test['fintK2'][-1]
            fintxK2 = test['fintxK2'][-1]
            hgap = test['hgap'][-1]
            h1 = test['h1'][-1]
            h2 = test['h2'][-1]
            fileName = filename + '__MAXALLDIPOLEPARAMS_'
            machine = _General.Machine(test.Particle, test._testRobustness)
            machine.AddDrift(name='dr1', length=0.2)

            if angle is not None:
                machine.AddDipole(name=componentName, category=component, length=length, angle=angle, k1=k1, e1=e1, e2=e2,
                                  fint=fint, fintx=fintx, fintK2=fintK2, fintxK2=fintxK2, hgap=hgap, h1=h1, h2=h2)
            elif field is not None:
                machine.AddDipole(name=componentName, category=component, length=length, b=field, k1=k1, e1=e1, e2=e2,
                                  fint=fint, fintx=fintx, fintK2=fintK2, fintxK2=fintxK2, hgap=hgap, h1=h1, h2=h2)

            machine.AddDrift(name='dr2', length=0.2)
            machine.AddSampler('dr2')
            machine.AddBeam(_General.GetBeam(test))
            self._writeToDisk(component, fileName, machine, test)

        component = test.Component
        componentName = 'dip1'
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
                    writeDipole(componentName, angleFileName, component, length, angle)

                    # if full test range wanted, calc field from angle
                    if test._useDefaults:
                        bfield = _General.CalcBField(length, angle, test.Energy, test.Particle)
                        fieldName = '__field_' + _np.str(bfield)
                        fieldFileName = lenFileName + fieldName
                        writeDipole(componentName, fieldFileName, component, length, field=bfield)
            elif test['field'] is not None:
                for bfield in test['field']:
                    fieldName = '__field_' + _np.str(bfield)
                    fieldFileName = lenFileName + fieldName
                    writeDipole(componentName, fieldFileName, component, length, field=bfield)

    def WriteDipoleTests(self, test):
        # function to loop over remaining params (kwargs) to save duplication.
        def loopOverDipoleKwargs(componentName, filename, component, length, angle=None, field=None):
            for k1 in test['k1']:
                k1Name = '__k1_'+_np.str(k1)
                k1FileName = filename + k1Name
                for e1 in test['e1']:
                    e1Name = '__e1_'+_np.str(e1)
                    e1FileName = k1FileName + e1Name
                    for e2 in test['e2']:
                        e2Name = '__e2_'+_np.str(e2)
                        e2FileName = e1FileName + e2Name
                        for fint in test['fint']:
                            fintName = '__fint_'+_np.str(fint)
                            fintFileName = e2FileName + fintName
                            for fintx in test['fintx']:
                                fintxName = '__fintx_'+_np.str(fintx)
                                fintxFileName = fintFileName + fintxName
                                for fintK2 in test['fintK2']:
                                    fintK2Name = '__fintK2_'+_np.str(fintK2)
                                    fintK2FileName = fintxFileName + fintK2Name
                                    for fintxK2 in test['fintxK2']:
                                        fintxK2Name = '__fintxK2_'+_np.str(fintxK2)
                                        fintxK2FileName = fintK2FileName + fintxK2Name
                                        for hgap in test['hgap']:
                                            hgapName = '__hgap_'+_np.str(hgap)
                                            hgapFileName = fintxK2FileName + hgapName
                                            for h1 in test['h1']:
                                                h1Name = '__h1_'+_np.str(h1)
                                                h1FileName = hgapFileName + h1Name
                                                for h2 in test['h2']:
                                                    h2Name = '__h2_'+_np.str(h2)
                                                    h2FileName = h1FileName + h2Name

                                                    machine = _General.Machine(test.Particle, test._testRobustness)
                                                    machine.AddDrift(name='dr1', length=0.2)

                                                    if angle is not None:
                                                        machine.AddDipole(name=componentName, category=component, length=length, angle=angle, k1=k1, e1=e1, e2=e2,
                                                                          fint=fint, fintx=fintx, fintK2=fintK2, fintxK2=fintxK2, hgap=hgap, h1=h1, h2=h2)
                                                    elif field is not None:
                                                        machine.AddDipole(name=componentName, category=component, length=length, b=field, k1=k1, e1=e1, e2=e2,
                                                                          fint=fint, fintx=fintx, fintK2=fintK2, fintxK2=fintxK2, hgap=hgap, h1=h1, h2=h2)

                                                    machine.AddDrift(name='dr2', length=0.2)
                                                    machine.AddSampler('dr2')
                                                    machine.AddBeam(_General.GetBeam(test))
                                                    self._writeToDisk(component, h2FileName, machine, test)

        print("WARNING: Running a full test suite varying all dipole parameters will generate\n")
        print("a large number of tests and large amount of output data. Proceed with extreme caution.\n")

        component = test.Component
        componentName = 'dip1'
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
                        bfield = _General.CalcBField(length, angle, test.Energy, test.Particle)
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

                machine = _General.Machine(test.Particle, test._testRobustness)
                machine.AddQuadrupole(name='qd', length=length, k1=k1)
                machine.AddSampler('all')
                machine.AddBeam(_General.GetBeam(test))
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

                machine = _General.Machine(test.Particle, test._testRobustness)
                machine.AddSextupole(name='sx', length=length, k2=k2)
                machine.AddSampler('all')
                machine.AddBeam(_General.GetBeam(test))
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

                machine = _General.Machine(test.Particle, test._testRobustness)
                machine.AddOctupole(name='oc', length=length, k3=k3)
                machine.AddSampler('all')
                machine.AddBeam(_General.GetBeam(test))
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

                machine = _General.Machine(test.Particle, test._testRobustness)
                machine.AddOctupole(name='dc', length=length, k4=k4)
                machine.AddSampler('all')
                machine.AddBeam(_General.GetBeam(test))
                self._writeToDisk(component, k4FileName, machine, test)

    def WriteKickerTests(self, test):
        component = test.Component
        componentName = 'kick'
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

                machine = _General.Machine(test.Particle, test._testRobustness)
                if component == 'hkick':
                    machine.AddHKicker(name=componentName, l=length, angle=kickangle)
                elif component == 'vkick':
                    machine.AddVKicker(name=componentName, l=length, angle=kickangle)
                machine.AddSampler('all')
                machine.AddBeam(_General.GetBeam(test))
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
        def writemachine(knArray, ksArray, length, kslName):
            # convert multientry data type to tuples as pybdsim can only handle knl and ksl as tuples.
            if type(knArray) in GlobalData.multiEntryTypes and (type(knArray) is not tuple):
                knArray = tuple(knArray)
            if type(ksArray) in GlobalData.multiEntryTypes and (type(ksArray) is not tuple):
                ksArray = tuple(ksArray)

            machine = _General.Machine(test.Particle, test._testRobustness)
            if component == 'thinmultipole':
                machine.AddDrift(name='dr1', length=0.5)
                machine.AddThinMultipole(name='mp1', knl=knArray, ksl=ksArray)
                machine.AddDrift(name='dr2', length=0.5)
                machine.AddSampler('dr2')
            elif component == 'multipole':
                machine.AddMultipole(name='mp1', length=length, knl=knArray, ksl=ksArray)
                machine.AddSampler('mp1')
            machine.AddBeam(_General.GetBeam(test))
            self._writeToDisk(component, kslName, machine, test)

        def getName(kArray, skewed=False):
            """ Get the test file name based on the kn and ks component strengths.
                Do not include kn or ks component if it is zero.
                """
            componentsName = '__KNL'
            if skewed:
                componentsName = '__KSL'

            if not type(kArray) in GlobalData.multiEntryTypes:
                if _np.float(kArray) != 0:
                    return componentsName + '_K1_' + _np.str(kArray)
                else:
                    return ''

            for index, klArray in enumerate(kArray):
                if type(klArray) in GlobalData.multiEntryTypes:
                    for knOrder, knValue in enumerate(klArray):
                        if knValue != 0:
                            componentsName += '_K' + _np.str(knOrder + 1) + '_' + _np.str(knValue)
                else:
                    if klArray != 0:
                        componentsName += '_K' + _np.str(index + 1) + '_' + _np.str(klArray)
            if (componentsName == '__KNL') or (componentsName == '__KSL'):
                return ''
            else:
                return componentsName

        def getKSLandWrite(test, knlName, knlArray, length):
            """ Get the ksl component values and write. This is a seperate
                function to remove duplication.
                """
            if type(test['ksl']) in GlobalData.multiEntryTypes:
                if type(test['ksl'][0]) in GlobalData.multiEntryTypes:
                    for kslArrays in test['ksl']:
                        for kslArray in kslArrays:
                            kslName = knlName + getName(kslArray, True)
                            writemachine(knlArray, kslArray, length, kslName)
                else:
                    kslName = knlName + getName(test['ksl'], True)
                    writemachine(knlArray, tuple(test['ksl']), length, kslName)
            else:
                kslName = knlName + getName(test['ksl'], True)
                writemachine(knlArray, tuple(test['ksl']), length, kslName)

        # if the container is multiEntryType...
        if type(test['knl']) in GlobalData.multiEntryTypes:
            # containing multiEntryType containers...
            if type(test['knl'][0]) in GlobalData.multiEntryTypes:
                for knlArrays in test['knl']:
                    # then each of those multiEntryTypes should contain component strengths
                    for knlArray in knlArrays:
                        knlName = filename + getName(knlArray)
                        getKSLandWrite(test, knlName, knlArray, length)
            # containing non-multiEntryType data then it should only contain component strengths
            else:
                knlName = filename + getName(test['knl'])
                knlArray = tuple(test['knl'])
                getKSLandWrite(test, knlName, knlArray, length)
        # otherwise it's just a single component strength.
        else:
            knlName = getName(test['knl'])
            knlArray = tuple(test['knl'])
            getKSLandWrite(test, knlName, knlArray, length)

    def WriteCollimatorTests(self, test):
        component = test.Component
        filename = component + '__' + test.Particle + '__energy_' + _np.str(test.Energy)
        for length in test['length']:
            lenName = '__length_' + _np.str(length)
            lenFileName = filename + lenName
            xsize = GlobalData.paramValues['xcol'][0]
            ysize = GlobalData.paramValues['ycol'][0]
            collFileName = lenFileName + '__x_' + _np.str(xsize) + '__y_' + _np.str(ysize)

            machine = _General.Machine(test.Particle, test._testRobustness)
            if component == 'rcol':
                machine.AddRCol(name='rc1', length=length, xsize=xsize, ysize=ysize)
            if component == 'ecol':
                machine.AddECol(name='ec1', length=length, xsize=xsize, ysize=ysize)
            machine.AddSampler('all')
            machine.AddBeam(_General.GetBeam(test))
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

                machine = _General.Machine(test.Particle, test._testRobustness)
                machine.AddSolenoid(name='sn1', length=length, ks=ks)
                machine.AddSampler('all')
                machine.AddBeam(_General.GetBeam(test))
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

                machine = _General.Machine(test.Particle, test._testRobustness)
                machine.AddRFCavity(name='rc1', length=length, gradient=gradient)
                machine.AddSampler('all')
                machine.AddBeam(_General.GetBeam(test))
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
                  
                    machine = _General.Machine(test.Particle, test._testRobustness)
                    # thickness is fraction of length
                    machine.AddDegrader(name='deg1', length=length, nWedges=numWedges, materialThickness=thickness*length)
                    machine.AddSampler('all')
                    machine.AddBeam(_General.GetBeam(test))
                    self._writeToDisk(component, thicknessFileName, machine, test)

    def WriteMuSpoilerTests(self, test):
        component = 'muspoiler'
        filename = component + '__' + test.Particle + '__energy_' + _np.str(test.Energy)
        for length in test['length']:
            lenName = '__length_' + _np.str(length)
            lenFileName = filename + lenName
            for angle in test['angle']:
                bfield = _General.CalcBField(length, angle, test.Energy, test.Particle)
                fieldName = '__field_' + _np.str(bfield)
                fieldFileName = lenFileName + fieldName

                machine = _General.Machine(test.Particle, test._testRobustness)
                machine.AddDrift(name='dr1', length=length)
                machine.AddSampler('all')
                machine.AddBeam(_General.GetBeam(test))
                self._writeToDisk(component, fieldFileName, machine, test)

    def WriteLaserTests(self, test):
        component = 'laser'
        filename = component + '__' + test.Particle + '__energy_' + _np.str(test.Energy)
        for length in test['length']:
            lenName = '__length_' + _np.str(length)
            lenFileName = filename + lenName
            
            machine = _General.Machine(test.Particle, test._testRobustness)
            machine.AddLaser(name='las', length=length)
            machine.AddSampler('all')
            machine.AddBeam(_General.GetBeam(test))
            self._writeToDisk(component, lenFileName, machine, test)

    def WriteShieldTests(self, test):
        component = 'shield'
        filename = component + '__' + test.Particle + '__energy_' + _np.str(test.Energy)
        for length in test['length']:
            lenName = '__length_' + _np.str(length)
            lenFileName = filename + lenName
            
            machine = _General.Machine(test.Particle, test._testRobustness)
            machine.AddShield(name='sh', length=length)
            machine.AddSampler('all')
            machine.AddBeam(_General.GetBeam(test))
            self._writeToDisk(component, lenFileName, machine, test)

    def WriteGapTests(self, test):
        component = 'gap'
        filename = component + '__' + test.Particle + '__energy_' + _np.str(test.Energy)

        for length in test['length']:
            lenName = '__length_' + _np.str(length)
            lenFileName = filename + lenName

            machine = _General.Machine(test.Particle, test._testRobustness)
            machine.AddDrift(name='dr1', length=length)
            machine.AddGap(name='gp', length=length)
            machine.AddDrift(name='dr2', length=length)
            machine.AddSampler('all')
            machine.AddBeam(_General.GetBeam(test))
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


