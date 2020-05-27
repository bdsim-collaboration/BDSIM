//
// Created by Eliott Ramoisiaux on 26/05/2020.
//

#include "BDSHistBinMapper.hh"
#include "G4ThreeVector.hh"


BDSHistBinMapper::BDSHistBinMapper(G4int nBinsIIn, G4int nBinsJIn, G4int nBinsKIn, G4int nBinsLIn):nBinsI(nBinsIIn),nBinsJ(nBinsJIn),nBinsK(nBinsKIn),nBinsL(nBinsLIn)
{

}

G4int BDSHistBinMapper::GlobalFromIJKLIndex(G4int iIndex,
                                              G4int jIndex,
                                              G4int kIndex,
                                              G4int lIndex) const
{
    return iIndex*nBinsJ*nBinsK*nBinsL + jIndex*nBinsK*nBinsL + kIndex*nBinsL +lIndex;
}

void BDSHistBinMapper::IJKLFromGlobal(G4int  globalBin,
                                        G4int& iIndex,
                                        G4int& jIndex,
                                        G4int& kIndex,
                                        G4int& lIndex) const
{
    iIndex = globalBin / (nBinsL*nBinsK*nBinsJ);
    jIndex = (globalBin - iIndex*nBinsL*nBinsK*nBinsJ) / (nBinsL*nBinsK);
    kIndex = (globalBin - jIndex*nBinsL*nBinsK - iIndex*nBinsL*nBinsK*nBinsJ) / nBinsL;
    lIndex = globalBin - kIndex*nBinsL - jIndex*nBinsL*nBinsK - iIndex*nBinsL*nBinsK*nBinsJ;
}