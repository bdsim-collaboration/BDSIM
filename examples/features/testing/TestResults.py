import numpy as _np
import os as _os
import string as _string
import glob as _glob

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
        s += _np.str(_np.std(self.comparatorTimes))+ '.\r\n'
        return s


class Results:
    def __init__(self):
        self.results = []
        self.timingData = None

    def AddResults(self, results):
        if isinstance(results, dict):
            self.results.append(results)
        elif isinstance(results, list):
            self.results.extend(results)

    def AddTimingData(self, timingData):
        if not isinstance(timingData,Timing):
            raise TypeError("Timing data muse be a TestResults.Timing instance.")
        else:
            self.timingData = timingData

    def ProcessResults(self):
        pass


