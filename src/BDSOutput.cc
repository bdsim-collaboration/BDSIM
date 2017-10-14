#include "BDSAcceleratorModel.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSDebug.hh"
#include "BDSEnergyCounterHit.hh"
#include "BDSEventInfo.hh"
#include "BDSGlobalConstants.hh"
#include "BDSOutput.hh"
#include "BDSOutputROOTEventBeam.hh"
#include "BDSOutputROOTEventHistograms.hh"
#include "BDSOutputROOTEventInfo.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventModel.hh"
#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventRunInfo.hh"
#include "BDSOutputROOTEventSampler.hh"
#include "BDSOutputROOTEventTrajectory.hh"
#include "BDSSamplerHit.hh"
#include "BDSTrajectoryPoint.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"

#include "parser/beamBase.h"
#include "parser/optionsBase.h"

#include <cmath>
#include <ostream>
#include <set>
#include <vector>

#include "CLHEP/Units/SystemOfUnits.h"

const std::set<G4String> BDSOutput::protectedNames = {
  "Event", "Histos", "Info", "Primary", "Eloss",
  "PrimaryFirstHit", "PrimaryLastHit", "TunnelHit",
  "Trajectory"
};

BDSOutput::BDSOutput(G4String baseFileNameIn,
		     G4String fileExtensionIn,
		     G4int    fileNumberOffset):
  BDSOutputStructures(BDSGlobalConstants::Instance()),
  baseFileName(baseFileNameIn),
  fileExtension(fileExtensionIn),
  outputFileNumber(fileNumberOffset),
  sMaxHistograms(0),
  nbins(0)
{
  const BDSGlobalConstants* g = BDSGlobalConstants::Instance();
  numberEventPerFile = g->NumberOfEventsPerNtuple();
  writePrimaries     = g->WritePrimaries();
  useScoringMap      = g->UseScoringMap();
}

void BDSOutput::InitialiseGeometryDependent()
{
  CreateHistograms();
  InitialiseSamplers();
}

void BDSOutput::FillBeam(const GMAD::BeamBase* beam)
{
  *beamOutput = BDSOutputROOTEventBeam(beam);
  WriteBeam();
  ClearStructuresBeam();
}

void BDSOutput::FillOptions(const GMAD::OptionsBase* options)
{
  *optionsOutput = BDSOutputROOTEventOptions(options);
  WriteOptions();
  ClearStructuresOptions();
}

void BDSOutput::FillModel()
{
  modelOutput->Fill();
  WriteModel();
  ClearStructuresModel();
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

void BDSOutput::FillEventPrimaryOnly(G4double E,
				     G4double x0,
				     G4double y0,
				     G4double z0,
				     G4double xp,
				     G4double yp,
				     G4double zp,
				     G4double t,
				     G4double weight,
				     G4int    PDGType,
				     G4int    eventNumber,
				     G4int    turnsTaken)
{
  FillPrimary(E, x0, y0, z0, xp, yp, zp, t, weight, PDGType, eventNumber, turnsTaken);
  WriteFileEventLevel();
  ClearStructuresEventLevel();
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
    {FillEventInfo(info);}
  if (vertex)
    {FillPrimary(vertex, info->GetInfo()->index, turnsTaken);}
  if (samplerHitsPlane)
    {FillSamplerHits(samplerHitsPlane, BDSOutput::HitsType::plane);}
  if (samplerHitsCylinder)
    {FillSamplerHits(samplerHitsCylinder, BDSOutput::HitsType::cylinder);}
  if (energyLoss)
    {FillEnergyLoss(energyLoss, BDSOutput::LossType::energy);}
  if (tunnelLoss)
    {FillEnergyLoss(tunnelLoss, BDSOutput::LossType::tunnel);}
  if (primaryHit)
    {FillPrimaryHit(primaryHit);}
  if (primaryLoss)
    {FillPrimaryLoss(primaryLoss);}
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

G4bool BDSOutput::InvalidSamplerName(const G4String& samplerName)
{
  return protectedNames.find(samplerName) != protectedNames.end();
}

void BDSOutput::PrintProtectedNames(std::ostream& out)
{
  out << "Protected names for output " << G4endl;
  for (auto key : protectedNames)
    {out << "\"" << key << "\"" << G4endl; }
}
 
G4String BDSOutput::GetNextFileName()
{
  outputFileNumber++;
  const BDSGlobalConstants* globalConstants = BDSGlobalConstants::Instance();
  
  // Base root file name 
  G4String newFileName = baseFileName;

  // if more than one file add number (starting at 0)
  // of numberEventPerFile is specified and the number already generated exceeds that
  if (numberEventPerFile > 0 && globalConstants->NGenerate() > numberEventPerFile)
    {newFileName += "_" + std::to_string(outputFileNumber);} // note underscore
  
  // policy: overwrite if output filename specifically set, otherwise increase number
  // always check in interactive mode
  if (!globalConstants->OutputFileNameSet() || !globalConstants->Batch())
    {// check if file exists
      G4String original = newFileName; // could have nper file number suffix too
      G4int nTimeAppended = 1;
      while (BDS::FileExists(newFileName + fileExtension)) // always test with extension
	{// if exists increment suffix integer
	  newFileName = original + "-" + std::to_string(nTimeAppended);
	  nTimeAppended +=1;
	}
    }

  // add extension now we've got the base part fixed
  newFileName += fileExtension;
  
  G4cout << __METHOD_NAME__ << "Setting up new file: " << newFileName << G4endl;

  return newFileName;
}

void BDSOutput::CalculateHistogramParameters()
{
  // rounding up so last bin definitely covers smax
  // (max - min) / bin width -> min = 0 here.
  const G4double binWidth = BDSGlobalConstants::Instance()->ElossHistoBinWidth();
  const BDSBeamline* flatBeamline = BDSAcceleratorModel::Instance()->BeamlineMain();
  if (flatBeamline && flatBeamline->empty()==false)
    {
      G4double sMax = flatBeamline->GetLastItem()->GetSPositionEnd();
      nbins = (int) std::ceil(sMax / binWidth); // round up to integer # of bins
    }
  else
    {nbins = 1;} // can happen for generate primaries only
  
  sMaxHistograms = nbins * binWidth;
}

void BDSOutput::CreateHistograms()
{
  // construct output histograms
  // calculate histogram dimensions
  CalculateHistogramParameters();
  const G4double smin   = 0.0;
  const G4double smax   = sMaxHistograms / CLHEP::m;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "histogram parameters calculated to be: " << G4endl;
  G4cout << "s minimum: " << smin     << " m" << G4endl;
  G4cout << "s maximum: " << smax     << " m" << G4endl;
  G4cout << "# of bins: " << nbins    << G4endl;
#endif
  // create the histograms
  Create1DHistogram("PhitsHisto","Primary Hits", nbins,smin,smax);
  Create1DHistogram("PlossHisto","Primary Loss", nbins,smin,smax);
  Create1DHistogram("ElossHisto","Energy Loss",  nbins,smin,smax);
  // prepare bin edges for a by-element histogram
  std::vector<G4double> binedges;
  const BDSBeamline* flatBeamline = BDSAcceleratorModel::Instance()->BeamlineMain();
  if (flatBeamline) // can be nullptr in case of generate primaries only
    {binedges = flatBeamline->GetEdgeSPositions();}
  else
    {binedges = {0,1};}
  // create per element ("pe") bin width histograms
  Create1DHistogram("PhitsPEHisto","Primary Hits per Element", binedges);
  Create1DHistogram("PlossPEHisto","Primary Loss per Element", binedges);
  Create1DHistogram("ElossPEHisto","Energy Loss per Element" , binedges);
  
  if (useScoringMap)
    {
      const BDSGlobalConstants* g = BDSGlobalConstants::Instance();
      evtHistos->Create3DHistogram("ScoringMap", "Energy Deposition",
				   g->NBinsX(), g->XMin()/CLHEP::m, g->XMax()/CLHEP::m,
				   g->NBinsY(), g->YMin()/CLHEP::m, g->YMax()/CLHEP::m,
				   g->NBinsZ(), g->ZMin()/CLHEP::m, g->ZMax()/CLHEP::m);
    }
}

void BDSOutput::FillPrimary(G4double E,
			    G4double x0,
			    G4double y0,
			    G4double z0,
			    G4double xp,
			    G4double yp,
			    G4double zp,
			    G4double t,
			    G4double weight,
			    G4int    PDGType,
			    G4int    nEvent,
			    G4int    turnsTaken)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  if (WritePrimaries())
    {primary->Fill(E,x0,y0,z0,xp,yp,zp,t,weight,PDGType,nEvent,turnsTaken,0 /* always first element */);}
}

void BDSOutput::FillEventInfo(const BDSEventInfo* info)
{
  if (info)
    {*evtInfo = *(info->GetInfo());}
}

void BDSOutput::FillSamplerHits(const BDSSamplerHitsCollection* hits,
				const BDSOutput::HitsType)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << __METHOD_NAME__ << hits->entries() << std::endl;
#endif
  // Here, we don't switch on the type of the hits as the samplers are all
  // prepared and stored in one vector in the sampler registry.  The output
  // structures are based on this and cylinder output is no different from
  // plane output and indices will match.

  // TBC - cylinder output will have all the same z and S, which is wrong!
  if (!(hits->entries() > 0))
    {return;}
  for (int i=0; i<hits->entries(); i++)
    {
      G4int samplerID = (*hits)[i]->GetSamplerID();
      if (WritePrimaries())
        {samplerID += 1;} // offset index by one
      samplerTrees[samplerID]->Fill((*hits)[i]);
    }
}

void BDSOutput::FillEnergyLoss(const BDSEnergyCounterHitsCollection* hits,
			       const LossType lType)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  G4int n_hit = hits->entries();
  for(G4int i=0;i<n_hit;i++)
  {
    BDSEnergyCounterHit* hit = (*hits)[i];
    switch (lType)
      {
      case BDSOutput::LossType::energy:
	{eLoss->Fill(hit); break;}
      case BDSOutput::LossType::tunnel:
	{tHit->Fill(hit); break;}
      default:
	{eLoss->Fill(hit); break;}
      }
    G4double sHit = hit->GetSHit()/CLHEP::m;
    G4double eW   = hit->GetEnergyWeighted()/CLHEP::GeV;
    runHistos->Fill1DHistogram(2, sHit, eW);
    evtHistos->Fill1DHistogram(2, sHit, eW);
    runHistos->Fill1DHistogram(5, sHit, eW);
    evtHistos->Fill1DHistogram(5, sHit, eW);
    if (useScoringMap)
    {
      G4double x = hit->Getx()/CLHEP::m;
      G4double y = hit->Gety()/CLHEP::m;
      evtHistos->Fill3DHistogram(0, x, y, sHit, eW);
    }
  }
}

void BDSOutput::FillPrimaryHit(const BDSTrajectoryPoint* phit)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  pFirstHit->Fill(phit);
  const G4double preStepSPosition = phit->GetPreS() / CLHEP::m;
  runHistos->Fill1DHistogram(0, preStepSPosition);
  evtHistos->Fill1DHistogram(0, preStepSPosition);
  runHistos->Fill1DHistogram(3, preStepSPosition);
  evtHistos->Fill1DHistogram(3, preStepSPosition);
}

void BDSOutput::FillPrimaryLoss(const BDSTrajectoryPoint* ploss)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ <<G4endl;
#endif
  pLastHit->Fill(ploss);
  const G4double postStepSPosition = ploss->GetPostS() / CLHEP::m;
  runHistos->Fill1DHistogram(1, postStepSPosition);
  evtHistos->Fill1DHistogram(1, postStepSPosition);
  runHistos->Fill1DHistogram(4, postStepSPosition);
  evtHistos->Fill1DHistogram(4, postStepSPosition);
}

void BDSOutput::FillTrajectories(const std::vector<BDSTrajectory*>& trajectories)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " ntrajectory=" << trajectories.size() << G4endl;
#endif
  traj->Fill(trajectories);
}

void BDSOutput::FillRunInfo(const BDSEventInfo* info)
{
  if (info)
    {*runInfo = BDSOutputROOTEventRunInfo(info->GetInfo());}
}
