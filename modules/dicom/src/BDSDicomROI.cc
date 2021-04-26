//
// Created by strangesyd on 3/7/21.
//

#include "BDSDicomROI.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
BDSDicomROI::BDSDicomROI(int ROINumber, OFString ROIName) : theNumber(ROINumber+1), theName(ROIName)
{
    G4cout << " CREATED  DicomROI "  << theNumber << " : " << theName << G4endl; //GDEB
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void BDSDicomROI::AddContour( BDSDicomROIContour* cont )
{
    // check if it already exists a contour with same Z
    G4bool bOK = false;
    size_t ii;
    for( ii = 0; ii < theContours.size(); ii++ ){
        if( cont->GetZ() == theContours[ii]->GetZ() ) {
            bOK = true;
            break;
        }
    }
    if( !bOK ) {
        theContours.push_back( cont );
    } else {
        theContours[ii]->AddPoints( cont->GetPoints() );
        delete cont;
    }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void BDSDicomROI::Print( std::ostream& out )
{
    out << "@@@@@ ROI: " << theNumber << " " << theName << G4endl;

    out <<"@@@@ NUMBER OF ContourSequences " << theContours.size() << G4endl;
    for( size_t ii = 0; ii < theContours.size(); ii++ ) {
        out <<"@@@@ CONTOUR " << ii << G4endl;
        theContours[ii]->Print(out);
    }
}
