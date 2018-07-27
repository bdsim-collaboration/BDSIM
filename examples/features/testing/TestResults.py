import numpy as _np
import os as _os
import time as _time
import collections
import Globals
import pickle
import string as _string
from matplotlib import colors as _color
from matplotlib import ticker as _tick
import matplotlib.pyplot as _plt
import matplotlib.patches as _patches
import matplotlib.backends.backend_pdf as _bkpdf

# data type with multiple entries that can be handled by the functions.
multiEntryTypes = [tuple, list, _np.ndarray]

GlobalData = Globals.Globals()

resultsKeys = ['timingData',
               'comparatorResults',
               'fileLabel',
               'testResults']


class Results(list):
    """ A class containing all the data from the test results.

        Parameters:

        componentType :  string
            The component type that this class holds the data of.
        """
    def __init__(self, componentType=''):
        GlobalData._CheckComponent(componentType)
        self._component = componentType
        self._numEntries = 0

    def GetResultsByParticle(self, particle=''):
        """ A function to return a Results instance containing the subset of
            results which have the specified particle type.
            """
        if not particle in GlobalData.particles:
            raise ValueError("Unknown particle type.")
        particleResults = Results(self._component)
        for testNum, testResult in enumerate(self):
            if testResult['particle'] == particle:
                particleResults.append(testResult)
        particleResults._numEntries = len(particleResults)
        setattr(particleResults, 'uniqueValues', particleResults._getUniqueValues())
        setattr(particleResults, 'commonValues', particleResults._getCommonValues())
        return particleResults

    def GetResultsByEnergy(self, energy=''):
        """ A function to return a Results instance containing the subset of
            results which have the specified energy. Energy should be a string.
            """
        energyResults = Results(self._component)
        for testNum, testResult in enumerate(self):
            if testResult['testParams']['energy'] == energy:
                energyResults.append(testResult)
        energyResults._numEntries = len(energyResults)
        setattr(energyResults, 'uniqueValues', energyResults._getUniqueValues())
        setattr(energyResults, 'commonValues', energyResults._getCommonValues())
        return energyResults

    def _getCommonValues(self):
        """ A function get the common parameter values for all entries
            in this class. Returns an ordered dictionary with the parameters
            and common values.
            """
        uniqueValues = self._getUniqueValues()
        if uniqueValues is None:
            return None

        commonValues = collections.OrderedDict()
        for key, value in uniqueValues.iteritems():
            if len(value) == 1:
                commonValues[key] = value[0]

        for values in commonValues.values():
            if len(values) > 0:
                return commonValues
        return None

    def _getUniqueValues(self):
        """ A function get the unique parameter values for all entries
            in this class. Returns an ordered dictionary with the parameters
            and unique values.
            """
        uniqueValues = collections.OrderedDict()
        for test in self:
            for key, value in test['testParams'].iteritems():
                if not uniqueValues.keys().__contains__(key):
                    uniqueValues[key] = []
                if not uniqueValues[key].__contains__(value):
                    uniqueValues[key].append(value)
        return uniqueValues

    def GetResults(self):
        """ A function get the results for all entries in this class.
            Returns a list of the results.
            """
        testResults = [test['comparatorResults'] for test in self]
        return testResults

    def GetGeneralStatus(self):
        """ A function get the general status for all entries in this class.
            Returns a list of the results.
            """
        statResults = [test['generalStatus'] for test in self]
        return statResults

    def GetParams(self):
        """ A function get the component parameter values for all
            entries in this class. Returns a list of the results.
            """
        paramResults = [test['testParams'] for test in self]
        return paramResults


class Timing:
    """ A class containing the running time data for BDSIM and the
        comparator for each test.
        """
    def __init__(self):
        self.componentTimes = {}
        self.bdsimTimes = {}
        self.comparatorTimes = {}
        self.totalTime = 0

    def SetTotalTime(self, totalTime):
        """ Set the total time for all tests for all components.
            """
        try:
            totalTime = _np.float(totalTime)
            self.totalTime = totalTime
        except ValueError:
            pass

    def AddComponentTotalTime(self, component, componentTime):
        """ Set total time for all tests for a given components.
            """
        try:
            cTime = _np.float(componentTime)
            self.componentTimes[component] = cTime
        except ValueError:
            print("Cannot convert componentTime to a numerical value.")

    def AddComponentTestTime(self, component, test):
        if not self.bdsimTimes.keys().__contains__(component):
            self.bdsimTimes[component] = []

        if not self.comparatorTimes.keys().__contains__(component):
            self.comparatorTimes[component] = []
        self.bdsimTimes[component].append(test['bdsimTime'])
        self.comparatorTimes[component].append(test['compTime'])

    def __repr__(self):
        totalBDSIMTime = []
        totalCompTime = []
        for component, times in self.bdsimTimes.iteritems():
            totalBDSIMTime.extend(times)
        for component, times in self.comparatorTimes.iteritems():
            totalCompTime.extend(times)

        s = 'Total Testing time  = ' + _np.str(self.totalTime) + '\r\n'
        s += 'Average BDSIM time = ' + _np.str(_np.average(totalBDSIMTime)) + " +/- "
        s += _np.str(_np.std(totalBDSIMTime)) + '.\r\n'
        s += 'Average Comparator time = ' + _np.str(_np.average(totalCompTime)) + " +/- "
        s += _np.str(_np.std(totalCompTime)) + '.\r\n'
        return s


class ResultsUtilities:
    """ A class containing utility functions and data containers for the Analysis class.
        """
    def __init__(self):
        self.Results = {}
        self.DipoleResults = {}  # seperate results dict for dipoles.
        self.TimingData = Timing()  # timing data.

    def _getPhaseSpaceComparatorData(self, result, logFile=''):
        """ A function to get the comparator results for all 6 dimensions.

            result :  an entry from the Results object which respresents a single test.

            logfile :  string
                The filename and path of the comparator log file for the result.
            """
        # phasespace coords initialised as passed.
        coords = _np.zeros(7)

        # return all true if comparator returned passed
        code = result['code']
        if code == 0:
            return coords
        elif code == 2:  # incorrect args
            coords[:] = GlobalData.ReturnsAndErrors.GetCode('NO_DATA')
            return coords
        elif code == 3:  # file not found
            coords[:] = GlobalData.ReturnsAndErrors.GetCode('NO_DATA')
            return coords
        elif code == 6:  # Timeout
            coords[:] = GlobalData.ReturnsAndErrors.GetCode('NO_DATA')
            return coords

        # get data in logfile.
        f = open(logFile)
        lines = f.read()
        f.close()

        # get data on number particles comparison. If the numbers are not equal,
        # set the phase space coords to no data, and number of particle compared to failed.
        numParticlesIndex = lines.find('Event Tree (1/2) entries')
        if numParticlesIndex != -1:
            offendingSamplerBranchesLine = lines[numParticlesIndex:].split('\n')[0]
            branches = offendingSamplerBranchesLine.replace('Event Tree (1/2) entries ', '')
            branches = branches.replace('(', '')
            branches = branches.replace(')', '')
            numParticles = branches.split('/')
            if numParticles[0] != numParticles[1]:
                coords[6] = GlobalData.ReturnsAndErrors.GetCode('FAILED')
                coords[0:6] = GlobalData.ReturnsAndErrors.GetCode('NO_DATA')
                # if num particles don't match, there'll be no phase space comparison
                return coords

        # get data on phase space branches that failed. Log file only contains failed branches.
        phasespaceIndex = lines.find('type Sampler')
        if phasespaceIndex != -1:
            offendingSamplerBranchesLine = lines[phasespaceIndex:].split('\n')[1]
            branches = offendingSamplerBranchesLine.split(' ')
            branches.remove('Offending')
            branches.remove('branches:')
            branches.remove('')
            if branches.__contains__('x'):
                coords[0] = GlobalData.ReturnsAndErrors.GetCode('FAILED')
            if branches.__contains__('xp'):
                coords[1] = GlobalData.ReturnsAndErrors.GetCode('FAILED')
            if branches.__contains__('y'):
                coords[2] = GlobalData.ReturnsAndErrors.GetCode('FAILED')
            if branches.__contains__('yp'):
                coords[3] = GlobalData.ReturnsAndErrors.GetCode('FAILED')
            if branches.__contains__('t'):
                coords[4] = GlobalData.ReturnsAndErrors.GetCode('FAILED')
            if branches.__contains__('zp'):
                coords[5] = GlobalData.ReturnsAndErrors.GetCode('FAILED')
            if branches.__contains__('n'):
                coords[6] = GlobalData.ReturnsAndErrors.GetCode('FAILED')
                coords[0:6] = GlobalData.ReturnsAndErrors.GetCode('NO_DATA')
        return coords

    def _getBDSIMLogData(self, result):
        """ A function to recognise errors and warnings in the printout when
            running BDSIM .

            result :  an entry from the Results object which respresents a single test.
            """
        generalStatus = []

        # append comparator check
        if result['code'] == 0:
            generalStatus.append(0)
        elif result['code'] == 1:
            generalStatus.append(1)
        elif result['code'] == 2:
            generalStatus.append(2)
        elif result['code'] == 3:
            generalStatus.append(3)
        elif result['code'] == 6:
            generalStatus.append(6)

        # get data in logfile.
        f = open(result['bdsimLogFile'])
        lines = f.read()
        f.close()

        splitLines = lines.split('\n')

        # start and endlines. There are two copies of the G4Exception warning/error, however one
        # has a space at the end.
        startLines = ['-------- WWWW ------- G4Exception-START -------- WWWW -------',
                      '-------- WWWW ------- G4Exception-START -------- WWWW ------- ',
                      '-------- EEEE ------- G4Exception-START -------- EEEE -------',
                      '-------- EEEE ------- G4Exception-START -------- EEEE ------- ']
        endLines = ['-------- WWWW -------- G4Exception-END --------- WWWW -------',
                    '-------- WWWW -------- G4Exception-END --------- WWWW ------- ',
                    '-------- EEEE -------- G4Exception-END --------- EEEE -------',
                    '-------- EEEE -------- G4Exception-END --------- EEEE ------- ']

        # indices of start and end of geant4 exceptions and warnings.
        startLineIndices = [i for i, x in enumerate(splitLines) if startLines.__contains__(x)]
        endLineIndices = [i for i, x in enumerate(splitLines) if endLines.__contains__(x)]
        endLineIndices.sort()  # sort just in case

        if len(startLineIndices) != len(endLineIndices):
            # something went wrong
            return generalStatus

        # loop over all soft codes and update general status with error code depending
        # on which Geant4 class(es) the error was issued by. There are two seperate checks,
        # one for the whole error line in the file, the other for the Geant4 class that issued
        # the error in the G4Exception section in the file. The whole error line must be checked
        # as some Geant4 errors are written out with cerr rather than G4Exception class.
        softCodes = GlobalData.ReturnsAndErrors.GetSoftCodes()
        for code in softCodes:
            issuedBy = GlobalData.ReturnsAndErrors.GetIssuedBy(code)

            if multiEntryTypes.__contains__(type(issuedBy)):
                for issue in issuedBy:
                    issueCR = issue + "\r\n"
                    if splitLines.__contains__(issueCR):
                        generalStatus.append(GlobalData.ReturnsAndErrors.GetCode(code))
            else:
                issueCR = issuedBy + "\r\n"
                if splitLines.__contains__(issueCR):
                    generalStatus.append(GlobalData.ReturnsAndErrors.GetCode(code))

            if len(startLineIndices) > 0:
                for index, startLine in enumerate(startLineIndices):
                    exceptions = splitLines[startLine:endLineIndices[index] + 1]
                    issuedLine = exceptions[2]

                    if multiEntryTypes.__contains__(type(issuedBy)):
                        for issue in issuedBy:
                            if issuedLine.__contains__(issue):
                                generalStatus.append(GlobalData.ReturnsAndErrors.GetCode(code))
                    else:
                        if issuedLine.__contains__(issuedBy):
                            generalStatus.append(GlobalData.ReturnsAndErrors.GetCode(code))
        return generalStatus

    def _getGitCommit(self):
        """ Function to get the information about which commit BDSIM was built using.
            """
        pwd = _os.getcwd()  # keep copy of testing cwd
        logfile = pwd + '/gitCommit.log'
        branchfile = pwd + '/gitBranch.log'

        # cd to git repo, output info to log file in testing dir, and cd back
        _os.chdir(GlobalData._bdsimSource)
        _os.system("git log -1 > " + logfile)
        _os.system("git branch | grep  \* > " + branchfile)
        fetchTime = _time.strftime('%Y-%m-%d %H:%M:%S', _time.localtime(_os.stat('.git/FETCH_HEAD').st_mtime))
        _os.chdir(pwd)

        # branch info should only be one line of text
        f = open('gitBranch.log', 'r')
        branchLine = f.next()
        f.close()
        # get branch name
        branchLine = branchLine.strip('\r')
        branchLine = branchLine.strip('\n')
        branch = branchLine.split(' ')[1]

        gitLines = "BDSIM was built from the git repository using the branch " + branch + ".\r\n"
        gitLines += "Last fetch from remote was at " + fetchTime + ".\r\n"
        gitLines += "Local repository is at the following commit: \r\n"
        gitLines += "\r\n"

        # append lines from commit log
        f = open('gitCommit.log')
        for line in f:
            gitLines += (line + '\r\n')
        _os.remove('gitCommit.log')
        _os.remove('gitBranch.log')
        return gitLines

    def _getPickledData(self):
        """ Function to get the pickled data files.
            """
        with open('results.pickle', 'rb') as handle:
            self.Results = pickle.load(handle)
        with open('dipoleResults.pickle', 'rb') as handle:
            self.DipoleResults = pickle.load(handle)
        with open('timing.pickle', 'rb') as handle:
            self.TimingData = pickle.load(handle)

    def _getCommonFactors(self, results):
        """ Function to get the common parameter values of a data set.
            """
        # Note, this is equivalent to the _getCommonValues funstion in the Results
        # class, however, here the results are supplied as a argument which doesn't
        # have to be a Results instance.

        commonFactors = {}
        globalParams = {}
        for res in results:
            for param, value in res['testParams'].iteritems():
                if not globalParams.keys().__contains__(param):
                    globalParams[param] = []
                if not globalParams[param].__contains__(value):
                    globalParams[param].append(value)
        for param, value in globalParams.iteritems():
            if len(value) == 1:
                commonFactors[param] = value[0]
            elif len(value) == 2:
                commonFactors[param] = value
        return commonFactors

    def _processTimingData(self, component):
        bdsimMean = _np.mean(self.TimingData.bdsimTimes[component])
        compMean = _np.mean(self.TimingData.comparatorTimes[component])
        bdsimStd = _np.std(self.TimingData.bdsimTimes[component])
        compMean = _np.std(self.TimingData.comparatorTimes[component])

        bdsimLimit = bdsimMean + 3 * bdsimStd
        longTests = Results(component)
        longTests.extend([test for test in self.Results[component] if test['bdsimTime'] > bdsimLimit])

        commonFactors = self._getCommonFactors(longTests)

        if commonFactors.keys().__len__() > 0:
            s = "There were " + _np.str(len(longTests)) + " tests that took longer than " \
                + _np.str(_np.round(bdsimLimit, 6)) + " s,\r\n"
            s += "these tests had the following common parameters:\r\n"
            for param, value in commonFactors.iteritems():
                s += "\t" + param + " : " + _np.str(value) + ".\r\n"
            return s
        else:
            return None

    def _pickleResults(self):
        """ Function to pickle the Results instances and save to disk.
            """
        with open('results.pickle', 'wb') as handle:
            pickle.dump(self.Results, handle, protocol=pickle.HIGHEST_PROTOCOL)
        with open('timing.pickle', 'wb') as handle:
            pickle.dump(self.TimingData, handle, protocol=pickle.HIGHEST_PROTOCOL)
        with open('dipoleResults.pickle', 'wb') as handle:
            pickle.dump(self.DipoleResults, handle, protocol=pickle.HIGHEST_PROTOCOL)


class Analysis(ResultsUtilities):
    """ A class for processing the test results."""
    def __init__(self):
        ResultsUtilities.__init__(self)

    def AddResults(self, results):
        """ Add results to the class's data containers.
            """
        componentType = results['componentType']
        if not self.Results.keys().__contains__(componentType):
            self.Results[componentType] = Results(componentType)
        if isinstance(results, dict):
            self.Results[componentType].append(results)
        elif multiEntryTypes.__contains__(type(results)):
            for res in results:
                self.Results[componentType].append(res)
        self.Results[componentType]._numEntries = len(self.Results[componentType])

    def AddTimingData(self, componentType, timingData):
        """ Add timing data to the class's data containers.
            """
        if not isinstance(timingData, Timing):
            raise TypeError("Timing data muse be a TestResults.Timing instance.")
        else:
            self.Results[componentType].timingData = timingData

    def ProcessOriginals(self):
        """ Process the output when generating an original data set.
            """
        numTests = 0
        numFailed = 0
        failedTests = []
        softFails = {}

        for component, compResults in self.Results.iteritems():
            for testdict in compResults:
                numTests += 1

                generalStatus = testdict['generalStatus']
                # if failed
                if (generalStatus is not None) and (not generalStatus.__contains__(0)):
                    numFailed += 1
                    failedFile = testdict['testFile'].split('/')[-1]
                    failedTests.append(failedFile)
                # check for soft failures
                for failure in GlobalData.ReturnsAndErrors.GetSoftCodes():
                    if generalStatus.__contains__(GlobalData.ReturnsAndErrors.GetCode(failure)):
                        if not softFails.keys().__contains__(failure):
                            softFails[failure] = []
                        failedFile = testdict['testFile'].split('/')[-1]
                        softFails[failure].append(failedFile)

        s = _np.str(numTests - numFailed) + "/" + _np.str(numTests) + " ROOT files were successfully generated.\r\n"
        print(s)

        f = open('DataSetGeneration.log', 'a')
        timestring = '! ' + _time.strftime("%a, %d %b %Y %H:%M:%S +0000", _time.gmtime()) + '\n'
        f.write(timestring)
        f.write('\r\n')
        f.write(s)
        f.write('\r\n')

        if numFailed > 0:
            f.write("The following tests failed to generate ROOT output:\r\n")
            for test in failedTests:
                f.write(test + "\r\n")
            f.write("\r\n")

        for failure in GlobalData.ReturnsAndErrors.GetSoftCodes():
            failures = []
            if softFails.keys().__contains__(failure):
                failures = softFails[failure]

            if len(failures) > 0:
                failureString = failure.replace("_", " ")
                failureString = _string.lower(failureString)
                f.write("The following " + _np.str(len(failures)) + " generated files contain " \
                        + failureString + ":\r\n")
                for test in failures:
                    f.write(test + "\r\n")
                f.write("\r\n")

        f.write(self._getGitCommit())
        f.close()

    def ProcessResults(self, componentType=''):
        """ Process the results data held in the results class.
            """
        if componentType != '':
            GlobalData._CheckComponent(componentType)

            testResults = self.Results[componentType]
            # set comparator results of all failed tests
            for index, result in enumerate(testResults):
                coords = self._getPhaseSpaceComparatorData(result, 'FailedTests/' + result['compLogFile'])
                self.Results[componentType][index]['comparatorResults'] = coords
        else:
            GlobalData._CheckComponent(componentType)  # raises value error

        # reverse list of results. This is purely for plotting purposes,
        # we want the data to be displayed descending rather than ascending.
        self.Results[componentType].reverse()
        setattr(self.Results[componentType], 'uniqueValues', self.Results[componentType]._getUniqueValues())
        setattr(self.Results[componentType], 'commonValues', self.Results[componentType]._getCommonValues())

        if (componentType == 'rbend') or (componentType == 'sbend'):
            self._groupDipoleResults(componentType)
        self._processTimingData(componentType)

    def _groupDipoleResults(self, componentType=''):
        """ Process the dipole results data held in the results class.
            Dipole data is grouped according to energy, length, and angle
            or field. The results are then representative of all
            poleface parameter combinations.
            """
        # get dictionary of poleface parameter values
        def _dictPolefaceCompVals(testparams):
            paramSet = collections.OrderedDict()
            paramSet['e1'] = testparams['e1']
            paramSet['e2'] = testparams['e2']
            paramSet['fint'] = testparams['fint']
            paramSet['fintx'] = testparams['fintx']
            paramSet['hgap'] = testparams['hgap']
            return paramSet

        # update general status column
        def _updateGeneralStatus(_genStatus):
            genStat = []
            for codes in _genStatus:
                if (len(codes) == 1) and (not genStat.__contains__(codes[0])):
                    genStat.append(codes[0])
                elif len(codes) > 1:
                    for code in codes:
                        if not genStat.__contains__(code):
                            genStat.append(code)
            return genStat

        # update phasespace coords
        def _updateCoords(_comparatorResults):
            resList = [[], [], [], [], [], [], []]
            coords = _np.array(_comparatorResults)
            for index in range(coords.shape[1]):
                values = coords[:, index]
                templist = []
                for i in values:
                    if not templist.__contains__(i):
                        templist.append(i)
                resList[index] = templist
            return resList

        # get dictionary of non-poleface parameters
        def _getResDict(energyVal, lengthVal, angleVal=0, fieldVal=0):
            dipoleResults = {}
            dipoleResults['testParams'] = collections.OrderedDict()
            dipoleResults['testParams']['energy'] = energyVal
            dipoleResults['testParams']['length'] = lengthVal
            dipoleResults['polefaceParams'] = []
            if angleVal != 0:
                dipoleResults['testParams']['angle'] = angleVal
            elif fieldVal != 0:
                dipoleResults['testParams']['field'] = fieldVal
            return dipoleResults

        # get the results data
        if (componentType != 'rbend') and (componentType != 'sbend'):
            raise ValueError("Component must be an rbend or sbend.")
        params = self.Results[componentType].GetParams()
        uniqueValues = self.Results[componentType].uniqueValues
        generalStatus = self.Results[componentType].GetGeneralStatus()
        comparatorResults = self.Results[componentType].GetResults()
        dipoleResults = Results(componentType)

        if len(self.Results[componentType]) == 1:
            self.DipoleResults[componentType] = self.Results[componentType]
        else:
            # loop over all non-poleface parameters
            for energy in uniqueValues['energy']:
                for length in uniqueValues['length']:
                    if uniqueValues.keys().__contains__('angle'):
                        for angle in uniqueValues['angle']:
                            # containers for dipole results
                            dipRes = _getResDict(energy, length, angleVal=angle)
                            _genStat = []
                            _resList = []

                            # create list of tests that have the correct energy, length, and angle.
                            # Also update poleface parameter values of appropriate tests.
                            for testNum in range(self.Results[componentType]._numEntries):
                                testParams = params[testNum]
                                if (testParams['energy'] == energy) and (testParams['length'] == length) \
                                        and (testParams['angle'] == angle):
                                    dipRes['particle'] = self.Results[componentType][testNum]['particle']
                                    dipRes['polefaceParams'].append(_dictPolefaceCompVals(testParams))
                                    _genStat.append(generalStatus[testNum])
                                    _resList.append(comparatorResults[testNum])

                            # update the poleface parameter values, general status, and coords results
                            dipRes['generalStatus'] = _updateGeneralStatus(_genStat)
                            dipRes['comparatorResults'] = _updateCoords(_resList)

                            dipoleResults.append(dipRes)
                    elif uniqueValues.keys().__contains__('field'):
                        for field in uniqueValues['field']:
                            # containers for dipole results
                            dipRes = _getResDict(energy, length, fieldVal=field)
                            _genStat = []
                            _resList = []

                            # create list of tests that have the correct energy, length, and field.
                            # Also update poleface parameter values of appropriate tests.
                            for testNum in range(self.Results[componentType]._numEntries):
                                testParams = params[testNum]
                                if (testParams['energy'] == energy) and (testParams['length'] == length) \
                                        and (testParams['field'] == field):
                                    dipRes['particle'] = self.Results[componentType][testNum]['particle']
                                    dipRes['polefaceParams'].append(_dictPolefaceCompVals(testParams))
                                    _genStat.append(generalStatus[testNum])
                                    _resList.append(comparatorResults[testNum])

                            # update the poleface parameter values, general status, and coords results
                            dipRes['generalStatus'] = _updateGeneralStatus(_genStat)
                            dipRes['comparatorResults'] = _updateCoords(_resList)

                            dipoleResults.append(dipRes)
            self.DipoleResults[componentType] = dipoleResults

    def PlotResults(self, componentType=''):
        """ Plot the results for the specified component type.
            """
        if self.Results.keys().__contains__(componentType):
            plotter = _Plotting()
            self._processTimingData(componentType)
            # if the component is a dipole, use the dipole data
            if (componentType == 'rbend') or (componentType == 'sbend'):
                plotter.PlotResults(self.DipoleResults, componentType)
                plotter.PlotTimingData(self.TimingData, componentType)
            # otherwise use the normal data
            else:
                plotter.PlotResults(self.Results, componentType)
                plotter.PlotTimingData(self.TimingData, componentType)

    def ProduceReport(self):
        """ Produce a written report for all component types.
            The report will contain an overall results summary, a breakdown
            of the hard and soft failures, a breakdown of the comparator failures,
            and display the component parameter values coomon to those failures.
            """
        report = _Report(self.Results, self.DipoleResults)
        report.ProduceReport()


class _Plotting:
    """ A class for producing plots of the testing results. Class should be hidden,
        it is instantiated in the Analysis class and used there."""
    def __init__(self):
        self.testsPerAxes = 75  # about the maximum that is resolvable on a figure.

    def _getNumFigures(self, num):
        """ Function to calculate the number of figures required based on the number of axes supplied."""
        remainder = _np.mod(num, 2)
        if remainder == 0:
            numFigures = num / 2.0
        else:
            numFigures = ((num - remainder) / 2.0) + 1
        return _np.int(numFigures)

    def _getFigSize(self, res1, res2=None):
        """ Function to return the figure size based on the number of results that will
            be displayed. """
        # No need to have complicated algorithm, just return discrete sizes.
        if res2 is not None:
            if (res1._numEntries < 25) or (res2._numEntries < 25):
                figHeight = 6
            elif (res1._numEntries < 50) or (res2._numEntries < 50):
                figHeight = 9
            else:
                figHeight = 12
            if (res1._numEntries == 0) or (res2._numEntries == 0):
                figWidth = 8
            else:
                figWidth = 12
        else:
            if res1._numEntries < 25:
                figHeight = 6
            elif res1._numEntries < 50:
                figHeight = 9
            else:
                figHeight = 12
            if res1._numEntries == 0:
                figWidth = 8
            else:
                figWidth = 12

        return figWidth, figHeight

    def PlotResults(self, allResults, componentType=''):
        """ Function for plotting the testing results for a specific component.
            """

        GlobalData._CheckComponent(componentType)

        # get all results of this component type.
        res = allResults[componentType]

        # split results into proton and electron
        electronResults = res.GetResultsByParticle('e-')
        protonResults = res.GetResultsByParticle('proton')

        # use appropriate function depending on the number of tests per particle type
        if electronResults._numEntries == 0:
            self._plotSingleParticle(protonResults)
        elif protonResults._numEntries == 0:
            self._plotSingleParticle(electronResults)
        elif (protonResults._numEntries > 0) and (electronResults._numEntries > 0):
            if (protonResults._numEntries <= self.testsPerAxes) and (electronResults._numEntries <= self.testsPerAxes):
                fileName = '../Results/' + componentType + '.png'
                self._singleFigureDoubleData(electronResults, protonResults, fileName)
            else:
                self._plotSingleParticle(electronResults)
                self._plotSingleParticle(protonResults)

    def _plotSingleParticle(self, results):
        """ Function for plotting the results for a single particle type."""

        # if number of tests is too high, split tests by particle energy, and call appropriate function.
        if results._numEntries > self.testsPerAxes:
            numEnergies = len(results.uniqueValues['energy'])
            if numEnergies == 1:
                self._singleDataAxesByEnergy(results)
            else:
                numFigures = self._getNumFigures(numEnergies)
                if (_np.mod(numFigures, 2) == 0) or (numFigures == 1):
                    self._doubleDataAxesByEnergy(numFigures, results)
                else:
                    self._doubleDataAxesByEnergy(numFigures - 1, results)
                    self._singleDataAxesByEnergy(results)
        else:
            fileName = '../Results/' + results._component + '.png'
            self._singleFigureSingleData(results, fileName)

    def _singleFigureDoubleData(self, results1, results2, filename):
        """ Function for plotting two sets of results on a single figure."""
        figsize = self._getFigSize(results2, results1)
        f = _plt.figure(figsize=figsize)
        # 4 axes, two for the data, two for the label boxing structure.
        ax1 = f.add_subplot(141)
        ax2 = f.add_subplot(142)
        ax3 = f.add_subplot(143)
        ax4 = f.add_subplot(144)

        # calculate the offset for the parameter name that will be added to the label boxing structure.
        # Normalised so that the offsets should appear at the same vertical position on both axes.
        res1Size = _np.float(figsize[1] - 1) / len(results1)
        res2Size = _np.float(figsize[1] - 1) / len(results2)
        res1Offset = 1.0
        res2Offset = 1.0
        if res1Size > res2Size:
            res1Offset = res2Size / res1Size
        elif res1Size < res2Size:
            res2Offset = res1Size / res2Size

        # plot the data / label boxing.
        dataAx1 = self._updateAxes(ax2, ax1, results1, res1Offset)
        dataAx2 = self._updateAxes(ax4, ax3, results2, res2Offset)

        self._addColorBar(f, dataAx1)

        pdf = _bkpdf.PdfPages(filename+"_plots.pdf")
        for i in _plt.get_fignums():
            pdf.savefig(i)
        pdf.close()

        #f.savefig(filename, dpi=600)
        #_plt.close()

    def _doubleDataAxesByEnergy(self, numFigures, results):
        """ Function for plotting data sets that are split by energy on multiple figures."""
        # loop over number of figures
        for i in range(numFigures):
            # get results by energy
            energy1 = results.uniqueValues['energy'][2 * i]
            energy2 = results.uniqueValues['energy'][2 * i + 1]
            res1 = results.GetResultsByEnergy(energy1)
            res2 = results.GetResultsByEnergy(energy2)

            particle = results[0]['particle']
            energyString = '_' + particle + '_energies__' + energy1 + '_' + energy2
            fileName = '../Results/' + results._component + energyString + '.png'

            self._singleFigureDoubleData(res1, res2, fileName)

    def _singleDataAxesByEnergy(self, results):
        """ Function for plotting a data set with a single energy on a single figure."""

        # get results by energy. Use last energy in unique values (though there should only be one value.
        res1 = results.GetResultsByEnergy(results.uniqueValues['energy'][-1])
        energy1 = results.uniqueValues['energy'][-1]
        particle = results[0]['particle']
        energyString = '_' + particle + '_energies__' + energy1
        fileName = '../Results/' + results._component + energyString + '.png'
        self._singleFigureSingleData(res1, fileName)

    def _singleFigureSingleData(self, results, filename):
        """ Function to plot a single data set on a single figure."""
        figsize = self._getFigSize(results)
        f = _plt.figure(figsize=figsize)
        if len(results) == 1:
            ax1 = None
            ax2 = f.add_subplot(111)
        else:
            ax1 = f.add_subplot(121)
            ax2 = f.add_subplot(122)

        dataAxes = self._updateAxes(ax2, ax1, results, 1.0)
        self._addColorBar(f, dataAxes)

        pdf = _bkpdf.PdfPages(filename+"_plots.pdf")
        for i in _plt.get_fignums():
            pdf.savefig(i)
        pdf.close()

        #f.savefig(filename, dpi=600)
        #_plt.close()

    def _addColorBar(self, f, ax):
        """ Add a colorbar to the results plot."""
        # colorbar colors and values independant of data, can be set according to either subplot.
        numReturns = len(GlobalData.ReturnsAndErrors['name'])
        cbar = f.colorbar(ax)
        cbarTicks = _np.linspace(0.5, numReturns - 0.5, numReturns)
        cbar.set_ticks(cbarTicks)
        cbar.set_ticklabels(GlobalData.ReturnsAndErrors['name'])
        cbar.ax.tick_params(labelsize=8)
        f.tight_layout()

    def _updateAxes(self, dataAxis, diagramAxis, dataSetresults, labOffset):
        """ Function to update the axes of a figure."""

        # get uniques values of this data set.
        uniqueValues = dataSetresults.uniqueValues

        def updateDataAxis(ax, results):
            # get all necessary data.
            data = results.GetResults()
            generalStatus = results.GetGeneralStatus()
            particle = results[0]['particle']
            numTests = len(results)

            cmap = _color.ListedColormap(GlobalData.ReturnsAndErrors['colours'])

            # for a given test and phase space axis, the results may contain multiple values
            # e.g [passed, failed, no_data]. Here, we plot using the first of the multiple values
            # and add the remaining values on as boxes later.
            zeroData = []
            for test in data:
                dataRes = []
                for val in test:
                    if multiEntryTypes.__contains__(type(val)):
                        dataRes.append(val[0])
                    else:
                        dataRes.append(val)
                zeroData.append(dataRes)

            # set normalised colormap.
            numReturns = len(GlobalData.ReturnsAndErrors['name'])
            bounds = _np.linspace(0, numReturns, numReturns + 1)
            norm = _color.BoundaryNorm(bounds, cmap.N)

            extent = [0, 7, 0, results._numEntries]

            commonValues = results.commonValues
            subplotTitle = particle

            # subplot title based on common parameter values.
            if commonValues is not None:
                subplotTitle += ", "
                for key, value in commonValues.iteritems():
                    if key != 'energy':
                        subplotTitle += _string.capitalize(key) + " = " + value
                        index = commonValues.keys().index(key)
                        if index != (len(commonValues.keys()) - 1):
                            subplotTitle += ", "
                        # multiple lines if title is longer than 22 chars
                        if len(subplotTitle.split('\n')[-1]) > 22:
                            subplotTitle += '\n'

            # plot the data
            cax = ax.imshow(zeroData, interpolation='none', origin='lower', cmap=cmap, norm=norm,
                            extent=extent, aspect='auto')

            ax.set_xlim(0, 8)

            # overlay the data with boxes for the results values that were not plotted before.
            for index, status in enumerate(data):
                for coord, vals in enumerate(status):
                    # only plot rectangles if there's more than one coord status value, imshow will
                    # have already plotted that box correctly.
                    if multiEntryTypes.__contains__(type(vals)):
                        numStatus = len(vals)
                        yIndex = index
                        for statIndex, stat in enumerate(vals):
                            boxColor = cmap.colors[_np.int(stat)]
                            boxWidth = 1.0 / numStatus
                            ax.add_patch(_patches.Rectangle((coord + statIndex*boxWidth, yIndex), boxWidth, 1, color=boxColor))

            # plot the general status for each test using boxes.
            for index, status in enumerate(generalStatus):
                # only plot rectangles if there's more than one general status value, imshow will
                # have already plotted that box correctly.
                if multiEntryTypes.__contains__(type(status)):
                    numStatus = len(status)
                    yIndex = index
                    for statIndex, stat in enumerate(status):
                        boxColor = cmap.colors[stat]
                        boxWidth = 1.0 / numStatus
                        ax.add_patch(_patches.Rectangle((7 + statIndex*boxWidth, yIndex), boxWidth, 1, color=boxColor))

            if subplotTitle != '':
                ax.set_title(subplotTitle)

            xtickMajors = _np.linspace(1, 8, 8)
            xtickCentre = xtickMajors - 0.5

            ax.set_xticks(xtickCentre)
            ax.set_xticklabels(['x', 'xp', 'y', 'yp', 't', 'zp', 'n', 'Gen'])

            if numTests > 1:
                ytickMajors = _np.linspace(numTests / (numTests - 1), numTests, numTests)
                ytickCentre = ytickMajors - 0.5

                ax.set_yticks(ytickCentre)
                empty_string_labels = [''] * numTests
                ax.set_yticklabels(empty_string_labels)

                ytickMinors = _np.linspace(0, len(data), len(data) + 1)
                minorYTicks = _tick.FixedLocator(ytickMinors)
                ax.yaxis.set_minor_locator(minorYTicks)
                ax.tick_params(axis='y', which='both', length=0, labelsize=9)
                ax.grid(which='minor', axis='y', linestyle='--')

            minorXTicks = _tick.FixedLocator(xtickMajors)
            ax.xaxis.set_minor_locator(minorXTicks)
            ax.tick_params(axis='x', which='both', length=0)
            ax.grid(which='minor', axis='x', linestyle='-')
            return cax

        def updateDiagramAxis(ax, results, labelOffset):
            # get all necessary data.
            params = results.GetParams()
            # dict for param value labels
            boxText = {}

            # calculate number of boxes per level
            numBoxesPerLevel = []
            numLevel = 1
            for i in range(len(uniqueValues.keys())):
                boxText[_np.str(i)] = []
                numValues = len(uniqueValues[uniqueValues.keys()[i]])
                if i == 0:  # alway include energy (1st key in uniqueValues)
                    numLevel *= numValues
                    numBoxesPerLevel.append(numLevel)
                elif numValues > 1:
                    numLevel *= numValues
                    numBoxesPerLevel.append(numLevel)

            # update dict of param values per box
            def updateBoxTextList(depth):
                for value in uniqueValues[uniqueValues.keys()[depth]]:
                    boxText[_np.str(depth)].append(value)
                    if depth < (len(uniqueValues.keys())-1):
                        updateBoxTextList(depth+1)

            # get text for each box
            updateBoxTextList(0)
            numLevels = len(numBoxesPerLevel)

            fontSizes = ['medium', 'medium', 'small', 'xx-small']

            # plot the lines representing the boxes.
            for level, numBoxes in enumerate(numBoxesPerLevel):
                ax.plot([level - numLevels, level - numLevels], [0, len(params)], linewidth=1, color='k')  # vertical bar
                for boxNum in range(numBoxes):
                    boxHeight = len(params) / numBoxesPerLevel[level]
                    textRot = 'horizontal'
                    txtHorPos = level + 0.5
                    txtVerPos = (boxNum * boxHeight) + 0.5 * boxHeight

                    ax.plot([level - numLevels, 0], [boxNum*boxHeight, boxNum*boxHeight],
                            linewidth=1, color='k')  # bottom horizontal bar
                    ax.plot([level - numLevels, 0], [(boxNum+1)*boxHeight, (boxNum+1)*boxHeight],
                            linewidth=1, color='k')  # top hor. bar
                    ax.text(txtHorPos - numLevels, txtVerPos, boxText[_np.str(level)][boxNum],
                            rotation=textRot, va='center', ha='center', fontsize=fontSizes[level])  # param value
                dataValue = _string.capitalize(uniqueValues.keys()[level])
                ax.text(level+0.5 - numLevels, len(params) + labelOffset, dataValue,
                        fontsize=12, ha='center')  # parameter title above the boxes

            ax.set_xlim(-numLevels, 0)
            ax.set_ylim(0, results._numEntries)

            ax.axis('off')
            ax.xaxis.set_visible(False)
            ax.yaxis.set_visible(False)

            return ax

        # update axes
        if diagramAxis is None:
            dataAx1 = updateDataAxis(dataAxis, dataSetresults)
        else:
            dataAx1 = updateDataAxis(dataAxis, dataSetresults)
            labAx1 = updateDiagramAxis(diagramAxis, dataSetresults, labOffset)

        return dataAx1

    def PlotTimingData(self, timingData, component):
        """ Plot the timing data as histograms."""
        f = _plt.figure()
        ax = f.add_subplot(121)
        ax2 = f.add_subplot(122)

        # max values to define histogram range
        bdsimMax = _np.max(timingData.bdsimTimes[component])
        compMax = _np.max(timingData.comparatorTimes[component])

        y, x, _ = ax.hist(timingData.bdsimTimes[component], bins=30, log=True, range=(0,_np.ceil(bdsimMax)))
        y2, x2, _ = ax2.hist(timingData.comparatorTimes[component], bins=30, log=True, range=(0,_np.ceil(compMax)))

        if (_np.max(y) < 100) and (_np.max(y2) < 100):

            ax.set_yscale("linear", nonposx='clip')
            ax2.set_yscale("linear", nonposx='clip')

            maxtimes = [_np.max(y), _np.max(y2)]

            ax.set_ylim(ymin=0, ymax=2.0*_np.max(maxtimes))
            ax2.set_ylim(ymin=0, ymax=2.0*_np.max(maxtimes))
        else:
            # calculate largest number of entries to get y-axis scale.
            maxtimes = [_np.max(y), _np.max(y2)]
            orderOfMag = _np.int(_np.log10(_np.max(maxtimes)))

            # plot on log scale, set min to 0.9 to show single entry bins.
            ax.set_ylim(ymin=0.9, ymax=2 * 10 ** orderOfMag)
            ax2.set_ylim(ymin=0.9, ymax=2 * 10 ** orderOfMag)

        ax.set_xlim(xmin=0)
        ax2.set_xlim(xmin=0)

        ax.set_xlabel('Time (s)')
        ax.set_ylabel('Number of Tests')
        ax.set_title('BDSIM Run Time')

        ax2.set_xlabel('Time (s)')
        ax2.set_title('Comparator Run Time')
        ax2.yaxis.set_visible(False)

        filename = '../Results/' + component + '_timingData'

        pdf = _bkpdf.PdfPages(filename+"_plots.pdf")
        for i in _plt.get_fignums():
            pdf.savefig(i)
        pdf.close()

        #f.savefig('../Results/' + component + '_timingData.png', dpi=600)
        #_plt.close()


class _Report:
    """ A class for producing a report of the testing results. Class should be hidden,
        it is instantiated in the Analysis class and used there."""
    def __init__(self, results, dipoleResults):
        self.Results = results
        self.DipoleResults = dipoleResults
        self.groupedResults = {}
        self._numResults = {}
        self._fileName = "../Results/TestResults_" + _time.strftime("%d%m%Y_%H%M%S", _time.gmtime()) + ".txt"

    def _getResults(self, component):
        """ A function to get the test results for the specified
            component type.
            """
        compResults = None
        if ((component == 'rbend') or (component == 'sbend')) and self.DipoleResults.keys().__contains__(component):
            compResults = self.DipoleResults[component]
        elif self.Results.keys().__contains__(component):
            compResults = self.Results[component]
        return compResults

    def _groupResults(self, comparatorResults, component):
        """ A function to group the results according to their general status,
            i.e, the would find all tests that have tracking warnings, all tests
            that have stuck particles ... etc.
            """
        # seperate dicts for results and numbers. Keys for both will be
        # Globals.Globals.returnCodes values.
        groupedResults = {}
        numResults = {}
        for codeIndex, error in enumerate(GlobalData.ReturnsAndErrors['name']):
            groupedResults[error] = Results(component)
            numResults[error] = 0
            for resultNum, result in enumerate(comparatorResults):
                if (result['generalStatus'].__contains__(codeIndex)) or \
                        (result['generalStatus'].__contains__(_np.str(codeIndex))):
                    groupedResults[error].append(result)
            for resultNum, result in enumerate(self.Results[component]):
                if (result['generalStatus'].__contains__(codeIndex)) or \
                        (result['generalStatus'].__contains__(_np.str(codeIndex))):
                    numResults[error] += 1

        self.groupedResults[component] = groupedResults
        self._numResults[component] = numResults

    def _processFatals(self, component):
        """ A function that gets all results that contain fatal exceptions.
            Returns a string with parameter values common to the results that were found.
            """
        results = self.groupedResults[component]['FATAL_EXCEPTION']
        utils = ResultsUtilities()
        commonFactors = utils._getCommonFactors(results)

        if commonFactors.keys().__len__() > 0:
            s = "\tTests where a fatal exception was called had the common parameters:\r\n"
            for param, value in commonFactors.iteritems():
                s += "\t" + param + " : " + _np.str(value) + ".\r\n"
            return s
        else:
            return None

    def _processSoftFail(self, component, failure):
        """ A function that gets all results that contain the soft failures.
            Returns a string with parameter values common to the results that were found.
            """
        results = self.groupedResults[component][failure]
        utils = ResultsUtilities()
        commonFactors = utils._getCommonFactors(results)

        if commonFactors.keys().__len__() > 0:
            strName = failure.replace("_", " ")
            strName = _string.lower(strName)
            s = "\tTests where a " + strName + "was called had the common parameters:\r\n"
            for param, value in commonFactors.iteritems():
                s += "\t" + param + " : " + _np.str(value) + ".\r\n"
            return s
        else:
            return None

    def _processComparatorFailure(self, component):
        """ A function that gets all results for the phasespace coordinates.
            Returns a string with the number of failures per dimension, and
            parameter values common to the results that were found.
            """
        results = self.Results[component]
        coordLabels = ['x', 'xp', 'y', 'yp', 't', 'zp', 'n']
        numFailures = [0, 0, 0, 0, 0, 0, 0]
        # get number of failures.
        for res in results:
            compResults = res['comparatorResults']
            for index, coord in enumerate(compResults):
                if coord == 1:
                    numFailures[index] += 1

        s = ''
        # only print comparator phase space failures if there are any
        compPasses = True
        for i in numFailures:
            if i > 0:
                compPasses = False
                break

        if not compPasses:
            s = "Number of comparator failures: \r\n"
            s += "  x:  " + _np.str(numFailures[0]) + ",\r\n"
            s += "  xp: " + _np.str(numFailures[1]) + ",\r\n"
            s += "  y:  " + _np.str(numFailures[2]) + ",\r\n"
            s += "  yp: " + _np.str(numFailures[3]) + ",\r\n"
            s += "  t:  " + _np.str(numFailures[4]) + ",\r\n"
            s += "  zp: " + _np.str(numFailures[5]) + ",\r\n"
            s += "  n:  " + _np.str(numFailures[6]) + ".\r\n"
            s += "\r\n"
            s += "Comparator failures had the following common parameters:\r\n"

            # get list of all failures per dimension and the parameter values common
            # to those failed tests.
            for i in range(7):
                s += "  " + coordLabels[i] + ":\r\n"
                failedTests = Results(component)
                for res in results:
                    compResults = res['comparatorResults']
                    if compResults[i] == 1:
                        failedTests.append(res)
                utils = ResultsUtilities()
                commonFactors = utils._getCommonFactors(failedTests)

                if commonFactors.keys().__len__() > 0:
                    for param, value in commonFactors.iteritems():
                        s += "    " + param + " : " + _np.str(value) + ".\r\n"
        return s

    def _componentSectionTitle(self, component):
        """ A function to return the component section title
            in the report.
            """
        s1 = ''
        for i in range(80):
            s1 += "-"
        s1 += "\r\n"

        # component type in the centre
        numSpaces = 80 - len(component)
        if _np.mod(numSpaces, 2) == 0:
            spBefore = numSpaces / 2.0
        else:
            spBefore = (numSpaces - 1) / 2.0
        s2 = ''
        for i in range(_np.int(spBefore)):
            s2 += ' '
        s2 += component + "\r\n"
        title = s1 + s2 + s1
        return title

    def _overallResults(self, component):
        """ A function to collate the results for a specifed component and
            return a string containing the overall results, breakdown of hard
            failures, and breakdown of soft failures.
            """
        # get data
        overallRes = self._numResults[component]
        totalTests = self.Results[component].__len__()

        # start string with overall results
        s = "Overall Results: \r\n"
        s += "  Passed: " + _np.str(overallRes['SUCCESS']) + "/" + _np.str(totalTests) + ",\r\n"
        s += "  Soft Failure: " + _np.str(overallRes['FAILED']) + "/" + _np.str(totalTests) + ",\r\n"
        s += "  Hard Failure: " + _np.str(overallRes['FILE_NOT_FOUND']) + "/" + _np.str(totalTests) + ".\r\n"
        s += "\r\n"

        numHardFailures = overallRes['FILE_NOT_FOUND']

        if numHardFailures > 0:
            # breakdown of hard failures.
            s += "Breakdown of Hard Failures: \r\n"

            # update string with number of fatal exceptions
            if overallRes['FATAL_EXCEPTION'] > 0:
                s += "  Fatal Exceptions: " + _np.str(overallRes['FATAL_EXCEPTION']) + "/" + \
                     _np.str(overallRes['FILE_NOT_FOUND']) + ",\r\n"
                numHardFailures -= overallRes['FATAL_EXCEPTION']
                fatalString = self._processFatals(component)
                s += fatalString

            # update string with number of timeouts
            if overallRes['TIMEOUT'] > 0:
                s += "  Timeouts: " + _np.str(overallRes['TIMEOUT']) + "/" + \
                     _np.str(overallRes['FILE_NOT_FOUND']) + ",\r\n"
                numHardFailures -= overallRes['TIMEOUT']


            s += "  Unknown Reason: " + _np.str(numHardFailures) + "/" + _np.str(overallRes['FILE_NOT_FOUND']) + "\r\n"
            s += "    (likely to be BDSIM self exit).\r\n"
            s += "\r\n"

        # breakdown soft failures
        softStr = "Breakdown of Soft Failures: \r\n"
        softFailures = False

        # loop over all soft failures and update string with numbers per failure.
        for index, failure in enumerate(GlobalData.ReturnsAndErrors.GetSoftCodes()):
            if overallRes[failure] > 0:
                softFailures = True
                strName = failure.replace("_", " ")
                strName = _string.capitalize(strName)
                softStr += "  " + strName + ": " + _np.str(overallRes[failure])
                if index == len(GlobalData.ReturnsAndErrors.GetSoftCodes())-1:
                    softStr += ".\r\n"
                else:
                    softStr += ",\r\n"
                failString = self._processSoftFail(component, failure)
                if failString is not None:
                    softStr += failString
        if softFailures:
            s += softStr
        return s

    def _componentSection(self, component):
        """ A function to write the section report for the specified component.
            """
        section = self._componentSectionTitle(component)
        section += "\r\n"
        section += self._overallResults(component)
        section += "\r\n"
        section += self._processComparatorFailure(component)
        section += "\r\n"
        f = open(self._fileName, 'a')
        f.write(section)
        f.close()

    def ProduceReport(self):
        """ Produce a report of the results.
            """
        for componentType in GlobalData.components:
            results = self._getResults(componentType)

            # if the results for that element exist
            if results is not None:
                self._groupResults(results, componentType)
                self._componentSection(componentType)
            else:
                pass
