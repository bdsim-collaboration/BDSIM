/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2022.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "BDSWrapperMuonSplitting.hh"

#include "G4ParticleDefinition.hh"
#include "G4Track.hh"
#include "G4Types.hh"
#include "G4VParticleChange.hh"
#include "G4VProcess.hh"

#include <cmath>
#include <set>
#include <vector>

G4int BDSWrapperMuonSplitting::nCallsThisEvent = 0;

BDSWrapperMuonSplitting::BDSWrapperMuonSplitting(G4VProcess* originalProcess,
                                                 G4int splittingFactorIn,
                                                 G4double splittingThresholdEKIn,
                                                 G4int splittingFactor2In,
                                                 G4double splittingThresholdEK2In):
  BDSWrapperProcess("MuonSplittingWrapper"),
  splittingFactor(splittingFactorIn),
  splittingThresholdEK(splittingThresholdEKIn),
  splittingFactor2(splittingFactor2In),
  splittingThresholdEK2(splittingThresholdEK2In)
{
  RegisterProcess(originalProcess);
  theProcessSubType = originalProcess->GetProcessSubType();
  theProcessName = "MuonSplittingWrapper("+originalProcess->GetProcessName()+")";
  if (splittingFactor2 < splittingFactor)
    {splittingFactor2 = splittingFactor;} // always want more high energy
  if (splittingThresholdEK2 < splittingThresholdEK)
    {splittingThresholdEK2 = splittingThresholdEK;}
}

BDSWrapperMuonSplitting::~BDSWrapperMuonSplitting()
{;}

G4VParticleChange* BDSWrapperMuonSplitting::PostStepDoIt(const G4Track& track,
							 const G4Step& step)
{
  G4VParticleChange* particleChange = pRegProcess->PostStepDoIt(track, step);
  
  if (splittingFactor == 1)
    {return particleChange;}
  
  G4double parentEk = track.GetKineticEnergy();
  if (parentEk < splittingThresholdEK) // smaller of the two thresholds by design
    {return particleChange;}
  
  G4int nSecondaries = particleChange->GetNumberOfSecondaries();
  if (nSecondaries == 0)
    {return particleChange;}
  
  G4bool muonPresent = false;
  std::vector<G4int> secondaryPDGIDs;
  std::vector<G4int> secondaryMuonIndices;
  for (G4int i = 0; i < nSecondaries; i++)
    {
      G4int secondaryPDGID = particleChange->GetSecondary(i)->GetDefinition()->GetPDGEncoding();
      if (std::abs(secondaryPDGID) == 13)
        {// it's a muon
          muonPresent = true;
          secondaryMuonIndices.push_back(i);
        }
    }
  if (!muonPresent)
    {return particleChange;}
  
  // we keep hold of the tracks and manage their memory
  std::vector<G4Track*> originalSecondaries;
  std::vector<G4Track*> originalMuons;
  for (G4int i = 0; i < particleChange->GetNumberOfSecondaries(); i++)
    {
      G4Track* secondary = particleChange->GetSecondary(i);
      if (std::abs(secondary->GetDefinition()->GetPDGEncoding()) != 13)
	{originalSecondaries.push_back(secondary);}
      else
	{originalMuons.push_back(secondary);}
    }
  
  G4int nOriginalSecondaries = particleChange->GetNumberOfSecondaries();
  
  particleChange->Clear(); // doesn't delete the secondaries
  
  // Attempt to generate more muons. This might be difficult or rare, so we must tolerate this.
  G4int thisTimeSplittingFactor = parentEk > splittingThresholdEK2 ? splittingFactor2 : splittingFactor;
  G4int maxTrials = 1000 * thisTimeSplittingFactor;
  G4int nSuccessfulMuonSplits = 0;
  G4int iTry = 0;
  std::vector<G4Track*> newMuons;
  while (iTry < maxTrials && nSuccessfulMuonSplits < thisTimeSplittingFactor-1)
    {
      iTry++;
      particleChange->Clear(); // wipes the vector of tracks, but doesn't delete them
      particleChange = pRegProcess->PostStepDoIt(track, step);
      G4bool success = false;
      for (G4int i = 0; i < particleChange->GetNumberOfSecondaries(); i++)
        {
          G4Track* sec = particleChange->GetSecondary(i);
          if (std::abs(sec->GetDefinition()->GetPDGEncoding()) == 13)
            {
              newMuons.push_back(sec);
              success = true;
            }
          else
            {delete sec;}
        }
      particleChange->Clear();
      if (success)
        {nSuccessfulMuonSplits++;}
    }
  
  particleChange->Clear();
  particleChange->SetNumberOfSecondaries(nOriginalSecondaries + (G4int)newMuons.size());
  particleChange->SetSecondaryWeightByProcess(true);
  if (nSuccessfulMuonSplits == 0)
    {// we've cleared the original ones, so we have to put them back
      for (auto secondary : originalSecondaries)
        {particleChange->AddSecondary(secondary);}
      for (auto muon : originalMuons)
        {particleChange->AddSecondary(muon);}
      return particleChange;
    }
    
  G4double weightFactor = 1.0 / ((G4double)nSuccessfulMuonSplits + 1);
  for (auto aSecondary : originalSecondaries)
    {particleChange->AddSecondary(aSecondary);}
  for (auto originalMuon : originalMuons)
    {
      G4double existingWeight = originalMuon->GetWeight();
      G4double newWeight = existingWeight * weightFactor;
      originalMuon->SetWeight(newWeight);
      particleChange->AddSecondary(originalMuon);
    }
  for (auto newMuon : newMuons)
    {
      G4double existingWeight = newMuon->GetWeight();
      G4double newWeight = existingWeight * weightFactor;
      newMuon->SetWeight(newWeight);
      particleChange->AddSecondary(newMuon);
      //G4cout << newMuon->GetKineticEnergy()/ CLHEP::GeV << G4endl;
    }
  nCallsThisEvent++;
  //G4cout << "Muon parent trackID / pdgID: " << track.GetTrackID() << " " << track.GetParticleDefinition()->GetPDGEncoding() << G4endl;
  return particleChange;
}