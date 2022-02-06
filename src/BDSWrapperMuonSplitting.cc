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

#include <algorithm>
#include <cmath>
#include <vector>

BDSWrapperMuonSplitting::BDSWrapperMuonSplitting(G4VProcess* originalProcess,
                                                 G4int splittingFactorIn):
  BDSWrapperProcess("MuonSplittingWrapper"),
  splittingFactor(splittingFactorIn)
{
  RegisterProcess(originalProcess);
  theProcessSubType = originalProcess->GetProcessSubType();
  theProcessName = "MuonSplittingWrapper("+originalProcess->GetProcessName()+")";
}

BDSWrapperMuonSplitting::~BDSWrapperMuonSplitting()
{;}

G4VParticleChange* BDSWrapperMuonSplitting::PostStepDoIt(const G4Track& track,
							 const G4Step& step)
{
  G4VParticleChange* particleChange = pRegProcess->PostStepDoIt(track, step);
  
  if (splittingFactor == 1)
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
  
  // we have to copy the original non-muon secondaries out because the interface
  // in G4VParticleChange forces us to delete all existing secondaries when we resize
  // which is a bit annoying.
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
  
  // Attempt to generate more muons. This might be difficult or rare, so we must tolerate this.
  G4int maxTrials = 1000 * splittingFactor;
  G4int nSuccessfulMuonSplits = 0;
  G4int iTry = 0;
  std::vector<G4Track*> newMuons;
  std::set<G4Track*> potentiallyDelete;
  while (iTry < maxTrials && nSuccessfulMuonSplits < splittingFactor-1)
    {
      iTry++;
      particleChange->Clear(); // wipes the vector of tracks, but doesn't delete them
      particleChange->SetNumberOfSecondaries(0);
      particleChange = pRegProcess->PostStepDoIt(track, step);
      G4bool aMuon = MuonPresent(particleChange);
      if (!aMuon)
	{
	  for (G4int i = 0; i < particleChange->GetNumberOfSecondaries(); i++)
	    {
	      auto sec = particleChange->GetSecondary(i);
	      potentiallyDelete.insert(sec);
	      
	    }
	  //DeleteSecondaries(particleChange); // causes double deletion bug only with certain processes...
	  // becuase of the allocator they use inside G4Track and G4DynamicParticle
	  continue;
	}
      
      nSuccessfulMuonSplits++; // we found at least one muon this call
      for (G4int i = 0; i < particleChange->GetNumberOfSecondaries(); i++)
	{
	  auto sec = particleChange->GetSecondary(i);
	  if (std::abs(sec->GetDefinition()->GetPDGEncoding()) == 13)
	    {newMuons.push_back(new G4Track( *(particleChange->GetSecondary(i) ))); delete sec;}
	  else
	    {delete sec;}
	}
    }
  particleChange->Clear();
  particleChange->SetNumberOfSecondaries(0);
  if (nSuccessfulMuonSplits == 0)
    {
      // we've cleared the original ones by now trying, so we have to put them back
      for (auto secondary : originalSecondaries)
        {particleChange->AddSecondary(secondary);}
      for (auto muon : originalMuons)
        {particleChange->AddSecondary(muon);}
      return particleChange;
    } // note muon vector is empty, no need to clear up
    
  particleChange->SetSecondaryWeightByProcess(true);
  particleChange->SetNumberOfSecondaries(nOriginalSecondaries + (G4int)newMuons.size());
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
    }
  
  std::set<G4Track*> finallyToKeep;
  for (G4int i = 0; i < particleChange->GetNumberOfSecondaries(); i++)
    {
      auto sec = particleChange->GetSecondary(i);
      finallyToKeep.insert(sec);
    }
  std::set<G4Track*> definitelyDelete;
  std::set_difference(potentiallyDelete.begin(), potentiallyDelete.end(), finallyToKeep.begin(), finallyToKeep.end(),
                      std::inserter(definitelyDelete, definitelyDelete.begin()));
  
  for (auto p : definitelyDelete)
    {delete p;}
  return particleChange;
}

void BDSWrapperMuonSplitting::DeleteSecondaries(G4VParticleChange* aChange) const
{
  for (G4int i = 0; i < aChange->GetNumberOfSecondaries(); i++)
  {
    auto sec = aChange->GetSecondary(i);
    if (sec)
    {delete sec;}
  }
  aChange->SetNumberOfSecondaries(0);
}

G4bool BDSWrapperMuonSplitting::MuonPresent(G4VParticleChange* aChange) const
{
  G4bool result = false;
  if (!aChange)
    {return result;}
  for (G4int i = 0; i < aChange->GetNumberOfSecondaries(); i++)
    {result = result || std::abs(aChange->GetSecondary(i)->GetDefinition()->GetPDGEncoding()) == 13;}
  return result;
}
