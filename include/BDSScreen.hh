#ifndef BDSSCREEN_H
#define BDSSCREEN_H

#include "BDSDrift.hh"

#include "globals.hh"
#include "G4TwoVector.hh"

#include <list>

class BDSBeamPipeInfo;
class BDSMultilayerScreen;

/**
 * @brief A multilayer screen in a beam pipe.
 *
 * @author Lawrence Deacon
 */

class BDSScreen: public BDSDrift
{
public:
  BDSScreen(G4String         name,  
	    G4double         chordength,  // Length of the drift section containing the screen
	    BDSBeamPipeInfo* beamPipeInfo,// Beam pipe specifications.
	    G4TwoVector      size = G4TwoVector(), // X Y dimensions of screen
	    G4double         screenAngle = 0);     //Phi angle of screen

  virtual ~BDSScreen();

  /// Add a screen layer.
  void AddScreenLayer(G4double thickness, G4String material, G4int isSampler=0);

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
};

#endif
