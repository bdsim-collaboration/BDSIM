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

all_pipe = pyg4ometry.geant4.solid.Tubs("all_pipe",0,20,199.2*1,0,2*m.pi,reg)
inner_pipe = pyg4ometry.geant4.solid.Tubs("inner_pipe",0,15,199.2*1,0,2*m.pi,reg)
pipe = pyg4ometry.geant4.solid.Subtraction("pipe",all_pipe,inner_pipe,[[0,0,0],[0,0,0]],reg)

# world logical volume
wl   = pyg4ometry.geant4.LogicalVolume(ws_1,"G4_Galactic","wl",reg)

# logical volumes
box_l = pyg4ometry.geant4.LogicalVolume(box,"G4_Fe","box_l",reg)
box_p = pyg4ometry.geant4.PhysicalVolume([0,0,0],[0,0,0],box_l,"box_p",wl,reg)

pipe_l   = pyg4ometry.geant4.LogicalVolume(pipe,"G4_Fe","pipe_l",reg)
pipe_p = pyg4ometry.geant4.PhysicalVolume([0,0,0],[-55,0,0],pipe_l,"pipe_p",wl,reg)

inner_pipe_l   = pyg4ometry.geant4.LogicalVolume(inner_pipe,"G4_Galactic","inner_pipe_l",reg)
inner_pipe_p = pyg4ometry.geant4.PhysicalVolume([0,0,0],[-55,0,0],inner_pipe_l,"inner_pipe_p",wl,reg)

reg.setWorld(wl.name)

w = pyg4ometry.gdml.Writer()
w.addDetector(reg)
w.write('Example/gdml/box.gdml')