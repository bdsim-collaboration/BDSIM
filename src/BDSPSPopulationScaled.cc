/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2020.

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

#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSHistBinMapper3D.hh"
#include "BDSScorerConversionLoader.hh"
#include "BDSPSPopulationScaled.hh"
#include "BDSUtilities.hh"

#include "G4PhysicsVector.hh"
#include "G4String.hh"
#include "G4SystemOfUnits.hh"
#include "G4Types.hh"
#include "G4VSolid.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VPVParameterisation.hh"
#include "G4UnitsTable.hh"

#include <fstream>
#include <string>
#include <dirent.h>

#ifdef USE_GZSTREAM
#include "src-external/gzstream/gzstream.h"
#endif

BDSPSPopulationScaled::BDSPSPopulationScaled(const G4String&   scorerName,
                                               G4int           depth):
        G4VPrimitiveScorer(scorerName),
        HCID(-1)
{}

BDSPSPopulationScaled::BDSPSPopulationScaled(const G4String&   scorerName,
                                             const G4String&   pathname,
                                             G4int             depth):
        BDSPSPopulationScaled(scorerName, depth)
{
    G4String directory = BDS::GetFullPath(pathname);
    if (directory.back() != '/')
    {directory += '/';}

    std::vector<G4String> dirs_angle;
    std::vector<G4String> files_particle;

    dirs_angle = loadDirectoryContents(directory);

    G4cout << "Scorer \"" << GetName() << "\" - adding conversionFiles:" << G4endl;

    for (auto dirname_ang : dirs_angle){

        G4String dir_ang = directory + dirname_ang;

        files_particle = loadDirectoryContents(dir_ang);

        BDSScorerConversionLoader<std::ifstream> loader;

        std::map<G4int, G4PhysicsVector*> conversionFactorsPID;

        for (const auto& filePDG : files_particle)
        {
            G4String filepathPDG = dir_ang + '/' + filePDG;
            if (filePDG.substr((filePDG.find_last_of(".") + 1)) == "gz" && BDS::FileExists(filepathPDG))
            {
#ifdef USE_GZSTREAM
                BDSScorerConversionLoader<igzstream> loaderC;
                conversionFactors[(G4double) std::stod(dirname_ang)][(G4int) std::stoi(filePDG)] = loaderC.Load(filepathPDG);
#else
                throw BDSException(__METHOD_NAME__, "Compressed file loading - but BDSIM not compiled with ZLIB.");
#endif
            }
            else if (BDS::FileExists(filepathPDG))
            {
                conversionFactors[(G4double) std::stod(dirname_ang)][(G4int) std::stoi(filePDG)] = loader.Load(filepathPDG);
            }
        }
    }
}

BDSPSPopulationScaled::~BDSPSPopulationScaled()
{
    for (auto af : conversionFactors)
    {
        for (auto cf : af.second)
        {delete cf.second;}
    }
}

void BDSPSPopulationScaled::Initialize(G4HCofThisEvent* HCE)
{
    EvtMap = new G4THitsMap<G4double>(detector->GetName(),
                                        GetName());
    if (HCID < 0)
    {HCID = GetCollectionID(0);}
    HCE->AddHitsCollection(HCID, EvtMap);
}

void BDSPSPopulationScaled::EndOfEvent(G4HCofThisEvent* /*HEC*/)
{;}

void BDSPSPopulationScaled::clear()
{
    EvtMap->clear();
}

G4bool BDSPSPopulationScaled::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
    G4double stepLength = aStep->GetStepLength();
    G4double radiationQuantity = 0;

    if (!BDS::IsFinite(stepLength))
    {return false;}

    const G4VTouchable* touchable = aStep->GetPreStepPoint()->GetTouchable();
    //G4VSolid* solid = touchable->GetSolid();

    G4double weight = aStep->GetPreStepPoint()->GetWeight();

    G4double kineticEnergy = aStep->GetPreStepPoint()->GetKineticEnergy();

    G4double  angle = 0.0;

    G4double factor = GetConversionFactor(aStep->GetTrack()->GetDefinition()->GetPDGEncoding(), kineticEnergy, angle);
    radiationQuantity = weight * factor;
    G4int index = GetIndex(aStep);

    EvtMap->add(index, radiationQuantity);
    return true;
}

G4double BDSPSPopulationScaled::GetConversionFactor(G4int particleID, G4double kineticEnergy, G4double angle) const
{
    std::vector<G4double> angles;
    for(auto const& imap: conversionFactors)
    {
        angles.push_back(imap.first);
    }

    G4double angleNearest = nearestNeighbourAngle(angles, angle);

    std::map<G4int, G4PhysicsVector*> conversionFactorsPart;
    auto search_angle = conversionFactors.find(angleNearest);
    if (search_angle != conversionFactors.end()) {
        conversionFactorsPart = search_angle->second;
    }
    else { return 0; }


    if (particleID < 1e7) {
        auto search = conversionFactorsPart.find(particleID);

        if (search != conversionFactorsPart.end()) { return search->second->Value(kineticEnergy); }
        else { return 0; }
    }
    else
    {
        std::vector<G4int> ionParticleIDs;  // Make a vector of all ion particle IDs
        for(auto const& imap: conversionFactorsPart) {
            G4int pid = imap.first;
            if (pid > 1e7) {
                ionParticleIDs.push_back(imap.first);
            }
        }

        // Get the nearest neighbour ion particle ID based on the ion Z
        G4int particleIDNearest = nearestNeighbourIonPID(ionParticleIDs, particleID);
        if (particleIDNearest < 0){ return 0; }

        // Get the ion Z in order to normalise the kinetic energy for table look-up
        G4double nearestIonZ = (G4double) getZFromParticleID(particleIDNearest);

        auto search_ion = conversionFactorsPart.find(particleID);
        if (search_ion != conversionFactorsPart.end()) {
            return search_ion->second->Value(kineticEnergy/nearestIonZ);
        }
        else { return 0; }
    }
}

std::vector<G4String> BDSPSPopulationScaled::loadDirectoryContents(const G4String& dirname){
    std::vector<G4String> contents;

    struct dirent *entry = nullptr;
    DIR *dp = nullptr;

    dp = opendir(dirname.c_str());
    if (dp == nullptr) {
        throw BDSException(__METHOD_NAME__, "Cannot open directory " + dirname);
    }
    else{
        while ((entry = readdir(dp)) != nullptr) {

            //if (entry == NULL) {
            //    break;
            //}

            std::string name_string(entry->d_name);

            if (name_string.front() != '.') {
                contents.push_back(name_string);
            }
        }
    }

    closedir(dp);

    return contents;
}

G4double BDSPSPopulationScaled::nearestNeighbourAngle(std::vector<G4double> const& vec, G4double value) const {
    auto const it = std::lower_bound(vec.begin(), vec.end(), value);
    if (it == vec.end()) { return -1.e9; }

    return *it;
}

G4int BDSPSPopulationScaled::nearestNeighbourIonPID(std::vector<G4int> const& vec, G4int value) const {

    // Make a vector of the ion Z for all ion particles
    std::vector<G4int> vecZ;
    for(auto pid: vec) {
        vecZ.push_back(getZFromParticleID(pid));
    }

    if (!vecZ.size()){
        return -1;
    }

    G4int nearestNeighbourZ;
    // Perform nearest neighbour interpolation on the ion Z
    auto const it = std::upper_bound(vecZ.begin(), vecZ.end(), getZFromParticleID(value)) -1;
    if (it == vecZ.end()) {
        nearestNeighbourZ = vecZ.back();
    }
    else{
        nearestNeighbourZ = *it;
    }

    // Find the index of the nearest neighbour Z - used to look up the full particle ID
    auto const itr = std::find(vecZ.begin(), vecZ.end(), nearestNeighbourZ);
    int index = std::distance(vecZ.begin(), itr);

    return vec.at(index);
}

G4int  BDSPSPopulationScaled::getZFromParticleID(G4int particleID) const {
    std::ostringstream pid_stream;
    pid_stream << particleID;
    std::string pid_string = pid_stream.str();

    G4int Z = (G4int) std::stoi(pid_string.substr(3, 3));

    return Z;
}