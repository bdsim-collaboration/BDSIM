import sys

from dataclasses import dataclass
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import pybdsim
import pymadx


variables = ["x", "px", "y", "py"]

prim = "Primary"

XLABELS = {"x": r"Primary $x$ / m",
           "y": r"Primary $y$ / m",
           "px": r"Primary $p_x / p_0$",
           "py": fr"Primary $p_y / p_0$"}


def is_root(filename):
    with open(filename, "rb") as f:
        return f.read(4) == b"root"

def is_ptc(filename):
    with open(filename, "r") as f:
        return "TRACKONE" in f.readline()

class OpticalData: # primary or sampler.
    def __init__(self, x, px, y, py):
        self.x = x
        self.px = px
        self.y = y
        self.py = py

    def __getitem__(self, key):
        return getattr(self, key)

    def __repr__(self):
        return repr({"x": self.x, "px": self.px, "y": self.y, "py": self.py})


class PTC:
    def __init__(self, filename):
        if not is_ptc(filename):
            raise TypeError(f"Not a PTC file: {filename}")
        self.filename = filename

        self.data = self.load(self.filename)

        self.primaries = self._load_primaries()
        self.output = self._load_output()

    def _load_primaries(self):
        s = self._start()
        return OpticalData(s.GetColumn("X"),
                           s.GetColumn("PX"),
                           s.GetColumn("Y"),
                           s.GetColumn("PY"))

    def _load_output(self):
        e = self._end()
        return OpticalData(e.GetColumn("X"),
                           e.GetColumn("PX"),
                           e.GetColumn("Y"),
                           e.GetColumn("PY"))

    def _end(self):
        return self.data.GetSegment("end")

    def _start(self):
        return self.data.GetSegment("start")

    def load(self, filename):
        return pymadx.Data.Tfs(filename)

    def title(self):
        return self.data.header["TITLE"].upper()

class BDSIM:
    def __init__(self, filename):
        if not is_root(filename):
            raise TypeError(f"Not a ROOT file: {filename}")
        self.filename = filename

        self._beam = pybdsim.Data.BeamData.FromROOTFile(filename)
        self.data = self.load(self.filename)
        self.primaries = self._load_primaries()
        self.output = self._load_samplers()
        
    def load(self, filename):
        d = pybdsim.Data.Load(filename)
        return d

    def _convert_sampler_data(self, sampler):
        assert self._beam.particle == "proton", "must be a proton"
        mass = 0.9383 
        reference_momentum = (self._beam.beamEnergy**2 - mass**2)**0.5
        momentum = (sampler.data["energy"]**2 - mass**2)**0.5

        px = self._to_canonical_momentum(sampler.data["xp"],
                                         momentum,
                                         reference_momentum)
        py = self._to_canonical_momentum(sampler.data["yp"],
                                         momentum,
                                         reference_momentum)
        return OpticalData(sampler.data["x"], px, sampler.data["y"], py)

    @staticmethod
    def _to_canonical_momentum(divergence, p, p0):
        return divergence * p / p0

    def _load_primaries(self):
        s = pybdsim.Data.SamplerData(self.data, 0)
        return self._convert_sampler_data(s)

    def _load_samplers(self):
        s = pybdsim.Data.SamplerData(self.data, -1)
        return self._convert_sampler_data(s)

    def title(self):
        return self.filename.split(".")[0].upper()


def main(first, tracker):
    if is_root(first):
        first = BDSIM(first)
    else:
        first = PTC(first)

    tracker = BDSIM(tracker)
    title = first.title()

    primaries = first.primaries

    f, axes = plt.subplots(nrows=2, ncols=2, sharex=True, sharey=False,
                           figsize=(11,6))

    axes = axes.reshape(4,)

    for i, (ax, variable) in enumerate(zip(axes, variables)):
        vfirst = first.output[variable] 
        vtracker = tracker.output[variable]
        ax.plot(primaries[variable], abs(vfirst - vtracker),
                marker="x")

        assert len(vfirst) == len(vtracker), \
            ("Length mismatch", (len(vfirst), len(vtracker)))

        # from IPython import embed; embed()
        if i == 0 or i == 2:
            # ax.set_ylabel(
            #     r'$\frac{|\mathrm{BSDIM}-\mathrm{tracker}|}{\mathrm{PTC}}$')
            first_name = type(first).__name__
            ax.set_ylabel(rf"$|\mathrm{{{first_name}}}-\mathrm{{tracker}}$|")
        ax.set_xlabel(XLABELS[variable])

        ax.set_xscale("log")
        ax.set_yscale("log")

    f.suptitle(title, size="xx-large")

    # f.tight_layout()
    f.savefig(f"{title.lower()}.pdf", bbox_inches="tight")

    # plt.show()

if __name__ == '__main__':

    first = sys.argv[1]
    tracker = sys.argv[2]
    main(first, tracker)
