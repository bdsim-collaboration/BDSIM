#ifndef BDSTerminator_h
#define BDSTerminator_h

#include "BDSAcceleratorComponent.hh"

/** 
 * @brief Class for small control volume with dynamic user limits
 * that change based on the global constants parameter nturns. Used
 * to control the nunmber of tunrs a primary particle completes in 
 * the machine.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSTerminator: public BDSAcceleratorComponent
{
public:
  BDSTerminator(G4String aName, G4double aLength);
  ~BDSTerminator();
private:
  /// Override this function in BDSACceleratorComponent purely to avoid
  /// BDSAcceleratorComponent replacing the user limits on the terminator.
  virtual void Build();

  /// Construct a simple box and attach an instance of the customised
  /// BDSTerminatorUserLimits() to it.
  virtual void BuildContainerLogicalVolume();
};

#endif
