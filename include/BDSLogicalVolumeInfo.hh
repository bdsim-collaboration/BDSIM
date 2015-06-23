#ifndef BDSLOGICALVOLUMEINFO_H
#define BDSLOGICALVOLUMEINFO_H

#include "G4String.hh"
#include "globals.hh" // geant4 types / globals

/**
 * @brief A class holding any information pertaining to a particular logical
 * volume in a BDSIM geant4 model.  Currently, this is used for the name and
 * s position coordinate, but can be extended to store any other pertinent 
 * information.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSLogicalVolumeInfo
{
public:
  BDSLogicalVolumeInfo(G4double SPosAtMiddleOfElement);
  BDSLogicalVolumeInfo(G4String NameIn, G4double SPosIn);
  ~BDSLogicalVolumeInfo();

  /// Get the name of the logical volume
  G4String GetName()
  {return name;}

  /// Set the name of the logical volume
  void SetName(G4String NameIn)
  {name = NameIn;}

  /// Get the s position coordinate of the logical volume
  G4double GetSPos()
  {return spos;}

  /// Set the s position coordinate of the logical volume
  void SetSPos(G4double SPosIn)
  {spos = SPosIn;}

private:
  BDSLogicalVolumeInfo();

  G4String name;
  G4double spos;

};


#endif
