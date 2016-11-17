#ifndef BDSBEAMPIPEFACTORYRACETRACK_H
#define BDSBEAMPIPEFACTORYRACETRACK_H

#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipeFactoryPoints.hh"
#include "BDSBeamPipe.hh"

class G4Material;

/**
 * @brief Factory for racetrack aperture model beampipes.
 * 
 * This is a rectangular aperture with curved edges. In the 
 * positive quadrante (+ve x & y) the user specifies (x,y) as
 * well as radius r that forms a quarter circle from this point.
 * This is reflected to the other quadrants. To achieve this using
 * Geant4 solids, a vector of points is built up transversely in 
 * clockwise order to form a solid G4Polyhedra. To create the beam
 * pipe, two are created and subtracted. The number of points in the
 * curved sections can also be chosen but increasing the detail will
 * also reduce the efficiency of course.
 *
 * aper1,2 and 3 are used to represent x,y and r respectively. aper4 is ignored.
 * 
 * Singleton pattern.
 * 
 * @author Laurie Nevay
 */

class BDSBeamPipeFactoryRaceTrack: public BDSBeamPipeFactoryPoints
{
public:
  static BDSBeamPipeFactoryRaceTrack* Instance(); ///< singleton accessor
  
  virtual ~BDSBeamPipeFactoryRaceTrack();
  
private:
  BDSBeamPipeFactoryRaceTrack(); ///< private default constructor - singleton pattern
  static BDSBeamPipeFactoryRaceTrack* instance;

  /// Generate quarters of a circle to represent the edges joined by straight lines.
  /// Overloaded (required) from BDSBeamPipeFactoryPoints
  virtual void GeneratePoints(G4double aper1,
			      G4double aper2,
			      G4double aper3,
			      G4double aper4,
			      G4double beamPipeThickness,
			      G4int    pointsPerTwoPi = 40);

  /// Calculate the radius of the solid used for intersection for angled faces.
  virtual G4double CalculateIntersectionRadius(G4double aper1,
					       G4double aper2,
					       G4double aper3,
					       G4double aper4,
					       G4double beamPipeThickness);

  void GenerateRaceTrack(std::vector<G4TwoVector>& vec,
			 G4double x,
			 G4double y,
			 G4double r,
			 G4int    pointsPerTowPi);
};
  
#endif
