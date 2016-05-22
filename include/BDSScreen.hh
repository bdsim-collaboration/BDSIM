#ifndef __BDS_SCREEN_HH_
#define __BDS_SCREEN_HH_

#include "globals.hh"
#include "BDSDrift.hh"
#include "G4TwoVector.hh"
#include <list>

class BDSMultilayerScreen;

class BDSBeamPipeInfo;

class BDSScreen : public BDSDrift{
public:
  ~BDSScreen();
  BDSScreen(G4String aName,  
	    G4double aLength, //length of the drift section containing the screen
	    BDSBeamPipeInfo* beamPipeInfo,
	    G4TwoVector size = G4TwoVector(), //X Y dimensions of screen
	    G4double screenAngle = 0 //Phi angle of screen
	    );
  //Use the below method to add the screen layers (to be done in the ComponentFactory).
  void screenLayer(G4double thickness, G4String material, G4int isSampler=0);

private:
  BDSMultilayerScreen* mlScreen;
  std::list<G4double> layerThicknesses;
  G4TwoVector size;
  G4double screenAngle;
  std::list<G4String> layerMaterials;
  
  virtual void Build();
  void PlaceScreen();
  G4ThreeVector screenPos;
  G4RotationMatrix* screenRot;
  G4int nLayers;

  //TODO  virtual void BuildFieldAndStepper(); 
};

#endif
