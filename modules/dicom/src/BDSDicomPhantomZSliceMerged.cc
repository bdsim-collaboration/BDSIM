//
// Created by strangesyd on 3/12/21.
//

#include "BDSDicomPhantomZSliceMerged.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BDSDicomPhantomZSliceMerged::BDSDicomPhantomZSliceMerged()
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BDSDicomPhantomZSliceMerged::~BDSDicomPhantomZSliceMerged()
{
    fSlices.clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BDSDicomPhantomZSliceMerged::CheckSlices()
{
    G4cout << "\nDicomPhantomZSliceMerged::Checking "
           << fSlices.size() << " fSlices..." << G4endl;

    G4bool uniformSliceThickness = true;

    if(fSlices.size() > 1) {
        if(fSlices.size() == 2) {
            BDSDicomPhantomZSliceHeader* one = fSlices.begin()->second;
            BDSDicomPhantomZSliceHeader* two = fSlices.end()->second;

            G4double real_distance = (two->GetSliceLocation()-one->GetSliceLocation())/2.;

            if(one->GetMaxZ() != two->GetMinZ()) {
                one->SetMaxZ(one->GetSliceLocation()+real_distance);
                two->SetMinZ(two->GetSliceLocation()-real_distance);
                //one->SetMinZ(one->GetSliceLocation()-real_distance);
                //two->SetMaxZ(two->GetSliceLocation()+real_distance);
                if(uniformSliceThickness) {
                    one->SetMinZ(one->GetSliceLocation()-real_distance);
                    two->SetMaxZ(two->GetSliceLocation()+real_distance);
                }
            }
        } else {
            auto ite0 = fSlices.begin();
            auto ite1 = fSlices.begin();
            auto ite2 = fSlices.begin();
            ++ite1;
            ++ite2; ++ite2;

            for(; ite2 != fSlices.end(); ++ite0, ++ite1, ++ite2)
            {
                BDSDicomPhantomZSliceHeader* prev = ite0->second;
                BDSDicomPhantomZSliceHeader* slice = ite1->second;
                BDSDicomPhantomZSliceHeader* next = ite2->second;
                G4double real_max_distance = (next->GetSliceLocation() -
                                              slice->GetSliceLocation())/2.;
                G4double real_min_distance = (slice->GetSliceLocation() -
                                              prev->GetSliceLocation())/2.;
                G4double real_distance = real_max_distance + real_min_distance;
                G4double stated_distance = slice->GetMaxZ()-slice->GetMinZ();
                if(real_distance != stated_distance) {
                    uintmax_t sliceNum = std::distance(fSlices.begin(),ite1);
                    G4cout << "\tBDSDicomPhantomZSliceMerged::CheckSlices - Slice Distance Error in slice [" << sliceNum
                           << "]: Real Distance = "
                           << real_distance/mm
                           << " mm, Stated Distance = " << stated_distance/mm << G4endl;
                    slice->SetMinZ(slice->GetSliceLocation()-real_min_distance);
                    slice->SetMaxZ(slice->GetSliceLocation()+real_max_distance);

                    if(ite0 == fSlices.begin()) {
                        prev->SetMaxZ(slice->GetMinZ());
                        // Using below would make all slice same thickness
                        //prev->SetMinZ(prev->GetSliceLocation()-real_min_distance);
                        if(uniformSliceThickness) {
                            prev->SetMinZ(prev->GetSliceLocation()-real_min_distance); }

                    }
                    if(static_cast<unsigned int>(std::distance(fSlices.begin(),ite2)+1)==
                       fSlices.size()) {
                        next->SetMinZ(slice->GetMaxZ());
                        // Using below would make all slice same thickness
                        //next->SetMaxZ(next->GetSliceLocation()+real_max_distance);
                        if(uniformSliceThickness) {
                            next->SetMaxZ(next->GetSliceLocation()+real_max_distance); }
                    }
                }
            }
        }
    }
    G4cout << G4endl;

    for(auto ite = fSlices.cbegin(); ite != fSlices.cend(); ++ite) {
        ite->second->DumpToFile();
    }

}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
