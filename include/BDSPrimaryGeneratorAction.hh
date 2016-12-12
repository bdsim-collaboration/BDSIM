#ifndef BDSPRIMARYGENERATORACTION_H
#define BDSPRIMARYGENERATORACTION_H 

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class BDSBunch;
class BDSOutputLoader;
class G4Event;
class G4ParticleGun;

/**
 * @brief Generates primary particle vertices using BDSBunch.
 *
 */

class BDSPrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction
{
public:
  BDSPrimaryGeneratorAction(BDSBunch*);    
  virtual ~BDSPrimaryGeneratorAction();
  
  virtual void GeneratePrimaries(G4Event*);
  
private:
  /// Pointer a to G4 service class.
  G4ParticleGun*   particleGun;	  
  /// Weight of the primaries (currently always 1 and not possible to change it).
  G4double         weight;
  /// Pointer to the particle distribution generator.
  BDSBunch*        bdsBunch;

  /// Cache of whether to write seed state as ASCII per event.
  G4bool writeASCIISeedState;

  /// Optional output handler for restoring seed state.
  BDSOutputLoader* recreateFile;

  /// Whether to load seed state at start of event from rootevent file.
  G4bool recreate;

  /// The offset in the file to read events from when setting the seed.
  G4int  eventOffset;

  /// Whether to use the ascii seed state each time.
  G4bool useASCIISeedState;
};

#endif


