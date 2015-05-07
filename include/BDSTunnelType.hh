#ifndef BDSTUNNLTYPE_H
#define BDSTUNNLTYPE_H

#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types

/**
 * @brief Type definition for tunnels - used for comparison
 * in factory methods
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

struct tunneltypes_def {
  enum type { circular, elliptical, square, rectangular, ilc, rectangularaboveground};
};

typedef BDSTypeSafeEnum<tunneltypes_def,int> BDSTunnelType;

namespace BDS {
  /// function to determine the enum type of the tunnel (case-insensitive)
  BDSTunnelType DetermineTunnelType(G4String tunnelType);

  /*
  /// function to check relevant aperture values are set.  This is really a dispatch function
  /// for other aperture specific methods below
  void CheckTunnelInfo(BDSTunnelType tunnelTypeIn,
		       G4double     tunnelOffsetX,
		       G4double     tunnelOffsetY,
		       G4bool       tunnelFloor,
		       G4double     tunnelFloorOffset,
		       G4double     tunnel1,
		       G4double     tunnel2);

  /// aperture info check for circular tunnel
  void InfoOKForCircular(G4double tunnelOffsetX, G4double tunnelOffsetY, G4bool tunnelFloor, G4double tunnelFloorOffset,
			 G4double tunnel1, G4double tunnel2);

  /// aperture info check for elliptical tunnel
  void InfoOKForElliptical(G4double tunnelOffsetX, G4double tunnelOffsetY, G4bool tunnelFloor, G4double tunnelFloorOffset,
			   G4double tunnel1, G4double tunnel2);

  /// aperture info check for rectangular tunnel
  void InfoOKForRectangular(G4double tunnelOffsetX, G4double tunnelOffsetY, G4bool tunnelFloor, G4double tunnelFloorOffset,
			    G4double tunnel1, G4double tunnel2);

  /// aperture info check for rectangular tunnel
  void InfoOKForSquare(G4double tunnelOffsetX, G4double tunnelOffsetY, G4bool tunnelFloor, G4double tunnelFloorOffset,
		       G4double tunnel1, G4double tunnel2);
  */
}
#endif



