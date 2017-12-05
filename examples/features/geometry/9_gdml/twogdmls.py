import pygeometry as pg
import pygeometry.geant4 as g4

def Generate():
    GenerateFile1()
    GenerateFile2()

def GenerateFile1():
    b1s = g4.solid.Box('box', 20, 30, 40)
    b1l = g4.LogicalVolume(b1s, 'G4_Cu', 'boxlv')

    worlds = g4.solid.Box('world', 200, 200, 200)
    worldl = g4.LogicalVolume(worlds, 'G4_Galactic', 'worldlv')

    b1p = g4.PhysicalVolume([0,0,0], [0,0,0],
                            b1l,
                            "boxpv",
                            worldl)

    g4.registry.setWorld('worldlv')
    w = pg.gdml.Writer()
    w.addDetector(g4.registry)
    w.write("box1.gdml")
    

def GenerateFile2():
    # purposively give it the same name to test different file loading
    b2s = g4.solid.Orb('box', 34)
    b2l = g4.LogicalVolume(b2s, 'G4_Cu', 'boxlv')

    worlds = g4.solid.Box('world', 200, 200, 200)
    worldl = g4.LogicalVolume(worlds, 'G4_Galactic', 'worldlv')

    b2p = g4.PhysicalVolume([0,0,0], [0,0,0],
                            b2l,
                            "boxpv",
                            worldl)

    g4.registry.setWorld('worldlv')
    w = pg.gdml.Writer()
    w.addDetector(g4.registry)
    w.write("box2.gdml")
