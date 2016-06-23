#include <iostream>

#include "BDSOutputROOTEventTrajectory.hh"

#if 0
  0  fNotDefined,
  1  fTransportation,
    91 TRANSPORTATION,
    92 COUPLED_TRANSPORTATION
  // follwoing processes belong to 'General' type
  2  fElectromagnetic,
    1  fCoulombScattering, 
    2  fIonisation, 
    3  fBremsstrahlung, 
    4  fPairProdByCharged,
    5  fAnnihilation, 
    6  fAnnihilationToMuMu,
    7  fAnnihilationToHadrons,
    8  fNuclearStopping,
    10 fMultipleScattering,   
    11 fRayleigh,
    12 fPhotoElectricEffect,
    13 fComptonScattering,
    14 fGammaConversion,
    15 fGammaConversionToMuMu,
    21 fCerenkov,
    22 fScintillation,
    23 fSynchrotronRadiation,
    24 fTransitionRadiation 
  3  fOptical,             
    0 kCerenkov,      
    1 kScintillation, 
    2 kAbsorption,    
    3 kRayleigh,      
    4 kMieHG,         
    5 kBoundary,      
    6 kWLS,           
    7 kNoProcess      
  4  fHadronic,
    111 fHadronElastic,
    121 fHadronInelastic,
    131 fCapture,
    141 fFission,
    151 fHadronAtRest,
    152 fLeptonAtRest,
    161 fChargeExchange,
    210 fRadioactiveDecay
  5  fPhotolepton_hadron,
  6  fDecay,
    201 DECAY = 201,
    202 DECAY_WithSpin,
    203 DECAY_PionMakeSpin,
    210 DECAY_Radioactive,
    211 DECAY_Unknown,
    231 DECAY_External = 231
  7  fGeneral,
    401 STEP_LIMITER,
    402 USER_SPECIAL_CUTS,
    403 NEUTRON_KILLER
  8  fParameterisation,
  9  fUserDefined,
  10 fParallel,
    491 SBLN_UNKNOWN! 
  11 fPhonon,
  12 fUCN
#endif

ClassImp(BDSOutputROOTEventTrajectory)

BDSOutputROOTEventTrajectory::BDSOutputROOTEventTrajectory()
{;}

BDSOutputROOTEventTrajectory::~BDSOutputROOTEventTrajectory()
{;}

#ifndef __ROOTBUILD__
void BDSOutputROOTEventTrajectory::Fill(std::vector<BDSTrajectory*> &trajVec)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " " << trajVec.size() << G4endl;
#endif

  n = trajVec.size();
  for(auto iT = trajVec.begin(); iT != trajVec.end(); ++iT)
  {
    BDSTrajectory* traj = *iT;
    trackID.push_back( (unsigned int &&) traj->GetTrackID() );
    parentID.push_back((unsigned int &&) traj->GetParentID());
#ifdef BDSDEBUG
//    G4cout << __METHOD_NAME__ << traj->GetTrackID() << " " << traj->GetParentID() << G4endl;
#endif

    std::vector<TVector3> trajectory;
    std::vector<int> preProcessType;
    std::vector<int> preProcessSubType;
    std::vector<int> postProcessType;
    std::vector<int> postProcessSubType;
    std::vector<double> preWeight;
    std::vector<double> postWeight;
    std::vector<double> energy;

    for(auto i = 0; i< traj->GetPointEntries();++i)
    {
      BDSTrajectoryPoint *point = dynamic_cast<BDSTrajectoryPoint*>(traj->GetPoint(i));
      G4ThreeVector      pos = point->GetPosition();
      trajectory.push_back(TVector3(pos.getX() / CLHEP::m,
                                    pos.getY() / CLHEP::m,
                                    pos.getZ() / CLHEP::m));
      preProcessType.push_back(point->GetPreProcessType());
      preProcessSubType.push_back(point->GetPreProcessSubType());
      postProcessType.push_back(point->GetPostProcessType());
      postProcessSubType.push_back(point->GetPostProcessSubType());

      preWeight.push_back(point->GetPreWeight());
      postWeight.push_back(point->GetPostWeight());
      energy.push_back(point->GetEnergy());
    }
    trajectories.push_back(trajectory);
    preProcessTypes.push_back(preProcessType);
    preProcessSubTypes.push_back(preProcessSubType);
    postProcessTypes.push_back(postProcessType);
    postProcessSubTypes.push_back(postProcessSubType);
    preWeights.push_back(preWeight);
    postWeights.push_back(postWeight);
    energys.push_back(energy);
  }
}

void BDSOutputROOTEventTrajectory::Fill(BDSEnergyCounterHitsCollection *phc)
{
  G4cout << phc->GetSize() << G4endl;
}


#endif

void BDSOutputROOTEventTrajectory::Flush()
{
  trackID.clear();
  parentID.clear();
  trajectories.clear();
  preProcessTypes.clear();
  preProcessSubTypes.clear();
  postProcessTypes.clear();
  postProcessSubTypes.clear();
  preWeights.clear();
  postWeights.clear();
  energys.clear();
}

int BDSOutputROOTEventTrajectory::primary()
{
  for(size_t j=0;j<parentID.size();++j) {
    if(parentID[j] == 0) {
      return j;
    }
  }
  return -1;
}

int BDSOutputROOTEventTrajectory::primaryElectromagnetic()
{
  // loop over primary and find first electromagnetic
  int i = 0;
  for(size_t j=0;j<trajectories[i].size();++j) {
    if(postProcessTypes[i][j] == 2) {
      return postProcessSubTypes[i][j];
    }
  }
  return -1;
}

int BDSOutputROOTEventTrajectory::primaryHadronic()
{
  // loop over primary and find fist hadronic 
  int i = 0;
  for(size_t j=0;j<trajectories[i].size();++j) {
    if(postProcessTypes[i][j] == 4) {
      return postProcessSubTypes[i][j];
    }
  }
  return -1;
}


void BDSOutputROOTEventTrajectory::primaryPrint()
{
  int i = 0;
  for(size_t j=0;j<trajectories[i].size();++j) 
  {
      std::cout << trackID[i] << " " << parentID[i] << " " << postProcessTypes[i][j] << " " << postProcessSubTypes[i][j] << std::endl;
  }
}

std::ostream& operator<< (std::ostream& out, BDSOutputROOTEventTrajectory const &t)
{
  for(int i=0; i< (int)t.preProcessTypes.size();++i)
  {
    for(int j=0; j< (int)t.preProcessTypes[i].size(); ++j)
    {
      //if(t.preProcessTypes[i][j] != 1 && t.preProcessTypes[i][j] != 7)
      //{
        out << i << " " << j
            << " " << t.preProcessTypes[i][j]   << " " << t.preProcessSubTypes[i][j]
            << " " << t.postProcessTypes[i][j]  << " " << t.postProcessSubTypes[i][j]
            << " " << t.preWeights[i][j]        << " " << t.postWeights[i][j]
            << " " << t.energys[i][j]      << std::endl;
      //}
    }
  }
  return out;
}
