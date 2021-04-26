//
// Created by strangesyd on 3/7/21.
//

#include "BDSDicomROIContour.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
BDSDicomROIContour::BDSDicomROIContour()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void BDSDicomROIContour::SetData( std::vector<G4ThreeVector> data )
{
    thePoints = data;
    if( theGeomType == "CLOSED_PLANAR" ){
        //add a last point to close the circle
        thePoints.push_back( thePoints[0] );
        for( size_t ii = 1; ii < thePoints.size(); ii++ ){
            theDirections.push_back( thePoints[ii] - thePoints[ii-1] );
        }
        theDirections.push_back( thePoints[0] - thePoints[thePoints.size()-1] ); // (0,0)
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4double BDSDicomROIContour::GetZ()
{
    if( thePoints.size() == 0 ) {
        G4Exception("DicomROIContour::GetZ",
                    "DRC001",
                    FatalException,
                    "No Point in contour");
        Print(G4cout);
        return DBL_MAX;
    }
    return thePoints[0].z();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void BDSDicomROIContour::AddPoints( std::vector<G4ThreeVector> points )
{
    points.push_back( points[0] );
    size_t npold = thePoints.size();
    for( size_t ii = 0; ii < points.size(); ii++ ) {
        thePoints.push_back( points[ii] );
    }

    for( size_t ii = npold+1; ii < thePoints.size(); ii++ ){
        theDirections.push_back( thePoints[ii] - thePoints[ii-1] );
    }
    theDirections.push_back( thePoints[npold] - thePoints[thePoints.size()-1] );

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void BDSDicomROIContour::Print( std::ostream& out )
{

    out << this << "@@@ NUMBER OF ContourImageSequences " << theImageIUIDs.size() << G4endl;
    for( size_t ii = 0; ii < theImageIUIDs.size(); ii++ ) {
        out << "ContourImageSequence I= " << theImageIUIDs[ii] << G4endl;
    }
    out << "@@@ GeomType " << theGeomType << " NPOINTS " << thePoints.size() << G4endl;
    for( size_t ii = 0; ii < thePoints.size(); ii++ ) {
        out << "(" << thePoints[ii].x()<< "," << thePoints[ii].y()<< "," << thePoints[ii].z()
            << ")" << G4endl;
    }

}
