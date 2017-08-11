#ifndef BDSSAMPLERSD_H
#define BDSSAMPLERSD_H

#include "BDSSamplerHit.hh"

#include "globals.hh" // geant4 types / globals
#include "G4VSensitiveDetector.hh"

class BDSGlobalConstants;
class BDSSamplerRegistry;

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

/**
 * @brief The sensitive detector class that provides sensitivity to BDSSampler instances.
 *
 * It creates BDSSamplerHit instances for each particle impact on a sampler this SD is
 * attached to.
 * 
 * Written and edited by many authors over time.
 */

class BDSSamplerSD: public G4VSensitiveDetector
{ 
public:
  /// Construct a sampler with name and type (plane/cylinder).
  explicit BDSSamplerSD(G4String name);
  ~BDSSamplerSD();

  /// Overriden from G4VSensitiveDetector. Creates hits collection and registers it with
  /// the hits collection of this event (HCE).
  virtual void Initialize(G4HCofThisEvent* HCE);

  /// Overriden from G4VSensitiveDetector.  Creates hit instances and appends them to the
  /// hits collection.
  virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* readOutTH);

private:
  /// The hits collection for this sensitive detector class that's owned by each instance.
  BDSSamplerHitsCollection* SamplerCollection;

  /// The name of the hits collection that's created and registered.
  G4String itsCollectionName;

  /// Hits collection ID - an integer look up for the hits collection
  /// provided by G4SDManager (a registry) that is given to the
  /// G4HCofThisEvent (Hits collection of the event).
  int itsHCID;
  
  /// Cached pointer to registry as accessed many times
  BDSSamplerRegistry* registry;

  /// Cached pointer to global constants as accessed many times
  BDSGlobalConstants* globals;
};

#endif

