class BDSAcceleratorComponent;
class BDSIntegratorSet;
class BDSLine;
class BDSMagnet;
class BDSMagnetStrength;

namespace GMAD
{
  struct Element;
}

namespace BDS
{
  /// This calculates and constructs a BDSLine* of BDSMagnet*. A single magnet will be constructed and placed
  /// multiple times.
  BDSAcceleratorComponent *BuildUndulator(const G4String &elementName,
                                            const GMAD::Element *element,
                                            BDSMagnetStrength *st,
                                            const G4double brho,
                                            const BDSIntegratorSet *integratorSet);

  /// Function to return a single sector bend section.
  BDSMagnet *BuildUndulatorMagnet(const GMAD::Element *element,
                                  const G4String name,
                                  const G4double arcLength,
                                  const BDSMagnetStrength *strength,
                                  const G4double brho,
                                  const BDSIntegratorSet *integratorSet,
                                  const G4bool yokeOnLeft);
}