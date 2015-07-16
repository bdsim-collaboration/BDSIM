#ifndef BDSTUNNELBUILDER_H
#define BDSTUNNELBUILDER_H

#include <utility>

class BDSBeamline;

/**
 * @brief A singleton class that constructs the necessary tunnel segments
 * to fit around a beamline. It uses a beam line instance - the flat beam line
 * and creates a new beam line of tunnel segments that can then be placed.
 * 
 * @author Laurie Nevay <Laurie.Nevay@rhul.ac.uk>
 */

class BDSTunnelBuilder
{
public:
  static BDSTunnelBuilder* Instance(); /// singleton accessor

  ~BDSTunnelBuilder();

  /// Main function of class - use tunnel factories and builds segments
  /// of tunnel around the flat beamline (of accelerator components).
  /// Tunnel segments are placed in order in a new beamline instance,
  /// which is returned. Additionally, build supports for magnets at the
  /// same time and put them in their own beamline.
  std::pair<BDSBeamline*,BDSBeamline*> BuildTunnelAndSupports(BDSBeamline* flatBeamLine);

private:
  /// Private default constructor as singleton
  BDSTunnelBuilder();

  /// Decide whether this is a good point to break the tunnel based on various cumulative
  /// parameters.
  G4bool BreakTunnel(G4double cumulativeLength,
		     G4double cumulativeAngle,
		     G4int    cumulativeNItems);

  static BDSTunnelBuilder* _instance;
};


#endif
