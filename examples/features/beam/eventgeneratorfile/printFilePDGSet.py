import pyhepmc as hep
from collections import defaultdict


def Analyse(filename):

    f = hep.io.ReaderAsciiHepMC2(filename)
    if f.failed():
        raise IOError("can't open file")

    histogram = defaultdict(int)

    eventIndex = 0
    sets = []
    while not f.failed():
        evt = hep.GenEvent()
        readok = f.read_event(evt)
        if f.failed():
            break
        pdgids = set([p.pid for p in evt.particles])
        sets.append(pdgids)
        print("Event #:", eventIndex)
        print(sorted(pdgids))
        eventIndex += 1
        evt.clear()

    # print out unique pdgids in each event
    for i,s in enumerate(sets):
        # form a set of all pdgids across all events but this one
        allothereventpdgids = set().union(*[v for j,v in enumerate(sets) if j != i])
        unique = s.difference(allothereventpdgids)
        if len(unique) > 0:
            print("Event ",i,"unique pdgids: ",unique)
        else:
            print("Event ",i,"no unique pdgids")

        for key in s:
            histogram[key] += 1

    allpdgids = set().union(*sets)
    print("\nAll pdgids across the file: ",sorted(allpdgids))

    print("\nCount of number of events with a given PDG ID")
    for key in sorted(histogram.keys()):
        print(key,histogram[key])

    f.close()


if __name__ == "__main__":
    Analyse("egf-hepmc2-4pp-100gev-evts.dat")
