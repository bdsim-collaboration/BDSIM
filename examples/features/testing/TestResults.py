import numpy as _np
import os as _os
import time as _time
import collections
import Globals
import string as _string
from matplotlib import colors as _color
from matplotlib import ticker as _tick
import matplotlib.pyplot as _plt
import matplotlib.patches as _patches


# data type with multiple entries that can be handled by the functions.
multiEntryTypes = [tuple, list, _np.ndarray]

GlobalData = Globals.Globals()


class Results(dict):
    def __init__(self, component=''):
        if not GlobalData.components.__contains__(component):
            raise ValueError("Unknown component type.")
        self._component = component

        self['timingData'] = []
        self['resultsList'] = []
        self['fileLabel'] = []
        self['generalStatusList'] = []
        self['params'] = []
        self['testResults'] = []

    def GetElectronResults(self):
        electronResults = Results(self._component)


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
        self.ResultsDict = {}

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
                      '-------- WWWW ------- G4Exception-START -------- WWWW ------- ']
        endLines = ['-------- WWWW -------- G4Exception-END --------- WWWW -------',
                    '-------- WWWW -------- G4Exception-END --------- WWWW ------- ']

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
                if exceptions.__contains__('      issued by : G4PVPlacement::CheckOverlaps()'):
                    generalStatus.append(GlobalData.returnCodes['OVERLAPS'])
                if exceptions.__contains__('     issued by : G4PropagatorInField::ComputeStep()'):
                    generalStatus.append(GlobalData.returnCodes['STUCK_PARTICLE'])
                    # TODO: check for other types of warnings/errors.

        return generalStatus

    def _getCommonValues(self, componentType=''):
        uniqueValues = collections.OrderedDict()
        # if self._testParamValues is None:
        #     return None
        for test in self.ResultsDict[componentType]['params']:
            for key, value in test.iteritems():
                if not uniqueValues.keys().__contains__(key):
                    uniqueValues[key] = []
                if not uniqueValues[key].__contains__(value):
                    uniqueValues[key].append(value)

        commonValues = collections.OrderedDict()
        for key, value in uniqueValues.iteritems():
            if len(value) == 1:
                commonValues[key] = value[0]

        for values in commonValues.values():
            if len(values) > 0:
                return commonValues
        return None


class Analysis(ResultsUtilities):
    def __init__(self):
        ResultsUtilities.__init__(self)

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

    def AddResults(self, results):
        if not self.ResultsDict.keys().__contains__(results['componentType']):
            self.ResultsDict[results['componentType']] = Results(results['componentType'])
        if isinstance(results, dict):
            self.ResultsDict[results['componentType']]['testResults'].append(results)
        elif multiEntryTypes.__contains__(type(results)):
            for res in results:
                self.ResultsDict[results['componentType']]['testResults'].append(res)

    def AddTimingData(self, componentType, timingData):
        if not isinstance(timingData, Timing):
            raise TypeError("Timing data muse be a TestResults.Timing instance.")
        else:
            self.ResultsDict[componentType]['timingData'].append(timingData)
        dummy = 1

    def ProcessOriginals(self):
        numTests = 0
        numFailed = 0
        failedTests = []
        stuckParticles = []
        overlaps = []
        for component, compResults in self.ResultsDict.iteritems():
            for testdict in compResults['testResults']:
                numTests += 1

                generalStatus = testdict['generalStatus']

                if (generalStatus is not None) and (not generalStatus.__contains__(0)):
                    numFailed += 1
                    failedFile = testdict['testfile'].split('/')[-1]
                    failedTests.append(failedFile)
                if generalStatus.__contains__(GlobalData.returnCodes['STUCK_PARTICLE']):
                    failedFile = testdict['testfile'].split('/')[-1]
                    stuckParticles.append(failedFile)
                if generalStatus.__contains__(GlobalData.returnCodes['OVERLAPS']):
                    failedFile = testdict['testfile'].split('/')[-1]
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
        if (componentType != '') and (not GlobalData.components.__contains__(componentType)):
            raise ValueError("Unknown component type.")
        elif componentType != '':
            testResults = self.ResultsDict[componentType]['testResults']
            if not self.ResultsDict.keys().__contains__(componentType):
                self.ResultsDict[componentType] = Results(componentType)

            for index, result in enumerate(testResults):
                comparatorLog = 'FailedTests/' + result['compLogFile']
                coords = self._getPhaseSpaceComparatorData(result, comparatorLog)

                self.ResultsDict[componentType]['generalStatusList'].append(result['generalStatus'])
                self.ResultsDict[componentType]['resultsList'].append(coords)

                filename = result['ROOTFile']
                filename = filename.replace('_event.root', '')
                filename = filename.replace((componentType + "__"), '')

                self.ResultsDict[componentType]['fileLabel'].append(filename)
                self.ResultsDict[componentType]['params'].append(result['testParams'])
        else:
            pass

        self.ResultsDict[componentType]['resultsList'].reverse()
        self.ResultsDict[componentType]['fileLabel'].reverse()
        self.ResultsDict[componentType]['generalStatusList'].reverse()
        self.ResultsDict[componentType]['params'].reverse()

    def PlotResults(self, componentType=''):
        f = _plt.figure(figsize=(11, 7))
        ax = f.add_subplot(111)

        # set normalised colormap.
        bounds = _np.linspace(0, len(GlobalData.returnCodes), len(GlobalData.returnCodes) + 1)
        norm = _color.BoundaryNorm(bounds, GlobalData.cmap.N)

        extent = [0, 7, 0, len(self.ResultsDict[componentType]['resultsList'])]

        data = self.ResultsDict[componentType]['resultsList']
        files = self.ResultsDict[componentType]['fileLabel']
        generalStatus = self.ResultsDict[componentType]['generalStatusList']

        electronFiles = [i for i, x in enumerate(files) if x.__contains__('e-')]
        protonFiles = [i for i, x in enumerate(files) if x.__contains__('proton')]

        commonValues = self._getCommonValues(componentType)
        subplotTitle = ''

        if commonValues is not None:
            for index, fileName in enumerate(files):
                fname = fileName
                for key, value in commonValues.iteritems():
                    filestring = "__" + key + "_" + _np.str(value)
                    fname = fname.replace(filestring, '')
                if len(electronFiles) == len(files):
                    fname.replace("e-__", "")
                elif len(protonFiles) == len(files):
                    fname.replace("proton__", "")
                files[index] = fname

            for key, value in commonValues.iteritems():
                subplotTitle += _string.capitalize(key) + " = " + value
                index = commonValues.keys().index(key)
                if index != (len(commonValues.keys()) - 1):
                    subplotTitle += ", "

        cax = ax.imshow(data, interpolation='none', origin='lower', cmap=GlobalData.cmap, norm=norm,
                        extent=extent, aspect='auto')
        ax.set_xlim(0, 8)

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
        ax.set_xticklabels(['x', 'xp', 'y', 'yp', 't', 'zp', 'n', 'General'])

        ytickMajors = _np.linspace(len(files) / (len(files) - 1), len(files), len(files))
        ytickCentre = ytickMajors - 0.5

        ax.set_yticks(ytickCentre)
        ax.set_yticklabels(files)

        ytickMinors = _np.linspace(0, len(data), len(data) + 1)

        minorXTicks = _tick.FixedLocator(xtickMajors)
        minorYTicks = _tick.FixedLocator(ytickMinors)

        ax.xaxis.set_minor_locator(minorXTicks)
        ax.yaxis.set_minor_locator(minorYTicks)

        ax.tick_params(axis='x', which='both', length=0)
        ax.tick_params(axis='y', which='both', length=0, labelsize=9)

        ax.grid(which='minor', axis='x', linestyle='-')
        ax.grid(which='minor', axis='y', linestyle='--')

        cbar = f.colorbar(cax)
        cbarTicks = _np.linspace(0.5, len(GlobalData.returnCodes) - 0.5, len(GlobalData.returnCodes))
        cbar.set_ticks(cbarTicks)
        cbar.set_ticklabels(GlobalData.returnCodes.keys())

        f.tight_layout()

