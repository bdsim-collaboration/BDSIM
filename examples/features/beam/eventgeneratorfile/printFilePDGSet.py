import pyhepmc as hep


def Analyse(filename):

    f = hep.io.ReaderAsciiHepMC2(filename)
    if f.failed():
        raise IOError("can't open file")

    eventIndex = 0
    while not f.failed():
        evt = hep.GenEvent()
        readok = f.read_event(evt)
        if f.failed():
            break
        pdgids = set([p.pid for p in evt.particles])
        print("Event #:", eventIndex)
        print(sorted(pdgids))
        eventIndex += 1
        evt.clear()

    f.close()
