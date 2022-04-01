# load pyg4ometry
import pyg4ometry

# registry to store gdml data
reg  = pyg4ometry.geant4.Registry()

# world solid 
ws_1   = pyg4ometry.geant4.solid.Box("ws_1",199.1,199.1,0.1*199.1,reg)

# solids
all_box = pyg4ometry.geant4.solid.Box("all_box",170.1,170.1,0.1*199.2,reg)
inner_box = pyg4ometry.geant4.solid.Box("inner_box",160.1,160.1,0.1*199.2,reg)
box = pyg4ometry.geant4.solid.Subtraction("box",all_box,inner_box,[[0,0,0],[0,0,0]],reg)

# world logical volume
wl   = pyg4ometry.geant4.LogicalVolume(ws_1,"G4_Galactic","wl",reg)

# logical volumes
box_l = pyg4ometry.geant4.LogicalVolume(box,"G4_Fe","box_l",reg)
box_p = pyg4ometry.geant4.PhysicalVolume([0,0,0],[0,0,0],box_l,"box_p",wl,reg)

reg.setWorld(wl.name)

w = pyg4ometry.gdml.Writer()
w.addDetector(reg)
w.write('Example/gdml/box.gdml')