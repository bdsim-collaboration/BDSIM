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


# data type with multiple entries that can be handled by the functions.
multiEntryTypes = [tuple, list, _np.ndarray]

GlobalData = Globals.Globals()

resultsKeys = ['timingData',
               'resultsList',
               'fileLabel',
               'generalStatusList',
               'params',
               'testResults']


class Results(list):
    def __init__(self, componentType=''):
        GlobalData._CheckComponent(componentType)
        self._component = componentType
        self._numEntries = 0

    def GetResultsByParticle(self, particle=''):
        if not GlobalData.particles.__contains__(particle):
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
        energyResults = Results(self._component)
        for testNum, testResult in enumerate(self):
            if testResult['testParams']['energy'] == energy:
                energyResults.append(testResult)
        energyResults._numEntries = len(energyResults)
        setattr(energyResults, 'uniqueValues', energyResults._getUniqueValues())
        setattr(energyResults, 'commonValues', energyResults._getCommonValues())
        return energyResults

    def _getCommonValues(self):
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
        # get dict of all unique parameter values.
        uniqueValues = collections.OrderedDict()
        for test in self:
            for key, value in test['testParams'].iteritems():
                if not uniqueValues.keys().__contains__(key):
                    uniqueValues[key] = []
                if not uniqueValues[key].__contains__(value):
                    uniqueValues[key].append(value)
        return uniqueValues

    def GetResults(self):
        testResults = []
        for test in self:
            testResults.append(test['resultsList'])
        return testResults

    def GetGeneralStatus(self):
        statResults = []
        for test in self:
            statResults.append(test['generalStatus'])
        return statResults

    def GetParams(self):
        paramResults = []
        for test in self:
            paramResults.append(test['testParams'])
        return paramResults


class Timing:
    def __init__(self):
        self.componentTimes = {}
        self.bdsimTimes = []
        self.comparatorTimes = []
        self.totalTime = 0

    def SetTotalTime(self, totalTime):
        try:
            totalTime = _np.float(totalTime)
            self.totalTime = totalTime
        except ValueError:
            pass

    def AddComponentTime(self, component, componentTime):
        try:
            cTime = _np.float(componentTime)
            self.componentTimes[component] = cTime
        except ValueError:
            print("Cannot convert componentTime to a numerical value.")

    def __repr__(self):
        s = 'Total Testing time  = ' + _np.str(self.totalTime) + '\r\n'
        s += 'Average BDSIM time = ' + _np.str(_np.average(self.bdsimTimes)) + " +/- "
        s += _np.str(_np.std(self.bdsimTimes)) + '.\r\n'
        s += 'Average Comparator time = ' + _np.str(_np.average(self.comparatorTimes)) + " +/- "
        s += _np.str(_np.std(self.comparatorTimes)) + '.\r\n'
        return s


class ResultsUtilities:
    def __init__(self):
        # dict of list of Results instances for each component type.
        self.Results = {}
        self.DipoleResults = {}
        self.TimingData = Timing()  # timing data.

    def _getPhaseSpaceComparatorData(self, result, logFile=''):
        # phasespace coords initialised as passed.
        coords = _np.zeros(7)

        # return all true if comparator returned passed
        code = result['code']
        if code == 0:
            return coords
        elif code == 2:  # incorrect args
            coords[:] = GlobalData.returnCodes['NO_DATA']
            return coords
        elif code == 3:  # file not found
            coords[:] = GlobalData.returnCodes['NO_DATA']
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
                coords[6] = GlobalData.returnCodes['FAILED']
                coords[0:6] = GlobalData.returnCodes['NO_DATA']
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
                coords[0] = GlobalData.returnCodes['FAILED']
            if branches.__contains__('xp'):
                coords[1] = GlobalData.returnCodes['FAILED']
            if branches.__contains__('y'):
                coords[2] = GlobalData.returnCodes['FAILED']
            if branches.__contains__('yp'):
                coords[3] = GlobalData.returnCodes['FAILED']
            if branches.__contains__('t'):
                coords[4] = GlobalData.returnCodes['FAILED']
            if branches.__contains__('zp'):
                coords[5] = GlobalData.returnCodes['FAILED']
            if branches.__contains__('n'):
                coords[6] = GlobalData.returnCodes['FAILED']
                coords[0:6] = GlobalData.returnCodes['NO_DATA']
        return coords

    def _getBDSIMLogData(self, result):
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

        # get data in logfile.
        f = open(result['bdsimLogFile'])
        lines = f.read()
        f.close()

        splitLines = lines.split('\n')
        startLines = ['-------- WWWW ------- G4Exception-START -------- WWWW -------',
                      '-------- WWWW ------- G4Exception-START -------- WWWW ------- ',
                      '-------- EEEE ------- G4Exception-START -------- EEEE -------',
                      '-------- EEEE ------- G4Exception-START -------- EEEE ------- ']
        endLines = ['-------- WWWW -------- G4Exception-END --------- WWWW -------',
                    '-------- WWWW -------- G4Exception-END --------- WWWW ------- ',
                    '-------- EEEE -------- G4Exception-END --------- EEEE -------',
                    '-------- EEEE -------- G4Exception-END --------- EEEE ------- ']

        startLineIndices = []
        endLineIndices = []

        startLineIndices.extend([i for i, x in enumerate(splitLines) if startLines.__contains__(x)])
        endLineIndices.extend([i for i, x in enumerate(splitLines) if endLines.__contains__(x)])
        endLineIndices.sort()  # sort just in case

        if len(startLineIndices) != len(endLineIndices):
            # something went wrong
            generalStatus.append(8)  # append a no data number
            return generalStatus

        if len(startLineIndices) > 0:
            for index, startLine in enumerate(startLineIndices):
                exceptions = splitLines[startLine:endLineIndices[index] + 1]
                issuedLine = exceptions[2]
                if issuedLine.__contains__('G4PVPlacement::CheckOverlaps()'):
                    generalStatus.append(GlobalData.returnCodes['OVERLAPS'])
                if issuedLine.__contains__('G4PropagatorInField::ComputeStep()'):
                    generalStatus.append(GlobalData.returnCodes['STUCK_PARTICLE'])
                if issuedLine.__contains__('G4MagInt_Driver::AccurateAdvance()'):
                    generalStatus.append(GlobalData.returnCodes['TRACKING_WARNING'])
                if issuedLine.__contains__('G4CutTubs::G4CutTubs()'):
                    generalStatus.append(GlobalData.returnCodes['FATAL_EXCEPTION'])
                if issuedLine.__contains__('G4ChordFinder::FindNextChord()'):
                    generalStatus.append(GlobalData.returnCodes['NAN_CHORD'])

                # TODO: check for other types of warnings/errors.

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
        with open('results.pickle', 'rb') as handle:
            self.Results = pickle.load(handle)
        with open('dipoleResults.pickle', 'rb') as handle:
            self.DipoleResults = pickle.load(handle)
        with open('timing.pickle', 'rb') as handle:
            self.TimingData = pickle.load(handle)

    def _getCommonFactors(self, results):
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

class Analysis(ResultsUtilities):
    def __init__(self):
        ResultsUtilities.__init__(self)

    def AddResults(self, results):
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
        if not isinstance(timingData, Timing):
            raise TypeError("Timing data muse be a TestResults.Timing instance.")
        else:
            self.Results[componentType].timingData = timingData

    def ProcessOriginals(self):
        numTests = 0
        numFailed = 0
        failedTests = []
        stuckParticles = []
        overlaps = []
        for component, compResults in self.Results.iteritems():
            for testdict in compResults:
                numTests += 1

                generalStatus = testdict['generalStatus']

                if (generalStatus is not None) and (not generalStatus.__contains__(0)):
                    numFailed += 1
                    failedFile = testdict['testFile'].split('/')[-1]
                    failedTests.append(failedFile)
                if generalStatus.__contains__(GlobalData.returnCodes['STUCK_PARTICLE']):
                    failedFile = testdict['testFile'].split('/')[-1]
                    stuckParticles.append(failedFile)
                if generalStatus.__contains__(GlobalData.returnCodes['OVERLAPS']):
                    failedFile = testdict['testFile'].split('/')[-1]
                    overlaps.append(failedFile)

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

        if len(overlaps) > 0:
            f.write("The following " + _np.str(len(overlaps)) + " generated files contain overlaps:\r\n")
            for test in overlaps:
                f.write(test + "\r\n")
            f.write("\r\n")

        if len(stuckParticles) > 0:
            f.write("The following " + _np.str(len(stuckParticles)) + " generated files had stuck particles:\r\n")
            for test in stuckParticles:
                f.write(test + "\r\n")
            f.write("\r\n")

        f.write(self._getGitCommit())
        f.close()

    def ProcessResults(self, componentType=''):
        if componentType != '':
            GlobalData._CheckComponent(componentType)

            testResults = self.Results[componentType]

            for index, result in enumerate(testResults):
                # comparatorLog = 'FailedTests/' + result['compLogFile']
                coords = self._getPhaseSpaceComparatorData(result, 'FailedTests/' + result['compLogFile'])
                self.Results[componentType][index]['resultsList'] = coords
        else:
            GlobalData._CheckComponent(componentType)  # raises value error

        self.Results[componentType].reverse()
        setattr(self.Results[componentType], 'uniqueValues', self.Results[componentType]._getUniqueValues())
        setattr(self.Results[componentType], 'commonValues', self.Results[componentType]._getCommonValues())

        with open('results.pickle', 'wb') as handle:
            pickle.dump(self.Results, handle, protocol=pickle.HIGHEST_PROTOCOL)

        with open('timing.pickle', 'wb') as handle:
            pickle.dump(self.TimingData, handle, protocol=pickle.HIGHEST_PROTOCOL)

        if (componentType == 'rbend') or (componentType == 'sbend'):
            self._groupDipoleResults(componentType)
            with open('dipoleResults.pickle', 'wb') as handle:
                pickle.dump(self.DipoleResults, handle, protocol=pickle.HIGHEST_PROTOCOL)

    def _groupDipoleResults(self, componentType=''):
        def _dictPolefaceCompVals(testparams):
            paramSet = collections.OrderedDict()
            paramSet['e1'] = testparams['e1']
            paramSet['e2'] = testparams['e2']
            paramSet['fint'] = testparams['fint']
            paramSet['fintx'] = testparams['fintx']
            paramSet['hgap'] = testparams['hgap']
            return paramSet

        def _updatGeneralStatus(_genStatus):
            genStat = []
            for codes in _genStatus:
                if (len(codes) == 1) and (not genStat.__contains__(codes[0])):
                    genStat.append(codes[0])
                elif len(codes) > 1:
                    for code in codes:
                        if not genStat.__contains__(code):
                            genStat.append(code)
            return genStat

        def _updateCoordsRes(_resultsList):
            resList = [[], [], [], [], [], [], []]
            coords = _np.array(_resultsList)
            for index in range(coords.shape[1]):
                values = coords[:, index]
                templist = []
                for i in values:
                    if not templist.__contains__(i):
                        templist.append(i)
                resList[index] = templist
            return resList

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

        if (componentType != 'rbend') and (componentType != 'sbend'):
            raise ValueError("Component must be an rbend or sbend.")
        params = self.Results[componentType].GetParams()
        uniqueValues = self.Results[componentType].uniqueValues
        generalStatus = self.Results[componentType].GetGeneralStatus()
        resultsList = self.Results[componentType].GetResults()
        dipoleResults = Results(componentType)

        for energy in uniqueValues['energy']:
            for length in uniqueValues['length']:
                if uniqueValues.keys().__contains__('angle'):
                    for angle in uniqueValues['angle']:
                        dipRes = _getResDict(energy, length, angleVal=angle)

                        _genStat = []
                        _resList = []
                        for testNum in range(self.Results[componentType]._numEntries):
                            testParams = params[testNum]
                            if (testParams['energy'] == energy) and (testParams['length'] == length) \
                                    and (testParams['angle'] == angle):
                                dipRes['particle'] = self.Results[componentType][testNum]['particle']
                                dipRes['polefaceParams'].append(_dictPolefaceCompVals(testParams))
                                _genStat.append(generalStatus[testNum])
                                _resList.append(resultsList[testNum])

                        dipRes['generalStatus'] = _updatGeneralStatus(_genStat)
                        dipRes['resultsList'] = _updateCoordsRes(_resList)

                        dipoleResults.append(dipRes)
                elif uniqueValues.keys().__contains__('field'):
                    for field in uniqueValues['field']:
                        dipRes = _getResDict(energy, length, fieldVal=field)

                        _genStat = []
                        _resList = []
                        for testNum in range(self.Results[componentType]._numEntries):
                            testParams = params[testNum]
                            if (testParams['energy'] == energy) and (testParams['length'] == length) \
                                    and (testParams['field'] == field):
                                dipRes['particle'] = self.Results[componentType][testNum]['particle']
                                dipRes['polefaceParams'].append(_dictPolefaceCompVals(testParams))
                                _genStat.append(generalStatus[testNum])
                                _resList.append(resultsList[testNum])

                        dipRes['generalStatus'] = _updatGeneralStatus(_genStat)
                        dipRes['resultsList'] = _updateCoordsRes(_resList)

                        dipoleResults.append(dipRes)
        self.DipoleResults[componentType] = dipoleResults

    def PlotResults(self, componentType=''):
        plotter = _Plotting()
        if (componentType == 'rbend') or (componentType == 'sbend'):
            plotter.PlotResults(self.DipoleResults, componentType)
        else:
            plotter.PlotResults(self.Results, componentType)

    def ProduceReport(self, pickled=False):
        if pickled:
            with open('results.pickle', 'rb') as handle:
                self.Results = pickle.load(handle)
            with open('dipoleResults.pickle', 'rb') as handle:
                self.DipoleResults = pickle.load(handle)

        report = _Report(self.Results, self.DipoleResults)
        report.ProduceReport()


class _Plotting:
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
        GlobalData._CheckComponent(componentType)

        res = allResults[componentType]

        # split results into proton and electron
        electronResults = res.GetResultsByParticle('e-')
        protonResults = res.GetResultsByParticle('proton')

        if electronResults._numEntries == 0:
            self._singleParticlePlots(protonResults)
        elif protonResults._numEntries == 0:
            self._singleParticlePlots(electronResults)
        elif (protonResults._numEntries > 0) and (electronResults._numEntries > 0):
            if (protonResults._numEntries <= self.testsPerAxes) and (electronResults._numEntries <= self.testsPerAxes):
                figsize = self._getFigSize(protonResults, electronResults)
                f = _plt.figure(figsize=figsize)
                ax1 = f.add_subplot(141)
                ax2 = f.add_subplot(142)
                ax3 = f.add_subplot(143)
                ax4 = f.add_subplot(144)
                res1Size = _np.float(figsize[1] - 1) / len(electronResults)
                res2Size = _np.float(figsize[1] - 1) / len(protonResults)

                res1Offset = 1.0
                res2Offset = 1.0

                if res1Size > res2Size:
                    res1Offset = res2Size / res1Size
                elif res1Size < res2Size:
                    res2Offset = res1Size / res2Size

                dataAx1 = self._updateAxes(ax2, ax1, electronResults, res1Offset)
                dataAx2 = self._updateAxes(ax4, ax3, protonResults, res2Offset)
                self._addColorBar(f, dataAx1)
                f.savefig('../Results/' + electronResults._component + '.png', dpi=600)
            else:
                self._singleParticlePlots(electronResults)
                self._singleParticlePlots(protonResults)

    def _singleParticlePlots(self, results):
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
            self._singleDataAxes(results)

    def _doubleDataAxesByEnergy(self, numFigures, results):
        for i in range(numFigures):
            energy1 = results.uniqueValues['energy'][2 * i]
            energy2 = results.uniqueValues['energy'][2 * i + 1]

            res1 = results.GetResultsByEnergy(energy1)
            res2 = results.GetResultsByEnergy(energy2)
            particle = results[0]['particle']
            energyString = '_' + particle + '_energies__' + energy1 + '_' + energy2

            figsize = (15, 9)
            f = _plt.figure(figsize=figsize)
            ax1 = f.add_subplot(141)
            ax2 = f.add_subplot(142)
            ax3 = f.add_subplot(143)
            ax4 = f.add_subplot(144)
            res1Size = _np.float(figsize[1] - 1) / len(res1)
            res2Size = _np.float(figsize[1] - 1) / len(res2)

            res1Offset = 1.0
            res2Offset = 1.0

            if res1Size > res2Size:
                res1Offset = res2Size / res1Size
            elif res1Size < res2Size:
                res2Offset = res1Size / res2Size

            dataAx1 = self._updateAxes(ax2, ax1, res1, res1Offset)
            dataAx2 = self._updateAxes(ax4, ax3, res2, res2Offset)
            self._addColorBar(f, dataAx1)
            f.savefig('../Results/' + results._component + energyString + '.png', dpi=600)

    def _singleDataAxesByEnergy(self, results):
        figsize = self._getFigSize(results)
        f = _plt.figure(figsize=figsize)
        ax1 = f.add_subplot(121)
        ax2 = f.add_subplot(122)

        res1 = results.GetResultsByEnergy(results.uniqueValues['energy'][-1])
        energy1 = results.uniqueValues['energy'][-1]
        particle = results[0]['particle']
        energyString = '_' + particle + '_energies__' + energy1

        dataAxes = self._updateAxes(ax2, ax1, res1, 1.0)
        self._addColorBar(f, dataAxes)
        f.savefig('../Results/' + results._component + energyString + '.png', dpi=600)

    def _singleDataAxes(self, results):
        figsize = self._getFigSize(results)
        f = _plt.figure(figsize=figsize)
        ax1 = f.add_subplot(121)
        ax2 = f.add_subplot(122)

        dataAxes = self._updateAxes(ax2, ax1, results, 1.0)
        self._addColorBar(f, dataAxes)
        f.savefig('../Results/' + results._component + '.png', dpi=600)

    def _addColorBar(self, f, ax):
        # colorbar colors and values independant of data, can be set according to either subplot.
        cbar = f.colorbar(ax)
        cbarTicks = _np.linspace(0.5, len(GlobalData.returnCodes) - 0.5, len(GlobalData.returnCodes))
        cbar.set_ticks(cbarTicks)
        cbar.set_ticklabels(GlobalData.returnCodes.keys())
        cbar.ax.tick_params(labelsize=8)
        f.tight_layout()

    def _updateAxes(self, dataAxis, diagramAxis, dataSetresults, labOffset):
        uniqueValues = dataSetresults.uniqueValues

        def updateDataAxis(ax, results):
            # get all necessary data.
            data = results.GetResults()
            generalStatus = results.GetGeneralStatus()
            particle = results[0]['particle']
            numTests = len(results)

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
            bounds = _np.linspace(0, len(GlobalData.returnCodes), len(GlobalData.returnCodes) + 1)
            norm = _color.BoundaryNorm(bounds, GlobalData.cmap.N)

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
                        if len(subplotTitle.split('\n')[-1]) > 22:
                            subplotTitle += '\n'

            cax = ax.imshow(zeroData, interpolation='none', origin='lower', cmap=GlobalData.cmap, norm=norm,
                            extent=extent, aspect='auto')
            ax.set_xlim(0, 8)

            for index, status in enumerate(data):
                for coord, vals in enumerate(status):
                    numStatus = len(vals)
                    yIndex = index
                    for statIndex, stat in enumerate(vals):
                        boxColor = GlobalData.cmap.colors[_np.int(stat)]
                        boxWidth = 1.0 / numStatus
                        ax.add_patch(_patches.Rectangle((coord + statIndex*boxWidth, yIndex), boxWidth, 1, color=boxColor))

            for index, status in enumerate(generalStatus):
                numStatus = len(status)
                yIndex = index
                for statIndex, stat in enumerate(status):
                    boxColor = GlobalData.cmap.colors[stat]
                    boxWidth = 1.0 / numStatus
                    ax.add_patch(_patches.Rectangle((7 + statIndex*boxWidth, yIndex), boxWidth, 1, color=boxColor))

            if subplotTitle != '':
                ax.set_title(subplotTitle)

            xtickMajors = _np.linspace(1, 8, 8)
            xtickCentre = xtickMajors - 0.5

            ax.set_xticks(xtickCentre)
            ax.set_xticklabels(['x', 'xp', 'y', 'yp', 't', 'zp', 'n', 'Gen'])

            ytickMajors = _np.linspace(numTests / (numTests - 1), numTests, numTests)
            ytickCentre = ytickMajors - 0.5

            ax.set_yticks(ytickCentre)
            empty_string_labels = [''] * numTests
            ax.set_yticklabels(empty_string_labels)

            ytickMinors = _np.linspace(0, len(data), len(data) + 1)

            minorXTicks = _tick.FixedLocator(xtickMajors)
            minorYTicks = _tick.FixedLocator(ytickMinors)

            ax.xaxis.set_minor_locator(minorXTicks)
            ax.yaxis.set_minor_locator(minorYTicks)

            ax.tick_params(axis='x', which='both', length=0)
            ax.tick_params(axis='y', which='both', length=0, labelsize=9)

            ax.grid(which='minor', axis='x', linestyle='-')
            ax.grid(which='minor', axis='y', linestyle='--')
            return cax

        def updateDiagramAxis(ax, results, labelOffset):
            # get all necessary data.
            params = results.GetParams()
            # dict for param value labels
            boxText = {}

            # calculate number of boxes per order
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
                            rotation=textRot, va='center', ha='center', fontsize=fontSizes[level])
                dataValue = _string.capitalize(uniqueValues.keys()[level])
                ax.text(level+0.5 - numLevels, len(params) + labelOffset, dataValue,
                        fontsize=12, ha='center')

            ax.set_xlim(-numLevels, 0)
            ax.set_ylim(0, results._numEntries)

            ax.axis('off')
            ax.xaxis.set_visible(False)
            ax.yaxis.set_visible(False)

            return ax

        dataAx1 = updateDataAxis(dataAxis, dataSetresults)
        labAx1 = updateDiagramAxis(diagramAxis, dataSetresults, labOffset)

        return dataAx1


class _Report:
    def __init__(self, results, dipoleResults):
        self.Results = results
        self.DipoleResults = dipoleResults
        self.groupedResults = {}
        self._numResults = {}
        self._fileName = "../Results/TestResults_" + _time.strftime("%d%m%Y_%H%M%S", _time.gmtime()) + ".txt"

    def _getResults(self, component):
        compResults = None
        if ((component == 'rbend') or (component == 'sbend')) and self.DipoleResults.keys().__contains__(component):
            compResults = self.DipoleResults[component]
        elif self.Results.keys().__contains__(component):
            compResults = self.Results[component]
        return compResults

    def _groupResults(self, resultsList, component):
        groupedResults = {}
        numResults = {}
        for error, codeIndex in GlobalData.returnCodes.iteritems():
            groupedResults[error] = Results(component)
            numResults[error] = 0
            for resultNum, result in enumerate(resultsList):
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
        results = self.Results[component]
        coordLabels = ['x', 'xp', 'y', 'yp', 't', 'zp', 'n']
        numFailures = [0, 0, 0, 0, 0, 0, 0]
        for res in results:
            compResults = res['resultsList']
            for index, coord in enumerate(compResults):
                if coord == 1:
                    numFailures[index] += 1
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

        for i in range(7):
            s += "  " + coordLabels[i] + ":\r\n"
            failedTests = Results(component)
            for res in results:
                compResults = res['resultsList']
                if compResults[i] == 1:
                    failedTests.append(res)

            commonFactors = {}
            globalParams = {}
            for res in failedTests:
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
            if commonFactors.keys().__len__() > 0:
                for param, value in commonFactors.iteritems():
                    s += "    " + param + " : " + _np.str(value) + ".\r\n"
        return s

    def _componentSectionTitle(self, component):
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
        overallRes = self._numResults[component]
        totalTests = self.Results[component].__len__()
        s = "Overall Results: \r\n"
        s += "  Passed: " + _np.str(overallRes['SUCCESS']) + "/" + _np.str(totalTests) + ",\r\n"
        s += "  Soft Failure: " + _np.str(overallRes['FAILED']) + "/" + _np.str(totalTests) + ",\r\n"
        s += "  Hard Failure: " + _np.str(overallRes['FILE_NOT_FOUND']) + "/" + _np.str(totalTests) + ".\r\n"
        s += "\r\n"
        s += "Breakdown of Hard Failures: \r\n"
        numHardFailures = overallRes['FILE_NOT_FOUND']
        if overallRes['FATAL_EXCEPTION'] > 0:
            s += "  Fatal Exceptions: " + _np.str(overallRes['FATAL_EXCEPTION']) + "/" + \
                 _np.str(overallRes['FILE_NOT_FOUND']) + ",\r\n"
            numHardFailures -= overallRes['FATAL_EXCEPTION']
            fatalString = self._processFatals(component)
            s += fatalString
        if overallRes['TIMEOUT'] > 0:
            s += "  Timeouts: " + _np.str(overallRes['TIMEOUT']) + "/" + \
                 _np.str(overallRes['FILE_NOT_FOUND']) + ",\r\n"
            numHardFailures -= overallRes['TIMEOUT']
        s += "  Unknown Reason: " + _np.str(numHardFailures) + "/" + _np.str(overallRes['FILE_NOT_FOUND']) + "\r\n"
        s += "    (likely to be BDSIM self exit).\r\n"
        s += "\r\n"
        softStr = "Breakdown of Soft Failures: \r\n"
        softFailures = False
        for index, failure in enumerate(GlobalData.softFailures):
            if overallRes[failure] > 0:
                softFailures = True
                strName = failure.replace("_", " ")
                strName = _string.capitalize(strName)
                softStr += "  " + strName + ": " + _np.str(overallRes[failure])
                if index == len(GlobalData.softFailures)-1:
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
        for componentType in GlobalData.components:
            results = self._getResults(componentType)

            if results is not None:
                self._groupResults(results, componentType)
                self._componentSection(componentType)
            else:
                pass
