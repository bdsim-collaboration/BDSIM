import numpy as _np
from scipy import constants as _con
import os as _os
import string as _string
import glob as _glob
import Globals
import PhaseSpace
import Writer
from pybdsim import Writer as _pybdsimWriter
from pybdsim import Options as _options

TestParams = Globals.TestParameters()

multiEntryTypes = [tuple,list,_np.ndarray]

class Test(dict):
    def __init__(self,component, energy, particle, phaseSpace=None, useDefaults=False, **kwargs):
        dict.__init__(self)
        self._numFiles = 0
        self._testFiles = []
        self._useDefaults = useDefaults
        
        #Initialise parameters for the component as empty lists (or defaults) and dynamically
        #create setter functions for those component parameters.
        if isinstance(component,_np.str) and (TestParams.components.__contains__(component)):
            self.Component = component
            for param in TestParams.hasParams[component]:
                if not useDefaults:
                    self[param] = []
                else:
                    self.__Update(param,TestParams.paramValues[param])
                funcName = "Set"+_string.capitalize(param)
                setattr(self,funcName,self.__createSetterFunction(name=param))
            
            #set the parameter values to the kwarg values if defaults are not specified
            if not useDefaults:
                for key,value in kwargs.iteritems():
                    if TestParams.hasParams[component].__contains__(key):
                        self.__Update(key,value)
        else:
            raise("Unknown component type.")
        self.SetEnergy(energy)
        self.SetBeamPhaseSpace(phaseSpace)
        self.SetParticleType(particle)
        
    def __createSetterFunction(self,name='',val=[]):
        def function_template(val):
            self.__Update(name,val)
        return function_template

    def __Update(self,parameter,values):
        ''' Function to check the data type is allowable and set dict[parameter] to be those values.
            This allows the input to be multiple data types.
            
            Also, the number of parameter test combinations is recalculated upon the dict being updated.
            '''
        #function to test if entry is numeric. note: is_numlike doesn't work on strings e.g. '3',
        #which we can accept as input, hence this function.
        def _isNumeric(val):
            try:
                floatval = _np.float(val)
            except ValueError:
                raise ValueError("Cannot set "+parameter+" to '"+val+"'.")
            return floatval

        variableValues = []
        #process multiple value types
        if multiEntryTypes.__contains__(type(values)):
            if len(values) > 0:
                for val in values:
                    #if each value is another multiple entry type, set it as the parameter value
                    if multiEntryTypes.__contains__(type(val)):
                        variableValues.append(val)
                    else:
                        #try converting all other dtypes to float
                        val = _isNumeric(val)
                        variableValues.append(val)
                self[parameter] = variableValues
            else:
                pass
        else:
            val = _isNumeric(values)
            variableValues.append(val)
            self[parameter] = variableValues
        self._UpdateTestCombinations()

    def _UpdateTestCombinations(self):
        #update number of parameter combinations
        numcomponentVariations = 1
        for key,values in self.iteritems():
            if key == 'knl' or key == 'ksl':
                numcomponentVariations *= (len(TestParams.k1l)*len(values))
            elif len(values) != 0:
                numcomponentVariations *= len(values)
        if (self.Component == 'rbend' or self.Component == 'sbend') and self._useDefaults:
            numcomponentVariations /= len(TestParams.paramValues['field'])
            numcomponentVariations *= 2 #angle or field
        self._numFiles = numcomponentVariations

    def __repr__(self):
        s  = 'pybdsimTest.testParameters.Test instance.\r\n'
        s += 'This is a test for a/an ' +self.Component+ ' with '+ self.Particle+' at an energy of '+ _np.str(self.Energy)+' GeV.\r\n'
        s += 'The component will be test all combinations of the following parameters:\r\n'
        for param in self.keys():
            s += '  '+param+' : ' + self[param].__repr__()+'\r\n'
        return s

    def SetEnergy(self,energy):
        try:
            self.Energy = _np.float(energy)
        except TypeError:
            raise ValueError("Unknown data type.")

    def SetParticleType(self,particle=''):
        if TestParams.particles.__contains__(particle):
            self.Particle = particle
        else:
            raise ValueError("Unknown particle type")

    def SetBeamPhaseSpace(self,phaseSpace=None, x=0, px=0, y=0, py=0, t=0, pt=0):
        if phaseSpace != None:
            if(isinstance(phaseSpace,PhaseSpace.PhaseSpace)):
                self.PhaseSpace = phaseSpace
            else:
                raise TypeError("phaseSpace can only be a bdsimtesting.PhaseSpace.PhaseSpace instance.")
        else:
            self.PhaseSpace = PhaseSpace.PhaseSpace(x,px,y,py,t,pt)

    def AddParameter(self,parameter,values=[]):
        if self.keys().__contains__(parameter):
            raise ValueError("Parameter is already listed as a test parameter.")
        
        elif isinstance(parameter,_np.str):
            if TestParams.parameters.__contains__(parameter):
                self[parameter] = []
                funcName = "Set"+_string.capitalize(parameter)
                setattr(self,funcName,self.createSetterFunction(name=parameter))
                self.__Update(parameter,values)
            else:
                raise ValueError("Unknown parameter type: "+parameter+".")
        else:
            raise TypeError("Unknown data type for "+parameter)


class TestSuite():
    def __init__(self,directory=''):
        self._tests = []
        if not isinstance(directory,_np.str):
            raise TypeError("Testing directory is not a string")
        else:
            if directory == '':
                directory = 'Test1'
            #check for directory and make it if not:
            if not _os.path.exists(directory):
                _os.system("mkdir " + directory)
            _os.chdir(directory)

        #make dirs for gmad files, bdsimoutput, and failed outputs
        if not _os.path.exists('Tests'):
            _os.system("mkdir Tests")

        if not _os.path.exists('BDSIMOutput'):
            _os.system("mkdir BDSIMOutput")
        _os.chdir('BDSIMOutput')
        if not _os.path.exists('FailedTests'):
            _os.system("mkdir FailedTests")
        _os.chdir('../')
        self.bdsimFailLog = 'bdsimOutputFailures.log'
        self.bdsimPassLog = 'bdsimOutputPassed.log'
        self._comparatorLog = 'comparatorOutput.log'

    def AddTest(self,test):
        if not isinstance(test,Test):
            raise TypeError("Test is not a bdsimtesting.pybdsimTest.Test instance")
        else:
            self._tests.append(test)

    def WriteGmadFiles(self):
        _os.chdir('Tests')
        for test in self._tests:
            writer = Writer.Writer()
            writer.WriteTests(test)
        _os.chdir('../')
    
    def GenerateRootFile(self,file):
        if (file[-5:] != '.gmad'):
            outputfile = file[:-5]
        else:
            outputfile = file
        outputfile = outputfile.split('/')[-1]
        bdsimCommand = "~/bdsim/bdsim_builds/devel/bdsim --file="+file+" --output=rootevent --outfile="+outputfile+" --batch"
        _os.system(bdsimCommand + ' > temp.log')
        files = _glob.glob('*.root')
        outputevent = outputfile+'_event.root'
        if not files.__contains__(outputevent):
            self.UpdateBDSIMFailLog(file)
            print("Output for file "+file+" was not generated.")
            return None
        else:
            _os.system("rm temp.log")
            return outputevent

    def CompareOutput(self,file1,file2):
        comparatorCommand = '~/bdsim/bdsim_builds/devel/comparator/comparator '+ file1 + ' ' + file2
        _os.system(comparatorCommand + ' > tempComp.log')
        hasPassed = self.CheckComparison()
        if hasPassed:
            _os.system("rm "+file1)
            _os.system("rm "+file2)
            _os.system("rm tempComp.log")
            f = open(self.bdsimPassLog,'a')
            f.write("File "+file2+" has passed.\r\n")
            f.close()
        else:
            _os.system("mv "+file2 + " FailedTests/"+file2)
            self.UpdateComparatorLog(file2)

    def Main(self,quickTest=True):
        self.WriteGlobalOptions()
        self.WriteGmadFiles()
        
        _os.chdir('BDSIMOutput')
        testfilesDir = '../Tests/*/'
        componentDirs = _glob.glob(testfilesDir)
        for dir in componentDirs:
            testfileStr = dir+'*.gmad'
            tests = _glob.glob(testfileStr)
            for test in tests:
                outputEvent = self.GenerateRootFile(test)

                if quickTest:
                    copiedEvent = outputEvent.split('_event.root')[0] + '_event2.root'
                    copyString = 'cp '+outputEvent+' '+copiedEvent
                    _os.system(copyString)
                    self.CompareOutput(outputEvent,copiedEvent)

    def CheckComparison(self):
        return True

    def WriteGlobalOptions(self):
        options = _options.Options()
        options.SetSamplerDiameter(3)
        Writer = _pybdsimWriter.Writer()
        Writer.WriteOptions(options,'Tests/trackingTestOptions.gmad')
    
    def UpdateBDSIMFailLog(self,testFileName):
        f = open('temp.log','r')
        g = open(self.bdsimFailLog,'a')
        g.write('\r\n')
        g.write('FAILED TEST FILE: '+testFileName)
        g.write('\r\n')
        for line in f:
            g.write(line)
        g.close()
        
    def UpdateComparatorLog(self,testFileName):
        f = open('tempComp.log','r')
        g = open(self._comparatorLog,'a')
        g.write('\r\n')
        g.write('FAILED TEST FILE: '+testFileName)
        g.write('\r\n')
        for line in f:
            g.write(line)
        g.close()

    def _FullTestSuite(self):
        writer = Writer()
        writer.SetBeamFilename('trackingTestBeam.madx')
        writer.SetOptionsFilename('trackingTestOptions.gmad')

        self.numFiles =  {}
        self.componentTests = []
        for component in TestParams.components:
            self.numFiles[component] = 0
        
        TestPS = TestParams.BeamPhaseSpace
    
        BeamPhaseSpace = PhaseSpace.PhaseSpace(TestPS['X'],TestPS['PX'],TestPS['Y'],TestPS['PY'],TestPS['T'],TestPS['PT'])
        BeamPhaseSpace.Write()
    
        for machineInfo in TestParams.accelerators.values():
            energy = machineInfo['energy']
            particle = machineInfo['particle']
        
            for component in TestParams.components:
                componentTest = Test(component,energy,particle,BeamPhaseSpace,useDefaults=True)
                self.componentTests.append(componentTest)
                self.numFiles[component] += componentTest._numFiles

        self.totalFiles = 0
        for component in self.numFiles:
            self.totalFiles += self.numFiles[component]

        self.BeamPhaseSpace = BeamPhaseSpace





