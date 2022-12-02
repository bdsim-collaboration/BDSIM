import pyg4ometry as pg
import pyg4ometry.geant4 as g4

def GenerateFile(option):
    reg = g4.Registry()
    b1s = g4.solid.Box('box', 20, 30, 40, reg)

    if option == 1:
        # "iron" is a material already defined in bdsim but we're creating
        # a unique conflicting definition here
        mat = g4.MaterialSingleElement("iron",26,55.8452,7.874,reg) # iron at near room temp
    elif option == 2:
        # test a more involved material... both elements and material
        # should conflict with pre-existing ones in bdsim
        mat = g4.MaterialCompound("air",1.290e-3,2,reg)
        ne = g4.ElementSimple("nitrogen","N",7,14.01)
        oe = g4.ElementSimple("oxygen","O",8,16.0)
        mat.add_element_massfraction(ne,0.7)
        mat.add_element_massfraction(oe,0.3)

    
    b1l = g4.LogicalVolume(b1s, mat, 'boxlv', reg)

    worlds = g4.solid.Box('world', 200, 200, 200, reg)
    worldl = g4.LogicalVolume(worlds, 'G4_Galactic', 'worldlv', reg)

    b1p = g4.PhysicalVolume([0,0,0], [0,0,0], b1l, "boxpv", worldl, reg)

    reg.setWorld(worldl)
    w = pg.gdml.Writer()
    w.addDetector(reg)
    w.write("box-w-bdsim-material-conflict-"+str(option)+".gdml")


if __name__ == "__main__":
    GenerateFile(1)
    GenerateFile(2)
