#ifndef __BDS_SCREEN_HH_
#define __BDS_SCREEN_HH_

#include "globals.hh"
#include "BDSDrift.hh"
#include "BDSMultilayerScreen.hh"
#include "BDSColourWheel.hh"
#include "G4TwoVector.hh"
#include <list>

class BDSScreen : public BDSDrift{
public:
  ~BDSScreen();
  BDSScreen(G4String aName,  
	    G4double aLength, //length of the drift section containing the screen
	    G4bool aperset = false, 
	    G4double aper = 0, //Aperture of beampipe around screen
	    G4String tunnelMaterial="",
	    G4double tunnelOffsetX = BDSGlobalConstants::Instance()->GetTunnelOffsetX(),
	    G4TwoVector size = G4TwoVector(), //X Y dimensions of screen
	    G4double screenAngle = 0 //Phi angle of screen
	    );
  //Use the below method to add the screen layers (to be done in the ComponentFactory).
  void screenLayer(G4double thickness, G4String material);

private:
  BDSMultilayerScreen* _mlScreen;
  std::list<G4double> _layerThicknesses;
  G4TwoVector _size;
  G4double _screenAngle;
  std::list<G4String> _layerMaterials;
  
  virtual void Build();
  void PlaceScreen();
  G4ThreeVector _screenPos;
  G4RotationMatrix* _screenRot;
  G4int _nLayers;

  virtual void BuildFieldAndStepper(); 
};

#endif
