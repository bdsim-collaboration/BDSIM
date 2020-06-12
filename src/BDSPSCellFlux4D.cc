//
// Created by Eliott Ramoisiaux on 25/05/2020.
//

#include "BDSPSCellFlux4D.hh"
#include "BDSHistBinMapper.hh"

#include <boost/histogram.hpp>


BDSPSCellFlux4D::BDSPSCellFlux4D(G4String name, const G4double eLow, const G4double eHigh, unsigned int ne, const BDSHistBinMapper* mapperIn, G4int ni, G4int nj, G4int nk, G4int depi, G4int depj, G4int depk):
G4PSCellFlux3D(name,ni,nj,nk,depi,depj,depk), fDepthi(depi), fDepthj(depj), fDepthk(depk),ne(ne), eLow(eLow), eHigh(eHigh), mapper(mapperIn)
{

}

BDSPSCellFlux4D::BDSPSCellFlux4D(G4String name, const G4double eLow, const G4double eHigh, unsigned int ne, const BDSHistBinMapper* mapperIn, const G4String& unit, G4int ni, G4int nj, G4int nk, G4int depi, G4int depj, G4int depk):
        G4PSCellFlux3D(name, unit,ni,nj,nk,depi,depj,depk), fDepthi(depi), fDepthj(depj), fDepthk(depk),ne(ne), eLow(eLow), eHigh(eHigh), mapper(mapperIn)
{

}


G4int BDSPSCellFlux4D::GetIndex(G4Step* aStep)
{
    const G4VTouchable* touchable = aStep->GetPreStepPoint()->GetTouchable();

    G4int i = touchable->GetReplicaNumber(fDepthi);
    G4int j = touchable->GetReplicaNumber(fDepthj);
    G4int k = touchable->GetReplicaNumber(fDepthk);

    boost::histogram::axis::regular<double,boost::histogram::axis::transform::log> Laxis = boost::histogram::axis::regular<double, boost::histogram::axis::transform::log> {ne, eLow, eHigh, "Energy_log"};

    G4int l = Laxis.index(aStep->GetPostStepPoint()->GetKineticEnergy());


    if (i<0 || j<0 || k<0)
    {
        G4ExceptionDescription ED;
        ED << "GetReplicaNumber is negative" << G4endl
           << "touchable->GetReplicaNumber(fDepthi) returns i,j,k = "
           << i << "," << j << "," << k << "," << l << " for volume "
           << touchable->GetVolume(fDepthi)->GetName() << ","
           << touchable->GetVolume(fDepthj)->GetName() << ","
           << touchable->GetVolume(fDepthk)->GetName() << "," << G4endl;
        G4Exception("PSRadiationQuantity3D::GetIndex","DetPS0006",JustWarning,ED);
    }

    G4int globalIndex = mapper->GlobalFromIJKLIndex(i,j,k,l); // x,y,z,e
    return globalIndex;
}