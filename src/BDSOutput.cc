#include "BDSEventInfo.hh"
#include "BDSGlobalConstants.hh"
#include "BDSOutput.hh"

#include "globals.hh"
#include "G4PrimaryVertex.hh"

#include <ostream>
#include <set>

const std::set<G4String> BDSOutput::protectedNames = {
  "Event", "Histos", "Info", "Primary", "Eloss",
  "PrimaryFirstHit", "PrimaryLastHit", "TunnelHit",
  "Trajectory"
};

BDSOutput::BDSOutput(G4String fileName,
		     G4int    fileNumberOffset):
  filename(fileName),
  eventNumber(0),
  outputFileNumber(fileNumberOffset)
{
  const BDSGlobalConstants* g = BDSGlobalConstants::Instance();
  numberEventPerFile = g->NumberOfEventsPerNtuple();
  writePrimaries     = g->WritePrimaries();
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

void BDSOutput::FillPrimary(const G4PrimaryVertex* vertex,
			    const G4int            eventNumber,
			    const G4int            turnsTaken)
{
  const G4PrimaryParticle* primaryParticle = vertex->GetPrimary();
  G4ThreeVector momDir  = primaryParticle->GetMomentumDirection();
  G4double      E       = primaryParticle->GetTotalEnergy();
  G4double      xp      = momDir.x();
  G4double      yp      = momDir.y();
  G4double      zp      = momDir.z();
  G4double      x0      = vertex->GetX0();
  G4double      y0      = vertex->GetY0();
  G4double      z0      = vertex->GetZ0();
  G4double      t       = vertex->GetT0();
  G4double      weight  = primaryParticle->GetWeight();
  G4int         PDGType = primaryParticle->GetPDGcode();

  FillPrimary(E, x0, y0, z0, xp, yp, zp, t, weight, PDGType, eventNumber, turnsTaken);
}

void BDSOutput::FillEvent(const BDSEventInfo*                   info,
			  const G4PrimaryVertex*                vertex,
			  const BDSSamplerHitsCollection*       samplerHitsPlane,
			  const BDSSamplerHitsCollection*       samplerHitsCylinder,
			  const BDSEnergyCounterHitsCollection* energyLoss,
			  const BDSEnergyCounterHitsCollection* tunnelLoss,
			  const BDSTrajectoryPoint*             primaryHit,
			  const BDSTrajectoryPoint*             primaryLoss,
			  const std::vector<BDSTrajectory*>&    trajectories,
			  const G4int                           turnsTaken)
{
  if (info)
    { FillEventInfo(info);}
  if (vertex)
    {FillPrimary(vertex, info->GetInfo()->index, turnsTaken);}
  if (samplerHitsPlane)
    { FillSamplerHits(samplerHitsPlane, BDSOutput::HitsType::plane);}
  if (samplerHitsCylinder)
    { FillSamplerHits(samplerHitsPlane, BDSOutput::HitsType::cylinder);}
  if (energyLoss)
    { FillEnergyLoss(energyLoss, BDSOutput::LossType::energy);}
  if (tunnelLoss)
    { FillEnergyLoss(tunnelLoss, BDSOutput::LossType::tunnel);}
  if (primaryHit)
    { FillPrimaryHit(primaryHit);}
  if (primaryLoss)
    { FillPrimaryLoss(primaryLoss);}
    FillTrajectories(trajectories);
    WriteFileEventLevel();
  ClearStructuresEventLevel();
}

void BDSOutput::CloseAndOpenNewFile()
{
  CloseFile();
  NewFile();
  InitialiseGeometryDependent();
}

void BDSOutput::FillRun(const BDSEventInfo* info)
{
  FillRunInfo(info);
  WriteFileRunLevel();
  ClearStructuresRunLevel();
}


