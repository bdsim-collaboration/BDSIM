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

    def SetTotalTime(self,totalTime):
        try:
            totalTime = _np.float(totalTime)
            self.totalTime = totalTime
        except ValueError:
            pass

    def AddComponentTime(self,component,componentTime):
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
        self.results = []
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
        _os.chdir(pwd)

        # branch info should only be one line of text
        f = open('gitBranch.log')
        branchLine = f.next()
        f.close()
        # get branch name
        branchLine = branchLine.strip('\r')
        branchLine = branchLine.strip('\n')
        branch = branchLine.split(' ')[1]

        gitLines = "BDSIM was built from the git repository using the branch " + branch + ",\r\n"
        gitLines += "and was at the following commit: \r\n"
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
            self.results.append(results)
        elif isinstance(results, list):
            self.results.extend(results)

    def AddTimingData(self, timingData):
        if not isinstance(timingData, Timing):
            raise TypeError("Timing data muse be a TestResults.Timing instance.")
        else:
            self.timingData = timingData

    def ProcessOriginals(self):
        numFailed = 0
        for testdict in self.results:
            if testdict['code'] != 0:
                numFailed += 1
            # TODO: Handle other return types (i.e overlaps, stuck particles, tracking warnings etc)

        numTests = len(self.results)
        s = _np.str(numTests - numFailed) + "/" + _np.str(numTests) + " ROOT files were successfully generated.\r\n"
        print(s)

        f = open('DataSetGeneration.log', 'a')
        timestring = '! ' + _time.strftime("%a, %d %b %Y %H:%M:%S +0000", _time.gmtime()) + '\n'
        f.write(timestring)
        f.write('\r\n')
        f.write(s)

        # TODO: Write out reasons for failure.
        f.write(self._getGitCommit())
        f.close()
