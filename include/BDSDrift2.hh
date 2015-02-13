#ifndef BDSDrift2_h
#define BDSDrift2_h 1

#include "globals.hh"
#include "BDSMultipole.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamPipeType.hh"

#include "G4Material.hh"

class BDSDrift2 :public BDSAcceleratorComponent
{
public:
  BDSDrift2(G4String        nameIn, 
	    G4double        lengthIn,
	    BDSBeamPipeType beamPipeTypeIn,
	    G4double        beamPipeThicknessIn = 0,
	    G4double        aper1In = 0,
	    G4double        aper2In = 0,
	    G4double        aper3In = 0,
	    G4double        aper4In = 0,
	    G4Material*     beamPipeMaterialIn = NULL,
	    G4Material*     vacuumMaterialIn = NULL
	    );
  ~BDSDrift2(){;};

protected:
  void Build();

private:
  void BuildMarkerLogicalVolume(){;}; // to fulfill bdsacceleratorcomponent requirements
  
  BDSBeamPipeType beamPipeType;
  G4double aper1;
  G4double aper2;
  G4double aper3;
  G4double aper4;
  G4double beamPipeThickness;
  G4Material* beamPipeMaterial;
  G4Material* vacuumMaterial;


};

#endif
