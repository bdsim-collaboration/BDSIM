#include "BDSGlobalConstants.hh"
#include "BDSOutput.hh"

#include <ostream>
#include <set>

const std::set<G4String> BDSOutput::protectedNames = {
  "Event", "Histos", "Info", "Primary", "Eloss",
  "PrimaryFirstHit", "PrimaryLastHit", "TunnelHit",
  "Trajectory"
};

BDSOutput::BDSOutput():eventNumber(0),outputFileNumber(-1)
{
  numberEventPerFile = BDSGlobalConstants::Instance()->NumberOfEventsPerNtuple();
}

void BDSOutput::Commit(const time_t&  startTime,
                           const time_t&  stopTime,
                           const G4float& duration,
                           const std::string& seedStateAtStart)
{
  Write(startTime, stopTime, duration, seedStateAtStart);
  Close();
  Initialise();
}

bool BDSOutput::InvalidSamplerName(const G4String& samplerName)
{
  return protectedNames.find(samplerName) != protectedNames.end();
}

void BDSOutput::PrintProtectedNames(std::ostream& out)
{
  out << "Protected names for output " << G4endl;
  for (auto key : protectedNames)
    {out << "\"" << key << "\"" << G4endl; }
}
