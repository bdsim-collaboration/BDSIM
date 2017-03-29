#include <iostream>
#include <ostream>
#include <iomanip>

#include "BDSMessenger.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSAcceleratorModel.hh"
#include "BDSSamplerRegistry.hh"

#include "G4UImanager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAString.hh"

#include "globals.hh"

BDSMessenger::BDSMessenger()
{
  bdsDirectory = new G4UIdirectory("/bds/");
  bdsDirectory->SetGuidance("BDSIM commands");

  bdsBeamlineDirectory = new G4UIdirectory("/bds/beamline/");

  beamlineListCmd = new G4UIcmdWithoutParameter("/bds/beamline/list",this);
  beamlineListCmd->SetGuidance("List beamline components");

  elementNameSearchCmd = new G4UIcmdWithAString("/bds/beamline/namesearch",this);
  beamlineListCmd->SetGuidance("Search beamline components for element");

  bdsSamplersDirectory = new G4UIdirectory("/bds/samplers/");
  samplerListCmd = new G4UIcmdWithoutParameter("/bds/samplers/list",this);
  samplerListCmd->SetGuidance("List samplers");

  // G4UImanager* UIManager = G4UImanager::GetUIpointer();
  // UIManager->ApplyCommand("/control/execute " + visMacroFilename);
}

BDSMessenger::~BDSMessenger()
{
  delete bdsDirectory;
  delete bdsBeamlineDirectory;
  delete beamlineListCmd;
  delete elementNameSearchCmd;
  delete bdsSamplersDirectory;
  delete samplerListCmd;
}

void BDSMessenger::SetNewValue(G4UIcommand *command, G4String newValue)
{
  if(     command == beamlineListCmd)       {this->BeamLineList();}
  else if(command == samplerListCmd)        {this->SamplerList();}
  else if(command == elementNameSearchCmd)  {this->ElementNameSearch(newValue);}
}

void BDSMessenger::BeamLineList() {
  BDSBeamline *beamline = BDSAcceleratorModel::Instance()->GetFlatBeamline();

  int j = 0;
  G4cout << std::right
         << std::setw(4)  << " index" << std::setfill(' ')
         << std::setw(20) << " name"
         << std::setw(20) << " placement name"
         << std::setw(20) << " type"
         << std::setw(20) << " S-middle" << G4endl;
  for (auto i = beamline->begin(); i != beamline->end(); ++i, ++j) {
    G4cout << this->BDSBeamlineElementToString(j) << G4endl;
  }
}

std::string BDSMessenger::BDSBeamlineElementToString(G4int iElement) {
  std::stringstream ss;

  BDSBeamline *beamline = BDSAcceleratorModel::Instance()->GetFlatBeamline();
  const BDSBeamlineElement *e = beamline->at(iElement);

  ss << std::setfill('0') << std::right << std::setw(4)  << iElement << " " << std::setfill(' ')
     << std::setw(20) << e->GetName() << " "
     << std::setw(20) << e->GetPlacementName() << " "
     << std::setw(20) << e->GetType() << " "
     << std::setw(20) << std::setprecision(4) << std::fixed << e->GetSPositionMiddle();

  return ss.str();

}

void BDSMessenger::ElementNameSearch(std::string name)
{
  BDSBeamline *beamline = BDSAcceleratorModel::Instance()->GetFlatBeamline();
  int j=0;
  for (auto i = beamline->begin(); i != beamline->end(); ++i, ++j)
  {
    if((*i)->GetName().contains(name))
	  {
	    G4cout << (*i)->GetName() << G4endl;
	  }
  }
}

void BDSMessenger::ElementTypeSearch(std::string /*type*/)
{

}

void BDSMessenger::SamplerList()
{
  int j=0;
  for (const auto name : BDSSamplerRegistry::Instance()->GetUniqueNames())
  {
    G4cout << this->BDSSamplerToString(j) << G4endl;
    ++j;
  }
}

std::string BDSMessenger::BDSSamplerToString(std::string /*samplerName*/)
{
  return std::string();
}

std::string BDSMessenger::BDSSamplerToString(int iSampler) {
  std::stringstream ss;

  BDSSamplerInfo sInfo = BDSSamplerRegistry::Instance()->GetInfo(iSampler);

  ss << std::setfill('0') << std::right << std::setw(4) << iSampler << " " << std::setfill(' ')
     << std::right << std::setw(20) << sInfo.Name() << " "
     << std::right << std::setw(20) << sInfo.SPosition();
  return ss.str();
}
