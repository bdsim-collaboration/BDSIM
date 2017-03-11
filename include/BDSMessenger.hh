//
// Created by Stewart Boogert on 08/03/2017.
//

#ifndef BDSIM_BDSMESSENGER_H
#define BDSIM_BDSMESSENGER_H

class BDSBeamlineElement;

#include <string>

#include "G4UImanager.hh"
#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAString.hh"

class BDSMessenger : public G4UImessenger {
public:
  BDSMessenger();
  ~BDSMessenger();
  void SetNewValue(G4UIcommand *command, G4String newValue);
  void BeamLineList();
  std::string BDSBeamlineElementToString(G4int iElement);
  void ElementNameSearch(std::string name);
  void ElementTypeSearch(std::string type);
  void SamplerList();
  std::string BDSSamplerToString(G4int iSampler);
  std::string BDSSamplerToString(std::string samplerName);

protected:
  G4UImanager             *UIManager;

  G4UIdirectory           *bdsDirectory;

  G4UIdirectory           *bdsBeamlineDirectory;
  G4UIcmdWithoutParameter *beamlineListCmd;
  G4UIcmdWithAString      *elementNameSearchCmd;
  G4UIcmdWithAString      *elementTypeSearchCmd;
  G4UIcmdWithAString      *elementInfoCmd;

  G4UIdirectory           *bdsSamplersDirectory;
  G4UIcmdWithoutParameter *samplerListCmd;

  G4UIdirectory           *bdsVisualDirectory;
  G4UIcmdWithAString      *axisVisualCmd;
  G4UIcmdWithAString      *elementTargetVisualCmd;
  G4UIcmdWithAString      *elementTransparencyVisualCmd;
};


#endif //BDSIM_BDSMESSENGER_H
