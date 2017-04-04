#ifndef BDSMESSENGER_H
#define BDSMESSENGER_H

#include <string>

#include "G4UImessenger.hh"

class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;
class G4UIcommand;
class G4UIdirectory;
//class G4UImanager;

/**
 * @brief User interface commands.
 *
 * @author Stewart Boogert
 */

class BDSMessenger: public G4UImessenger
{
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
  //G4UImanager             *UIManager;

  G4UIdirectory           *bdsDirectory;

  G4UIdirectory           *bdsBeamlineDirectory;
  G4UIcmdWithoutParameter *beamlineListCmd;
  G4UIcmdWithAString      *elementNameSearchCmd;
  //G4UIcmdWithAString      *elementTypeSearchCmd;
  //G4UIcmdWithAString      *elementInfoCmd;

  G4UIdirectory           *bdsSamplersDirectory;
  G4UIcmdWithoutParameter *samplerListCmd;

  //G4UIdirectory           *bdsVisualDirectory;
  //G4UIcmdWithAString      *axisVisualCmd;
  //G4UIcmdWithAString      *elementTargetVisualCmd;
  //G4UIcmdWithAString      *elementTransparencyVisualCmd;
};


#endif //BDSIM_BDSMESSENGER_H
