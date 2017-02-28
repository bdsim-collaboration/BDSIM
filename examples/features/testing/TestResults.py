import numpy as _np
import os as _os
import time as _time
import Globals
from matplotlib import colors as _color
from matplotlib import ticker as _tick
import matplotlib.pyplot as _plt

# data type with multiple entries that can be handled by the functions.
multiEntryTypes = [tuple, list, _np.ndarray]

GlobalData = Globals.Globals()


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
        self.Results = {}
        for component in GlobalData.components:
            self.Results[component] = []
        self.timingData = None
        self._resultsList = {}
        self._filesList = {}
        self._generalStatusList = {}

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

        return coords

    def _getBDSIMLogData(self, result, logFile=''):
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
        f = open(logFile)
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


class Results(ResultsUtilities):
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
        f = open('gitBranch.log','r')
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
        if isinstance(results, dict):
            self.Results[results['componentType']].append(results)
        elif multiEntryTypes.__contains__(type(results)):
            for res in results:
                self.Results[results['componentType']].append(res)

    def AddTimingData(self, timingData):
        if not isinstance(timingData, Timing):
            raise TypeError("Timing data muse be a TestResults.Timing instance.")
        else:
            self.timingData = timingData

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

        # TODO: Write out reasons for failure.
        if numFailed > 0:
            f.write("The following tests failed to generate ROOT output:\r\n")
            for test in failedTests:
                f.write(test + "\r\n")
            f.write("\r\n")

        f.write(self._getGitCommit())
        f.close()

    def ProcessResults(self, componentType=''):
        if (componentType != '') and (not GlobalData.components.__contains__(componentType)):
            raise ValueError("Unknown component type.")
        elif componentType != '':
            testResults = self.Results[componentType]
            if not self._resultsList.keys().__contains__(componentType):
                self._resultsList[componentType] = []
                self._filesList[componentType] = []
                self._generalStatusList[componentType] = []

            for index, result in enumerate(testResults):
                comparatorLog = 'FailedTests/' + result['compLogFile']
                coords = self._getPhaseSpaceComparatorData(result, comparatorLog)
                generalStatus = result['generalStatus']

                self._resultsList[componentType].append(coords)
                self._generalStatusList[componentType].append(generalStatus)

                filename = result['ROOTFile']
                filename = filename.replace('_event.root', '')
                filename = filename.replace((componentType + "__"), '')
                self._filesList[componentType].append(filename)
        else:
            pass
        self._resultsList[componentType].reverse()
        self._filesList[componentType].reverse()
        self._generalStatusList[componentType].reverse()

    def PlotResults(self, componentType=''):
        f = _plt.figure(figsize=(15, 7))
        ax = f.add_subplot(111)

        # set normalised colormap.
        bounds = _np.linspace(0, len(GlobalData.returnCodes), len(GlobalData.returnCodes) + 1)
        norm = _color.BoundaryNorm(bounds, GlobalData.cmap.N)

        extent = [0, 8, 0, len(self._resultsList[componentType])]

        data = self._resultsList[componentType]

        files = self._filesList[componentType]

        cax = ax.imshow(data, interpolation='none', origin='lower', cmap=GlobalData.cmap, norm=norm, extent=extent)

        xtickMajors = _np.linspace(1, 8, 8)
        xtickCentre = xtickMajors - 0.5

        ax.set_xticks(xtickCentre)
        ax.set_xticklabels(['x', 'px', 'y', 'py', 't', 'pt', 'n', 'Gen'])
        ax.set_xlim(0, 8)

        ytickMajors = _np.linspace(len(files) / (len(files) - 1), len(files), len(files))
        ytickCentre = ytickMajors - 0.5

        ax.set_yticks(ytickCentre)
        ax.set_yticklabels(files)

        ytickMinors = _np.linspace(0, len(data), len(data) + 1)

        # ax.xaxis.set_visible(False)

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

