import pyg4ometry.geant4 as _g4
import pyg4ometry.vtk as _vtk
import pyg4ometry.gdml as _gdml

def GenerateSensitive1():
    # create with box, then two concurrent spheres in line of beam
    # to practice hiearchy for sensitivity
    # let's call this one 'sa'
    saContainerSolid = _g4.solid.Box("saContainerSolid",
                                     200,
                                     200,
                                     200)
    saContainerLV = _g4.LogicalVolume(saContainerSolid, 'G4_Be', 'saContainerLV')

    saOuterSolid = _g4.solid.Orb('saOuterSolid', 100)
    saInnerSolid = _g4.solid.Orb('saInnerSolid',  50)

    saOuterLV = _g4.LogicalVolume(saOuterSolid, 'G4_C',  'saOuterLV')
    saInnerLV = _g4.LogicalVolume(saInnerSolid, 'G4_Ca', 'saInnerLV')

    saOuterPV = _g4.PhysicalVolume([0,0,0],[0,0,0],saOuterLV,
                                   'saOuterPV', saContainerLV)
    saInnerPV = _g4.PhysicalVolume([0,0,0],[0,0,0],saInnerLV,
                                   'saInnerPV', saOuterLV)

    _g4.registry.setWorld('saContainerLV')

    m = saContainerLV.pycsgmesh()
    v = _vtk.Viewer()
    v.addPycsgMeshList(m)
    v.view();
    w = _gdml.Writer()
    w.addDetector(_g4.registry)
    w.write('sensitive1.gdml')


def GenerateSensitive2():
    # create with box, then two concurrent spheres in line of beam
    # to practice hiearchy for sensitivity
    # let's call this one 'sb'
    sbContainerSolid = _g4.solid.Box("sbContainerSolid",
                                    200,
                                    200,
                                    200)
    sbContainerLV = _g4.LogicalVolume(sbContainerSolid, 'G4_Be',
                                      'sbContainerLV')

    sbOuterSolid = _g4.solid.Box('sbOuterSolid', 100,100,100)
    sbInnerSolid = _g4.solid.Box('sbInnerSolid',  50, 50, 50)
    sbCordSolid  = _g4.solid.Box('sbCordSolid',   30, 30, 30)

    sbOuterLV = _g4.LogicalVolume(sbOuterSolid, 'G4_C',  'sbOuterLV')
    sbInnerLV = _g4.LogicalVolume(sbInnerSolid, 'G4_Ca', 'sbInnerLV')
    sbCordLV  = _g4.LogicalVolume(sbCordSolid,  'G4_Fe', 'sbCoreLV')

    sbOuterPV = _g4.PhysicalVolume([0,0,0],[0,0,0],sbOuterLV,
                                   'sbOuterPV', sbContainerLV)
    sbInnerPV = _g4.PhysicalVolume([0,0,0],[0,0,0],sbInnerLV,
                                   'sbInnerPV', sbOuterLV)
    sbInnerPV = _g4.PhysicalVolume([0,0,0],[0,0,0],sbCordLV,
                                   'sbCorePV', sbInnerLV)

    _g4.registry.setWorld('sbContainerLV')

    m = sbContainerLV.pycsgmesh()
    v = _vtk.Viewer()
    v.addPycsgMeshList(m)
    v.view();
    w = _gdml.Writer()
    w.addDetector(_g4.registry)
    w.write('sensitive2.gdml')
