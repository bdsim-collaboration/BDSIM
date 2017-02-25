import numpy as _np
import os as _os
import string as _string
import glob as _glob
import time as _time
import Globals


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


class Results:
    def __init__(self):
        self.Results = {}
        for component in GlobalData.components:
            self.Results[component] = []
        self.timingData = None

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
        for component, compResults in self.Results.iteritems():
            for testdict in compResults:
                numTests += 1
                if (testdict['code'] != 0) and (testdict['code'] != None):
                    numFailed += 1
                    failedFile = testdict['testfile'].split('/')[-1]
                    failedTests.append(failedFile)
                # TODO: Handle other return types (i.e overlaps, stuck particles, tracking warnings etc)

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
            self._resultsList = []

            for index,result in enumerate(testResults):
                comparatorLog = 'FailedTests/' + result['compLogFile']
                coords = self._getPhaseSpaceComparatorData(result, comparatorLog)
                self._resultsList.append(coords)
        else:
            pass
        self._resultsList.reverse()

    def _getPhaseSpaceComparatorData(self, result, logFile=''):
        # phasespace coords initialised as passed.
        coords = _np.zeros(7)
        
        # return all true if comparator returned passed
        code = result['code']
        if code == 0:
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
            branches = offendingSamplerBranchesLine.replace('Event Tree (1/2) entries ','')
            branches = branches.replace('(', '')
            branches = branches.replace(')', '')
            numParticles = branches.split('/')
            if numParticles[0] != numParticles[1]:
                coords[6] = GlobalData.returnCodes['FAILED']
                coords[0:5] = GlobalData.returnCodes['NO_DATA']
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

