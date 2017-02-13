import numpy as _np
from scipy import constants as _con
import os as _os
import string as _string
import glob as _glob
import subprocess as _sub

import Globals
import PhaseSpace
import Writer
from pybdsim import Writer as _pybdsimWriter
from pybdsim import Options as _options

multiEntryTypes = [tuple,list,_np.ndarray]

GlobalData = Globals.Globals()

class Test(dict):
    def __init__(self,component, energy, particle, phaseSpace=None, useDefaults=False, **kwargs):
        dict.__init__(self)
        self._numFiles = 0
        self._testFiles = []
        self._useDefaults = useDefaults
        
        #Initialise parameters for the component as empty lists (or defaults) and dynamically
        #create setter functions for those component parameters.
        if isinstance(component,_np.str) and (GlobalData.components.__contains__(component)):
            self.Component = component
            for param in GlobalData.hasParams[component]:
                if not useDefaults:
                    self[param] = []
                else:
                    self.__Update(param,GlobalData.paramValues[param])
                funcName = "Set"+_string.capitalize(param)
                setattr(self,funcName,self.__createSetterFunction(name=param))
            
            #set the parameter values to the kwarg values if defaults are not specified
            if not useDefaults:
                for key,value in kwargs.iteritems():
                    if GlobalData.hasParams[component].__contains__(key):
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
                numcomponentVariations *= (len(GlobalData.k1l)*len(values))
            elif len(values) != 0:
                numcomponentVariations *= len(values)
        if (self.Component == 'rbend' or self.Component == 'sbend') and self._useDefaults:
            numcomponentVariations /= len(GlobalData.paramValues['field'])
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
        if GlobalData.particles.__contains__(particle):
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
            if GlobalData.parameters.__contains__(parameter):
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
        self._tests = [] #list of test objects
        
        self._testStatus = {} #dict of test statuses
        
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
        ''' Add a bdsimtesting.pybdsimTest.Test instance to the test suite.
            '''
        if not isinstance(test,Test):
            raise TypeError("Test is not a bdsimtesting.pybdsimTest.Test instance")
        else:
            self._tests.append(test)

    def WriteGmadFiles(self):
        ''' Write the gmad files for all tests in the Tests directory.
            '''
        _os.chdir('Tests')
        for test in self._tests:
            writer = Writer.Writer()
            writer.WriteTests(test)
        _os.chdir('../')
    
    def GenerateRootFile(self,file):
        ''' Generate the rootevent output for a given gmad file.
            '''
        #strip extension to leave test name, will be used as output name
        if (file[-5:] != '.gmad'):
            outputfile = file[:-5]
        else:
            outputfile = file
        outputfile = outputfile.split('/')[-1]
        
        #run bdsim and dump output to temporary log file.
        #os.system used as subprocess.call has difficulty with the arguments for some reason.
        bdsimCommand = GlobalData._bdsimExecutable + " --file="+file+" --output=rootevent --outfile="+outputfile+" --batch"
        _os.system(bdsimCommand + ' > temp.log')

        #quick check for output file. If it doesn't exist, update the main failure log and return None.
        #If it does exist, delete the log and return the filename
        files = _glob.glob('*.root')
        outputevent = outputfile+'_event.root'
        if not files.__contains__(outputevent):
            return None
        else:
            _os.system("rm temp.log")
            return outputevent

    def CompareOutput(self,test,originalFile,newFile,isSelfComparison=False):
        ''' Compare the output file against another file. This function uses BDSIM's comparator.
            The test gmad file name is needed for updating the appropriate log files.
            If the comparison is successful:
                The generated root file is deleted.
                The BDSIM pass log file is updated.
            If the comparison is not successful:
                The generated file is moved to the FailedTests directory.
                The Comparator log is updated.
            '''

        outputLog = open("tempComp.log",'w') #temp log file for the comparator output.
        TestResult = _sub.call(args=[GlobalData._comparatorExecutable, originalFile,newFile],stdout=outputLog)
        outputLog.close()
        
        self._testStatus[test] = TestResult #Set dict val to comparator return num.
        #immediate pass/fail bool for decision on keeping root output
        hasPassed = True
        if TestResult != 0:
            hasPassed = False
        
        if hasPassed:
            _os.system("rm "+newFile)
            if isSelfComparison:
                _os.system("rm "+originalFile)
            _os.system("rm tempComp.log")
            f = open(self.bdsimPassLog,'a') #Update the pass log
            f.write("File "+newFile+" has passed.\r\n")
            f.close()
        else:
            if isSelfComparison:
                _os.system("rm "+originalFile)
            _os.system("mv "+newFile + " FailedTests/"+newFile) #move the failed file
            self._UpdateComparatorLog(newFile)
            _os.system("rm tempComp.log")


    def RunTestSuite(self,isSelfComparison=True):
        ''' Run all tests in the test suite. This will generate the tests rootevent 
            output, compares to an original file, and processes the comparison results.
            '''
        self.WriteGlobalOptions()
        self.WriteGmadFiles()   #Write all gmad files for all test objects.
        
        _os.chdir('BDSIMOutput')
        testfilesDir = '../Tests/*/'
        componentDirs = _glob.glob(testfilesDir) #get all component dirs in the Tests dir
        
        for dir in componentDirs:
            testfileStr = dir+'*.gmad'  #get all gmad files in a components dir
            tests = _glob.glob(testfileStr)
            for test in tests:
                outputEvent = self.GenerateRootFile(test)
                
                #Only compare if the output was generated.
                if outputEvent != None:
                    if isSelfComparison:
                        originalEvent = outputEvent.split('_event.root')[0] + '_event2.root'
                        copyString = 'cp '+outputEvent+' '+originalEvent
                        _os.system(copyString)
                    else:
                        pass #This is where the comparison with the original file will occur.
                            #TODO: figure out how to process original files that will be compared to.
                
                    self.CompareOutput(test,originalEvent,outputEvent)
                else:
                    self._UpdateBDSIMFailLog(test)
                    print("Output for test "+test+" was not generated.")
        _os.chdir('../')

    def WriteGlobalOptions(self):
        ''' Write the options file that will be used by all test files.
            '''
        options = _options.Options()
        options.SetSamplerDiameter(3)
        Writer = _pybdsimWriter.Writer()
        Writer.WriteOptions(options,'Tests/trackingTestOptions.gmad')
    
    def _UpdateBDSIMFailLog(self,testFileName):
        ''' Update the test failure log.
            '''
        f = open('temp.log','r')
        g = open(self.bdsimFailLog,'a')
        g.write('\r\n')
        g.write('FAILED TEST FILE: '+testFileName)
        g.write('\r\n')
        for line in f:
            g.write(line)
        g.close()
        
    def _UpdateComparatorLog(self,testFileName):
        ''' Update the test pass log.
            '''
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
        for component in GlobalData.components:
            self.numFiles[component] = 0
        
        TestPS = GlobalData.BeamPhaseSpace
    
        BeamPhaseSpace = PhaseSpace.PhaseSpace(TestPS['X'],TestPS['PX'],TestPS['Y'],TestPS['PY'],TestPS['T'],TestPS['PT'])
        BeamPhaseSpace.Write()
    
        for machineInfo in Globals.accelerators.values():
            energy = machineInfo['energy']
            particle = machineInfo['particle']
        
            for component in GlobalData.components:
                componentTest = Test(component,energy,particle,BeamPhaseSpace,useDefaults=True)
                self.componentTests.append(componentTest)
                self.numFiles[component] += componentTest._numFiles

        self.totalFiles = 0
        for component in self.numFiles:
            self.totalFiles += self.numFiles[component]

        self.BeamPhaseSpace = BeamPhaseSpace





