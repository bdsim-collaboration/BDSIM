import numpy as _np
import os as _os
import string as _string
import glob as _glob
import subprocess as _sub
import multiprocessing
import time
import collections
from subprocess import Popen
import threading
import pickle
import socket as _soc

import Globals
import PhaseSpace
import Writer
import TestResults
import _General
from pybdsim import Writer as _pybdsimWriter
from pybdsim import Options as _options


GlobalData = Globals.Globals()

# result utility functions for checking output log files.
# Needed by Run function, so instantiate here.
ResultUtils = TestResults.ResultsUtilities()



def _runBDSIM(inputDict, timeout):

    # Start the BDSIM process
    process = Popen([GlobalData._bdsimExecutable,
                    "--file=" + inputDict['testFile'],
                    "--output=rootevent",
                    "--outfile="+inputDict['outputFile'],
                    "--batch",
                    "--seed=2017"],
                    stdout=open(inputDict['bdsimLogFile'], 'a'),
                    stderr=open(inputDict['bdsimLogFile'], 'a'))

    # Method of communicating with BDSIM process. Start and apply the timeout via joining
    processThread = threading.Thread(target=process.communicate)
    processThread.start()
    processThread.join(timeout)

    # After the timeout, if the process is still alive, kill it.
    if processThread.is_alive():
        process.kill()

    # get process return code. If negative, the process was killed. If positive, it was successful.
    # If None, it means the process is still 'ongoing'.
    returnCode = process.returncode

    # return negative if returnCode is None, its likely that there's a small time delay between
    # the kill signal and the program ending.
    if returnCode is None:
        return -1
    else:
        return returnCode


def Run(inputDict):
    """ Generate the rootevent output for a given gmad file.
        """
    selfCompare = inputDict['selfCompare']
    generateOriginals = inputDict['generateOriginals']

    t = time.time()

    # run bdsim and get return code
    processReturnCode = _runBDSIM(inputDict, timeout=GlobalData.timeout)

    files = _glob.glob('*.root')

    # set code for timeout. Delete root file if it exists, is likely to be zombie.
    if processReturnCode < 0:
        inputDict['code'] = GlobalData.ReturnsAndErrors.GetCode('TIMEOUT')
        inputDict['generalStatus'] = [GlobalData.ReturnsAndErrors.GetCode('TIMEOUT')]
        if inputDict['ROOTFile'] in files:
            _os.remove(inputDict['ROOTFile'])

    bdsimTime = _np.float(time.time() - t)
    inputDict['bdsimTime'] = bdsimTime
    originalFile = inputDict['originalFile']

    # check if the file exists - immediately determines if there was a generation error
    if not inputDict['ROOTFile'] in files:
        if inputDict['code'] != GlobalData.ReturnsAndErrors.GetCode('TIMEOUT'):
            inputDict['code'] = GlobalData.ReturnsAndErrors.GetCode('FILE_NOT_FOUND')  # False
    # if comparing
    elif not generateOriginals:
        with open(inputDict['compLogFile'], 'w') as outputLog:  # temp log file for the comparator output.

            # Only compare if the output was generated.
            if selfCompare:
                originalFile = inputDict['ROOTFile'].split('.root')[0] + '2.root'
                copyString = 'cp ' + inputDict['ROOTFile'] + ' ' + originalFile
                _os.system(copyString)
                if inputDict['originalFile'] == '':
                    inputDict["originalFile"] = originalFile
            else:
                originalFile = inputDict['originalFile']

            # run comparator and record comparator time.
            t = time.time()
            outputLog.write('\r\n')
            TestResult = _sub.call(args=[GlobalData._comparatorExecutable, originalFile, inputDict['ROOTFile']], stdout=outputLog)
            outputLog.close()
            ctime = time.time() - t
            inputDict['compTime'] = ctime

            if TestResult == 0:
                inputDict['code'] = GlobalData.ReturnsAndErrors.GetCode('SUCCESS')  # True
            elif TestResult != 0:
                inputDict['code'] = TestResult  # not passed comparator return code
    elif generateOriginals:
        # else root output was generated otherwise it would've been caught before, therefore success
        inputDict['code'] = GlobalData.ReturnsAndErrors.GetCode('SUCCESS')

    generalStatus = _General.GetBDSIMLogData(inputDict)

    inputDict['generalStatus'] = generalStatus

    # list of soft failure code in the general status.
    hasSofts = [code for code in GlobalData.ReturnsAndErrors.GetSoftCodeNumbers() if code in generalStatus]

    # if the comparator passed
    if inputDict['code'] == 0:
        # if only one general status entry then it must be 0 (passed), in which case,
        # delete all log and root files
        if len(generalStatus) == 1:
            if selfCompare:
                _os.remove(inputDict['originalFile'])
            if not generateOriginals:
                _os.remove(inputDict['compLogFile'])
                _os.remove(inputDict['ROOTFile'])
                _os.remove(inputDict['bdsimLogFile'])

        # else if the general status contains one of the 'soft' failures, move the bdsim log
        # and root output into the failed dir, and delete the passed comparator log.
        elif len(hasSofts) > 0:
            _os.system("mv " + inputDict['bdsimLogFile'] + " FailedTests/" + inputDict['bdsimLogFile'])
            _os.system("mv " + inputDict['ROOTFile'] + " FailedTests/" + inputDict['ROOTFile'])
            if selfCompare:
                _os.remove(inputDict['originalFile'])
            if not generateOriginals:
                _os.remove(inputDict['compLogFile'])

    # elif the comparator failed
    elif inputDict['code'] == 1:
        # move the comparator log and failed root file
        _os.system("mv " + inputDict['ROOTFile'] + " FailedTests/" + inputDict['ROOTFile'])
        if selfCompare:
            _os.remove(inputDict['originalFile'])
        if not generateOriginals:
            _os.system("mv " + inputDict['compLogFile'] + " FailedTests/" + inputDict['compLogFile'])

        # if the general status contains one of the 'soft' failures, move the bdsim log, otherwise delete.
        if len(hasSofts) > 0:
            _os.system("mv " + inputDict['bdsimLogFile'] + " FailedTests/" + inputDict['bdsimLogFile'])
        elif len(generalStatus) == 1:
            _os.remove(inputDict['bdsimLogFile'])

    # elif the comparator couldn't find one of the files
    elif inputDict['code'] == 2 :
        # if the original file that is being compared to doesn't exist, delete ROOT and
        # BDSIM log file, and move the comparator file.
        if (originalFile == '') and (inputDict['ROOTFile'] in files):
            _os.remove(inputDict['bdsimLogFile'])
            _os.remove(inputDict['ROOTFile'])
            _os.system("mv " + inputDict['compLogFile'] + " FailedTests/" + inputDict['compLogFile'])
        else:
            # move bdsim log file to fail dir
            _os.system("mv " + inputDict['bdsimLogFile'] + " FailedTests/" + inputDict['bdsimLogFile'])
        if selfCompare:
            _os.remove(inputDict['originalFile'])

    return inputDict


class TestData(dict):
    """ A class that holds the data for each individual test. This object will be
        passed into the function used in multithreaded testing.
        """
    def __init__(self, testFile='',
                 componentType='',
                 originalFile='',
                 generateOriginals=False,
                 selfCompare=False):
        dict.__init__(self)
        # add test file name, extract test params from filename and add to dict
        if testFile != '':
            self['testFile'] = testFile
            self['testParams'] = collections.OrderedDict()
            splitFile = testFile.split('/')[-1]
            splitFile = splitFile.replace('.gmad', '')
            splitFile = splitFile.replace(componentType + "__", '')
            params = splitFile.split('__')
            self['particle'] = params[0]
            params.remove(params[0])
            for i in params:
                prop = i.split('_')[0]
                value = i.split('_')[1]
                self['testParams'][prop] = value

        GlobalData._CheckComponent(componentType)
        self['componentType'] = componentType
        self['originalFile'] = originalFile  # file to which this test will be compared.

        # strip extension to leave test name, will be used as output name
        if testFile[-5:] == '.gmad':
            outputfile = testFile[:-5]
        else:
            outputfile = testFile
        outputfile = outputfile.split('/')[-1]
        self['outputFile'] = outputfile  # output file base name

        bdsimLogFile = outputfile + "_bdsim.log"
        comparatorLogFile = outputfile + "_comp.log"

        self['bdsimLogFile'] = bdsimLogFile
        self['compLogFile'] = comparatorLogFile

        self['ROOTFile'] = outputfile + '.root'
        self['generateOriginals'] = generateOriginals
        self['selfCompare'] = selfCompare

        self['bdsimTime'] = 0  # bdsim run time
        self['compTime'] = 0  # comparator run time

        self['code'] = None  # comparator return code
        self['generalStatus'] = None  # General status, can contain multiple return codes.
        self['comparatorResults'] = None


class Test(dict):
    """ A class that represents series of tests for a single component type,
        single particle type, and single particle energy.

        Parameters:

        component :  string
            The component type that these tests will represent.

        energy :  float or string
            The energy of the particle that will be used in these tests.

        particle :  string
            The particle type that will be used in these tests

        phaseSpace :  PhaseSpace.PhaseSpace, optional
            An instance of the PhaseSpace.PhaseSpace initial particle coordinates
            used in these tests.

        useDefaults :  bool, optional
            Use the default parameters in Globals.Globals for this element type.
            if useDefaults=True, specifying parameter values after a Test instance has
            been initialised will override the values from Globals.Globals. Default = False.

        testRobustness :  bool, optional
            Test the robustness of these tests. This will place a collimator in front of the
            element being tested for the purpose of generating a particle shower. Default = False.

        eFieldMap :  string, optional
            The file name and path to a Efieldmap that will be attached to this element.
            Currently redundant.

        bFieldMap :  string, optional
            The file name and path to a Bfieldmap that will be attached to this element.
            Currently redundant.

        comparisonFile :  string, optional
            The file name and path to a root file to which the file generated by this test will be
            compared. This can only be specified if this Test instance represents a single test, that
            is that there is only one value specified per component parameter.
     """
    def __init__(self, component, energy, particle, phaseSpace=None,
                 useDefaults=False, testRobustNess=False, eFieldMap='',
                 bFieldMap='', comparisonFile='', **kwargs):
        dict.__init__(self)
        self._numFiles = 0
        self._testFiles = []
        self._useDefaults = useDefaults
        self._testRobustness = testRobustNess
        self.PhaseSpace = None
        self._beamFilename = 'trackingTestBeam.madx'  # default file name
        
        # Initialise parameters for the component as empty lists (or defaults) and dynamically
        # create setter functions for those component parameters.
        if isinstance(component, _np.str) and (component in GlobalData.components):
            self.Component = component
            for param in GlobalData.hasParams[component]:
                if not useDefaults:
                    self[param] = []
                else:
                    self.__Update(param, GlobalData.paramValues[param])
                funcName = "Set" + _string.capitalize(param)
                setattr(self, funcName, self.__createSetterFunction(name=param))
            
            # set the parameter values to the kwarg values if defaults are not specified
            if not useDefaults:
                for key, value in kwargs.iteritems():
                    if key in GlobalData.hasParams[component]:
                        self.__Update(key, value)
        else:
            raise ValueError("Unknown component type.")

        self.SetComparisonFilename(fileName=comparisonFile)
        self.SetEnergy(energy)
        self.SetBeamPhaseSpace(phaseSpace)
        self.SetParticleType(particle)

    def __createSetterFunction(self, name=''):
        """ Function to return function template for updating component parameters.
            """
        def function_template(value):
            self.__Update(name, value)
        return function_template

    def __Update(self, parameter, values):
        """ Function to check the data type is allowable and set dict[parameter] to be those values.
            This allows the input to be multiple data types.
            
            Also, the number of parameter test combinations is recalculated upon the dict being updated.
            """
        # function to test if entry is numeric. note: is_numlike doesn't work on strings e.g. '3',
        # which we can accept as input, hence this function.
        def _isNumeric(number):
            try:
                floatval = _np.float(number)
            except ValueError:
                raise ValueError("Cannot set "+parameter+" to '"+val+"'.")
            return floatval

        variableValues = []
        # process multiple value types
        if type(values) in GlobalData.multiEntryTypes:
            if len(values) > 0:
                for val in values:
                    # if each value is another multiple entry type, set it as the parameter value
                    if type(val) in GlobalData.multiEntryTypes:
                        variableValues.append(val)
                    else:
                        # try converting all other dtypes to float
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
        # update number of parameter combinations
        numcomponentVariations = 1
        for key, values in self.iteritems():
            if key == 'knl' or key == 'ksl':
                numcomponentVariations *= (len(GlobalData.k1l)*len(values))
            elif len(values) != 0:
                numcomponentVariations *= len(values)
        if (self.Component == 'rbend' or self.Component == 'sbend') and self._useDefaults:
            numcomponentVariations /= len(GlobalData.paramValues['field'])
            numcomponentVariations *= 2  # angle or field
        self._numFiles = numcomponentVariations

    def __repr__(self):
        s = 'pybdsimTest.Test instance.\r\n'
        s += 'This is a test for a/an ' + self.Component + ' with ' + self.Particle
        s += ' at an energy of ' + _np.str(self.Energy) + ' GeV.\r\n'
        s += 'The component will be test all combinations of the following parameters:\r\n'
        for param in self.keys():
            s += '  '+param+' : ' + self[param].__repr__()+'\r\n'
        return s

    def SetComparisonFilename(self, fileName=''):
        """ Function to set the filename to which the generated BDSIM output will be compared.
            This filename can only be set if the pybdsim.Test object represent a solitary test.
            """
        if (self._numFiles > 1) and (fileName != ''):
            s = "This pybdsim.Test object is set to generate " + _np.str(self._numFiles) + " output files.\r\n"
            s += "A comparison test file can only be specified if this object represents a solitary test.\r\n"
            print(s)
        elif isinstance(fileName, _np.str) and (fileName != ''):
            # only set filename if the file exists
            startOfFileName = fileName.rfind('/')
            if startOfFileName == -1:
                files = _glob.glob('*')
                fName = startOfFileName
            else:
                files = _glob.glob(fileName[:(startOfFileName + 1)] + '*')
                fName = fileName[(startOfFileName + 1):]
            if fName in files:
                self._testFiles = fileName

    def SetEnergy(self, energy):
        """ Set test beam energy.
            """
        try:
            setattr(self, "Energy", _np.float(energy))
        except TypeError:
            raise ValueError("Unknown data type.")

    def SetParticleType(self, particle=''):
        """ Set test beam particle.
            """
        if particle in GlobalData.particles:
            setattr(self, "Particle", particle)
        else:
            raise ValueError("Unknown particle type")

    def SetBeamPhaseSpace(self, phaseSpace=None, x=0, px=0, y=0, py=0, t=0, pt=0):
        """ Set beam phase space. optional phaseSpace arg must be
            a PhaseSpace instance. Entry of particle co-ordinates instead
            internally creates a PhaseSpace instance anyway."""
        if phaseSpace is not None:
            if isinstance(phaseSpace, PhaseSpace.PhaseSpace):
                self.PhaseSpace = phaseSpace
            else:
                raise TypeError("phaseSpace can only be a PhaseSpace.PhaseSpace instance.")
        else:
            self.PhaseSpace = PhaseSpace.PhaseSpace(x, px, y, py, t, pt)

    def SetInrays(self, inraysFile=''):
        """ Set the filename which contains the ptc inrays file.
            """
        if isinstance(inraysFile, _np.str):
            if inraysFile != '':
                self._beamFilename = inraysFile
            else:
                raise ValueError("inraysFile cannot be an empty string ")
        else:
            raise TypeError("inraysFile must be a string")

    def AddParameter(self, parameter, values=[]):
        """ Function to add a parameter that is not a default parameter for the test's
            component type. An example would be defining a K1 value for a dipole."""
        if parameter in self.keys():
            raise ValueError("Parameter is already listed as a test parameter.")
        
        elif isinstance(parameter, _np.str):
            #check that the parameter can be parsed.
            if parameter in GlobalData.parameters:
                self[parameter] = []
                funcName = "Set" + _string.capitalize(parameter)
                setattr(self, funcName, self.__createSetterFunction(name=parameter))
                self.__Update(parameter, values)
            else:
                raise ValueError("Unknown parameter type: " + parameter + ".")
        else:
            raise TypeError("Unknown data type for " + parameter)

    def WriteToInrays(self, filename):
        """ Write the inrays file to disk. The filename should include the
            filepath relative to the TestSuite directory."""
        self.SetInrays(filename)
        self.PhaseSpace._WriteToInrays(filename)


class TestSuite(object):
    """ A class that is the test suite. Tests are added to this class,
        after which the BDSIM output will be generated, the comparator run,
        and result/plots generated.

        Parameters:

        testingDirectory :  string
            The name of the directory that the test files, output and results will
            be written in.
        dataSetDirectory : string, optional
            The filepath to the directory which contains the data to which the generated
            ROOT file will be compared. The filepath must be relative to the testingDirectory.
        usePickledData :  bool, optional
            Load the results data from pickled files. If True, the data will be loaded in the
            TestResults.Analysis class. Default = False.
        fullTestSuite :  bool, optional
            Automatically run the full test suite, i.e generate Test objects for every
            particle (& energy), and for every component using the default component
            parameters. Default = False.
        """
    def __init__(self, testingDirectory,
                 generateOriginalSet        = False,
                 generateDataSet            = True,
                 originalDataSetDirectory   = '',
                 usePickledData             = False,
                 fullTestSuite              = False,
                 plotResults                = True,
                 analyse                    = True,
                 generateOnly               = True,
                 numThreads                 = -1,
                 debug                      = False):
        self._tests = []      # list of test objects
        self._testNames = {}  # dict of test file names (a list of names per component)
        self._testParamValues = {}
        self._debugOutput = _General.debugFunc(debug)

        selfCompare = False
        if generateOnly:
            selfCompare = True

        numSysCores = multiprocessing.cpu_count()

        if numThreads == -1:
            numCores = numSysCores

            # reduce number of cores by a third on linappserv in case it is run by accident
            if _General.GetHostName() in GlobalData.limitedServers:
                numCores = _np.floor(numCores - (numCores / 3.0))
            self._debugOutput("Using " + _np.str(numCores) + " threads.")
        elif numThreads == 0:
            numCores = 1
            self._debugOutput("Number of threads must be finite. Setting number of threads to 1.")
        else:
            numCores = self._testEnv["numthreads"]
            if numCores > numSysCores:
                numCoreStr = _np.str(numCores)
                numSysStr = _np.str(numSysCores)
                threadError =  "Number of threads specified (" + numCoreStr + ") is greater than the number \n"
                threadError += "available (" + numSysStr + "), setting number of threads to " + numSysStr + "."
                self._debugOutput(threadError)
                numCores = numSysCores

        self._testEnv = {"pickledData"      : usePickledData,
                         "plotResults"      : plotResults,
                         "debug"            : debug,
                         "analyse"          : analyse,
                         "generateOriginals": generateOriginalSet,
                         "generateData"     : generateDataSet,
                         "numThreads"       : numCores,
                         "selfCompare"      : selfCompare,
                         "generateOnly"     : generateOnly
                         }

        # set directory of existing data set that the tests will be compared to
        dataSetDirectory = 'OriginalDataSet'
        if isinstance(originalDataSetDirectory, _np.str):
            dataSetDirectory = dataSetDirectory

        self._directories = {
                            "tests"      : 'Tests',
                            "results"    : 'Results',
                            "dataSet"     : 'GeneratedDataSet',
                            "origSet"    : dataSetDirectory,
                            "failed"     : 'FailedTests'
                            }

        # create testing directory
        if not isinstance(testingDirectory, _np.str):
            raise TypeError("Testing directory is not a string")
        else:
            if testingDirectory == '':
                raise ValueError("Testing directory string cannot be empty")
            # already in directory
            elif _os.getcwd().split('/')[-1] == testingDirectory:
                pass
            else:
                _General.CheckDirExistsElseMake(testingDirectory)

        # cd into main testing directory
        _os.chdir(testingDirectory)

        # make dirs for gmad files, results, bdsim output, and failed outputs
        _General.CheckDirExistsElseMake(self._directories["tests"])
        _General.CheckDirExistsElseMake(self._directories["results"])
        _General.CheckDirExistsElseMake(self._directories["dataSet"])
        _General.CheckDirExistsElseMake(self._directories["origSet"])
        _os.chdir(self._directories["dataSet"])
        _General.CheckDirExistsElseMake(self._directories["failed"])
        _os.chdir("../")

        self.Analysis = TestResults.Analysis()  # results instance

        if fullTestSuite:
            self._FullTestSuite()

    def WriteGmadFiles(self):
        """ Write the gmad files for all tests in the Tests directory.
            """
        _os.chdir('Tests')
        for test in self._tests:
            writer = Writer.Writer()
            writer.WriteTests(test)
            if not test.Component in self._testNames:
                self._testNames[test.Component] = []
            self._testNames[test.Component].extend(writer._fileNamesWritten[test.Component])
        _os.chdir('../')

    def _Run(self, testlist, componentType):
        """ Function that determines the numbers of cores,
            creates the pool, and calls the function for running BDSIM, comparator etc.
            """

        pool = multiprocessing.Pool(self._testEnv["numThreads"])

        # apply results asynchronously
        results = [pool.apply_async(Run, args=(i,)) for i in testlist]
        output = [p.get() for p in results]

        for testRes in output:
            self.Analysis.AddResults(testRes)
            self.Analysis.TimingData.AddComponentTestTime(componentType, testRes)
        pool.close()

    def _loadPickledData(self):
        """ Function to get the pickled data files.
            """
        self._debugOutput("Using pickled data.")
        _os.chdir('BDSIMOutput')
        with open('results.pickle', 'rb') as handle:
            self.Analysis.Results = pickle.load(handle)
        with open('dipoleResults.pickle', 'rb') as handle:
            self.Analysis.DipoleResults = pickle.load(handle)
        with open('timing.pickle', 'rb') as handle:
            self.Analysis.TimingData = pickle.load(handle)
        for comp in GlobalData.components:
            self.Analysis.PlotResults(comp)
        self.Analysis.ProduceReport()

    def AddTest(self, test):
        """ Add a Test instance to the test suite.
            """
        if not isinstance(test, Test):
            raise TypeError("Test is not a Test instance")
        else:
            self._tests.append(test)

    def GenerateOriginalDataSet(self):
        """ Function to generate an original data set from the supplied tests.
            """
        # set _generateOriginals here, generate data, and change back.
        # It's safer to not pass the bool in as an arg, it could be accidentally
        # passed and potentially overwrite an existing data set.
        self._testEnv['generateOriginals'] = True
        if not _os.path.exists('OriginalDataSet'):
            _os.system("mkdir OriginalDataSet")
        self.RunTestSuite()
        self._testEnv['generateOriginals'] = False
        _os.chdir('OriginalDataSet')
        self.Analysis.ProcessOriginals()
        _os.chdir('../')

    def _GenerateDataSet(self, generateOriginals=False, selfCompare=False):
        for componentType in self._testNames.keys():
            self._debugOutput("  Component: "+componentType)
            #testfilesDir = '../Tests/'

            _General.CheckDirExistsElseMake(componentType)
            _os.chdir(componentType)
            _General.CheckDirExistsElseMake("FailedTests")
            testfilesDir = '../../Tests/'

            t = time.time()  # initial time

            # list of file paths for every test of this component type
            tests = [(testfilesDir + testFile) for testFile in self._testNames[componentType]]

            # order tests
            if (len(tests) > 1) and (componentType != 'multipole') and (componentType != 'thinmultipole'):
                tests = _General.GetOrderedTests(self._testParamValues, tests, componentType)

            # compile iterable list of dicts for multithreading function.
            testlist = []
            for test in tests:
                origname = _General.CheckForOriginal(self._directories["dataSet"], test, componentType)

                # pass data in a dict. Easier to pass single expandable variable.
                testDict = TestData(testFile=test,
                                    componentType=componentType,
                                    originalFile=origname,
                                    generateOriginals=generateOriginals,
                                    selfCompare=selfCompare
                                    )

                testlist.append(testDict)
            self._debugOutput("... Done.")

            # Run BDSIM for these tests.
            self._Run(testlist, componentType)

            componentTime = time.time() - t  # final time
            self.Analysis.TimingData.AddComponentTotalTime(componentType, componentTime)

            self._debugOutput("\tElement finished.")
            # component finished, onto next component type
            _os.chdir('../')

    def RunTestSuite(self):
        """ Run all tests in the test suite. This will generate the tests rootevent
            output, compares to an original file, and processes the comparison results.

            Breakdown:
            1. Write All Tests
            2. Generate Original Data Set or point to Pre-Existing Data Set
            3. Generate New Data Set
            4. Compare Data Sets
            5. Analysis and Generate Plots/Results.

            """
        # if using pickled data, just produce the plots and report.
        if self._testEnv['pickledData']:
            self._loadPickledData()
            return None

        _General.WriteGlobalOptions()  # Write the global options file.
        self.WriteGmadFiles()  # Write all gmad files for all test objects.

        initialTime = time.time()

        if self._testEnv["generateOriginals"]:
            # generate original data set
            _os.chdir(self._directories["origSet"])
            self._GenerateDataSet(selfCompare=self._testEnv["selfCompare"])
            _os.chdir('../')

        if self._testEnv["generateData"]:
            # generate new data set
            _os.chdir(self._directories["dataSet"])
            self._GenerateDataSet()
            _os.chdir('../')

        # analysis
        if not generateOriginals:
            self._debugOutput("\tRunning analysis...")
            for componentType in self._testNames.keys():
                self.Analysis.AddTimingData(componentType, self.Analysis.TimingData)

                self._debugOutput("\tGenerating output results...")
                self.Analysis.ProcessResults(componentType=componentType)

                if self._testEnv["plotResults"]:
                    self.Analysis.PlotResults(componentType=componentType)
                    self._debugOutput("\tGenerating plot(s)...")
        else:
            self.Analysis.ProcessOriginals()

        self._debugOutput("\tAnalysis and output complete.")

        """
        if self._testEnv['generateOriginals']:
            _os.chdir(self._directories["origSet"])
        else:
            _os.chdir(self._directories["dataSet"])

        self._debugOutput("Running component tests...")
        # loop over all component types.
        for componentType in self._testNames.keys():
            self._debugOutput("  Component: "+componentType)
            testfilesDir = '../Tests/'

            if self._testEnv['generateOriginals']:
                _General.CheckDirExistsElseMake(componentType)
                _os.chdir(componentType)
                _General.CheckDirExistsElseMake("FailedTests")
                testfilesDir = '../../Tests/'

            t = time.time()  # initial time

            # list of file paths for every test of this component type
            tests = [(testfilesDir + testFile) for testFile in self._testNames[componentType]]

            # order tests
            if (len(tests) > 1) and (componentType != 'multipole') and (componentType != 'thinmultipole'):
                tests = _General.GetOrderedTests(self._testParamValues, tests, componentType)

            self._debugOutput("Compiling test list:")
            # compile iterable list of dicts for multithreading function.
            testlist = []
            for test in tests:
                origname = _General.CheckForOriginal(self._directories["dataSet"], test, componentType)

                # pass data in a dict. Easier to pass single expandable variable.
                testDict = TestData(testFile=test,
                                    componentType=componentType,
                                    originalFile=origname,
                                    generateOriginals=self._testEnv['generateOriginals'],
                                    selfCompare=selfCompare
                                    )

                testlist.append(testDict)
            self._debugOutput("... Done.")

            # Run BDSIM for these tests.
            self._Run(testlist, componentType)

            self._debugOutput("\tRunning analysis...")

            componentTime = time.time() - t  # final time
            self.Analysis.TimingData.AddComponentTotalTime(componentType, componentTime)

            if self._testEnv['generateOriginals']:
                _os.chdir('../')
            else:
                self.Analysis.AddTimingData(componentType, self.Analysis.TimingData)

                self._debugOutput("\tGenerating output results...")
                self.Analysis.ProcessResults(componentType=componentType)

                if self._testEnv["plotResults"]:
                    self.Analysis.PlotResults(componentType=componentType)
                    self._debugOutput("\tGenerating plot(s)...")

            self._debugOutput("\tAnalysis and output complete.")
            self._debugOutput("\tElement finished.")
        """
        if not self._testEnv['generateOnly']:
            finalTime = time.time() - initialTime
            self.Analysis.TimingData.SetTotalTime(finalTime)
            self.Analysis._pickleResults()
            self.Analysis.ProduceReport()
        _os.chdir('../')

        self._debugOutput("Testing complete.")

    def _FullTestSuite(self):
        # data containers for tests and number of files
        numFiles = {}
        for component in GlobalData.components:
            numFiles[component] = 0

        # set phase space
        TestPS = GlobalData.BeamPhaseSpace
        BeamPhaseSpace = PhaseSpace.PhaseSpace(TestPS['X'], TestPS['PX'], TestPS['Y'], TestPS['PY'], TestPS['T'], TestPS['PT'])
        BeamPhaseSpace._WriteToInrays('Tests/trackingTestBeam.madx')

        # loop over all machines which have an energy and particle
        for machineInfo in GlobalData.accelerators.values():
            energy = machineInfo['energy']
            particle = machineInfo['particle']

            bannedComponents = ["rbend", "sbend", "muspoiler"]
            # loop over all components
            for component in GlobalData.components:
                if component not in bannedComponents:
                    componentTest = Test(component, energy, particle, BeamPhaseSpace, useDefaults=True)
                    self.AddTest(componentTest)
                    numFiles[component] += componentTest._numFiles

        self.totalFiles = 0
        self.numFiles = numFiles
        for component in numFiles:
            self.totalFiles += numFiles[component]

        # run the test suite now that all the tests have been added.
        self.RunTestSuite()

