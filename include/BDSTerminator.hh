#ifndef BDSTERMINATOR_H
#define BDSTERMINATOR_H

#include "BDSAcceleratorComponent.hh"

class BDSTerminatorUserLimits;

/** 
 * @brief Class for small control volume for circular macines.
 * 
 * Controls particles with dynamic user limits
 * that change based on the global constants parameter nturns. Used
 * to control the nunmber of tunrs a primary particle completes in 
 * the machine.
 * 
 * @author Laurie Nevay
 */

class BDSTerminator: public BDSAcceleratorComponent
{
public:
  BDSTerminator(G4String aName, G4double aLength);
  virtual ~BDSTerminator();
  
private:
  /// Override this function in BDSACceleratorComponent purely to avoid
  /// BDSAcceleratorComponent replacing the user limits on the terminator.
  virtual void Build();

  /// Construct a simple box and attach an instance of the customised
  /// BDSTerminatorUserLimits() to it.
  virtual void BuildContainerLogicalVolume();

  BDSTerminatorUserLimits* userLimits;
};

#endif
