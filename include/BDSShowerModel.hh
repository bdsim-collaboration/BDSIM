#ifndef BDSSHOWERMODEL_H
#define BDSSHOWERMODEL_H

#include "GFlashShowerModel.hh"
#include "G4FastTrack.hh"

#include  <vector>

/**
 * @brief Revised implementation of GFlash with different fraction of shower considered.
 *
 * Not currently maintained.
 */

class BDSShowerModel: public GFlashShowerModel
{
public:
  BDSShowerModel(G4String, G4Region*);
  BDSShowerModel(G4String);
  virtual ~BDSShowerModel(){;}
private:
  G4bool CheckContainment(const G4FastTrack& fastTrack);
};
#endif

