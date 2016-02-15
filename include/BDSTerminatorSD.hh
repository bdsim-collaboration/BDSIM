#ifndef BDSTERMINATORSD_H
#define BDSTERMINATORSD_H

#include "G4VSensitiveDetector.hh"

/**
 * @brief Sensitivity that measures primary particle turns for terminator.
 *
 * @author Laurie Nevay
 */

class BDSTerminatorSD: public G4VSensitiveDetector
{
public:
  BDSTerminatorSD(G4String name);
  ~BDSTerminatorSD();

  virtual void   Initialize (G4HCofThisEvent* HCE);
  virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist);
  virtual void   EndOfEvent (G4HCofThisEvent* HCE);

private:
  G4bool verbose;
};

#endif
