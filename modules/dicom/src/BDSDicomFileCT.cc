/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2021.

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
#include "BDSDicomFileCT.hh"
#include "BDSDicomFileMgr.hh"
#include "BDSDicomFileStructure.hh"
#include "BDSDicomROI.hh"

#include "G4Exception.hh"
#include "G4GeometryTolerance.hh"
#include "G4String.hh"
#include "G4Types.hh"

#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcpixel.h"
#include "dcmtk/dcmdata/dcpxitem.h"
#include "dcmtk/dcmdata/dcpixseq.h"
#include "dcmtk/dcmrt/drtimage.h"

#include <cmath>
#include <limits>
#include <set>
#include <string>
#include <vector>

BDSDicomFileCT::BDSDicomFileCT()
{
  theFileMgr = BDSDicomFileMgr::GetInstance();
}

BDSDicomFileCT::BDSDicomFileCT(DcmDataset* dset):
  BDSDicomVFile(dset)
{
  theFileMgr = BDSDicomFileMgr::GetInstance();
}

void BDSDicomFileCT::ReadData()
{
  std::vector<double> dImagePositionPatient = Read1Data(theDataset, DCM_ImagePositionPatient,3);
  fLocation = dImagePositionPatient[2];
  std::vector<double> dSliceThickness = Read1Data(theDataset, DCM_SliceThickness, 1);
  std::vector<double> dPixelSpacing = Read1Data(theDataset, DCM_PixelSpacing, 2);
  
  std::vector<double> dRows = Read1Data(theDataset, DCM_Rows, 1);
  std::vector<double> dColumns = Read1Data(theDataset, DCM_Columns, 1);
  fNoVoxelY = dRows[0];
  fNoVoxelX = dColumns[0];
  fNoVoxelZ = 1;
  
  fMinX = dImagePositionPatient[0]; // center of upper corner of pixel?
  fMaxX = dImagePositionPatient[0]+dColumns[0]*dPixelSpacing[0];
  
  fMinY = dImagePositionPatient[1];
  fMaxY = dImagePositionPatient[1]+dRows[0]*dPixelSpacing[1];
  
  fMinZ = dImagePositionPatient[2]-dSliceThickness[0]/2.;
  fMaxZ = dImagePositionPatient[2]+dSliceThickness[0]/2.;
  fVoxelDimX = dPixelSpacing[0];
  fVoxelDimY = dPixelSpacing[1];
  fVoxelDimZ = dSliceThickness[0];
  
  if( BDSDicomFileMgr::verbose >= debugVerb )
    {
      G4cout << " DicomVFileImage::ReadData:  fNoVoxel " << fNoVoxelX << " " << fNoVoxelY << " " << fNoVoxelZ << G4endl;
      G4cout << " DicomVFileImage::ReadData:  fMin " << fMinX << " " << fMinY << " " << fMinZ << G4endl;
      G4cout << " DicomVFileImage::ReadData:  fMax " << fMaxX << " " << fMaxY << " " << fMaxZ << G4endl;
      G4cout << " DicomVFileImage::ReadData:  fVoxelDim " << fVoxelDimX << " " << fVoxelDimY << " " << fVoxelDimZ << G4endl;
    }

    std::vector<double> dImageOrientationPatient = Read1Data(theDataset, DCM_ImageOrientationPatient,6);
    fOrientationRows = G4ThreeVector(dImageOrientationPatient[0],
				     dImageOrientationPatient[1],
				     dImageOrientationPatient[2]);
    fOrientationColumns = G4ThreeVector(dImageOrientationPatient[3],
					dImageOrientationPatient[4],
                                        dImageOrientationPatient[5]);
    
    if( fOrientationRows != G4ThreeVector(1,0,0) || fOrientationColumns != G4ThreeVector(0,1,0) )
      {
        G4cerr << " OrientationRows " << fOrientationRows << " OrientationColumns "
               << fOrientationColumns << G4endl;
        G4Exception("DicomVFileImage::ReadData",
                    "DFCT0002",
                    JustWarning,
                    "OrientationRows must be (1,0,0) and OrientationColumns (0,1,0), please contact GAMOS authors");
      }
    fBitAllocated = Read1Data(theDataset, DCM_BitsAllocated, 1)[0];
    if( BDSDicomFileMgr::verbose >= 4 )
      {G4cout << " BIT ALLOCATED " << fBitAllocated << G4endl;}

    std::vector<double> dRescaleSlope = Read1Data(theDataset, DCM_RescaleSlope, 1);
    if( dRescaleSlope.size() == 1 )
      {fRescaleSlope = dRescaleSlope[0];}
    else
      {fRescaleSlope = 1;}
    std::vector<double> dRescaleIntercept = Read1Data(theDataset, DCM_RescaleIntercept, 1);
    if( dRescaleIntercept.size() == 1 )
      {fRescaleIntercept = dRescaleIntercept[0];}
    else
      {fRescaleIntercept = 1;}

    ReadPixelData();
}

void BDSDicomFileCT::ReadPixelData()
{
  //  READING THE PIXELS :
  OFCondition result = EC_Normal;
  //---- CHECK IF DATA IS COMPRESSED
  DcmElement* element = NULL;
  result = theDataset->findAndGetElement(DCM_PixelData, element);
  if (result.bad() || element == NULL)
    {
      G4Exception("ReadData",
		  "findAndGetElement(DCM_PixelData, ",
		  FatalException,
		  ("Element PixelData not found: " + G4String(result.text())).c_str());
    }
  DcmPixelData *dpix = NULL;
  dpix = OFstatic_cast(DcmPixelData*, element);
  // If we have compressed data, we must utilize DcmPixelSequence
  //   in order to access it in raw format, e. g. for decompressing it
  //   with an external library.
  DcmPixelSequence *dseq = NULL;
  E_TransferSyntax xferSyntax = EXS_Unknown;
  const DcmRepresentationParameter *rep = NULL;
  // Find the key that is needed to access the right representation of the data within DCMTK
  dpix->getOriginalRepresentationKey(xferSyntax, rep);
  // Access original data representation and get result within pixel sequence
  result = dpix->getEncapsulatedRepresentation(xferSyntax, rep, dseq);
  if ( result == EC_Normal ) // COMPRESSED DATA
    {
      G4Exception("DicomVFileImage::ReadData()",
		  "DFCT004",
		  FatalException,
		  "Compressed pixel data is not supported");
      
      if( BDSDicomFileMgr::verbose >= debugVerb )
	{G4cout << " DicomVFileImage::ReadData:  result == EC_Normal Reading compressed data " << std::endl;}
      DcmPixelItem* pixitem = NULL;
      // Access first frame (skipping offset table)
      for( int ii = 1; ii < 2; ii++ )
	{
	  OFCondition cond =  dseq->getItem(pixitem, ii);
	  if( !cond.good())
	    {break;}
	  G4cout << ii << " PIX LENGTH " << pixitem->getLength() << G4endl;
        }
      if (pixitem == NULL)
	{
	  G4Exception("ReadData",
		      "dseq->getItem()",
		      FatalException,
		      "No DcmPixelItem in DcmPixelSequence");
        }
      Uint8* pixData = NULL;
      // Get the length of this pixel item
      // (i.e. fragment, i.e. most of the time, the lenght of the frame)
      Uint32 length = pixitem->getLength();
      if (length == 0)
	{
	  G4Exception("ReadData",
		      "pixitem->getLength()",
		      FatalException,
		      "PixelData empty");
        }

      if( BDSDicomFileMgr::verbose >= debugVerb )
	{G4cout << " DicomVFileImage::ReadData:  number of pixels " << length << G4endl;}
      // Finally, get the compressed data for this pixel item
      result = pixitem->getUint8Array(pixData);
    }
  else
    { // UNCOMPRESSED DATA
      if(fBitAllocated == 8)
	{ // Case 8 bits :
	  Uint8* pixData = NULL;
	  if(! (element->getUint8Array(pixData)).good() )
	    {
	      G4Exception("ReadData",
			  "getUint8Array pixData, ",
			  FatalException,
			  ("PixelData not found: " + G4String(result.text())).c_str());
            }
	  for( int ir = 0; ir < fNoVoxelY; ir++ )
	    {
	      for( int ic = 0; ic < fNoVoxelX; ic++ )
		{fHounsfieldV.push_back(pixData[ic+ir*fNoVoxelX]*fRescaleSlope + fRescaleIntercept);}
            }
        }
      else if(fBitAllocated == 16)
	{ // Case 16 bits :
	  Uint16* pixData = NULL;
	  if(! (element->getUint16Array(pixData)).good() )
	    {
	      G4Exception("ReadData",
			  "getUint16Array pixData, ",
			  FatalException,
			  ("PixelData not found: " + G4String(result.text())).c_str());
            }
	  for( int ir = 0; ir < fNoVoxelY; ir++ )
	    {
	      for( int ic = 0; ic < fNoVoxelX; ic++ )
		{fHounsfieldV.push_back(pixData[ic+ir*fNoVoxelX]*fRescaleSlope + fRescaleIntercept);}
            }
        }
      else if(fBitAllocated == 32)
	{ // Case 32 bits :
	  Uint32* pixData = NULL;
	  if(! (element->getUint32Array(pixData)).good() )
	    {
	      G4Exception("ReadData",
			  "getUint32Array pixData, ",
			  FatalException,
			  ("PixelData not found: " + G4String(result.text())).c_str());
            }
	  for( int ir = 0; ir < fNoVoxelY; ir++ )
	    {
	      for( int ic = 0; ic < fNoVoxelX; ic++ )
		{fHounsfieldV.push_back(pixData[ic+ir*fNoVoxelX]*fRescaleSlope + fRescaleIntercept);}
            }
        }
    }
}

void BDSDicomFileCT::operator+=( const BDSDicomFileCT& rhs )
{
  *this = *this + rhs;
}

BDSDicomFileCT BDSDicomFileCT::operator+( const BDSDicomFileCT& rhs )
{
  //----- Check that both slices has the same dimensions
  if( fNoVoxelX != rhs.GetNoVoxelX() || fNoVoxelY != rhs.GetNoVoxelY() )
    {
      G4cerr << "DicomVFileImage error adding two slice headers: !!! Different number of voxels: "
	     << "  X= " << fNoVoxelX << " =? " << rhs.GetNoVoxelX()
	     << "  Y=  " << fNoVoxelY << " =? " << rhs.GetNoVoxelY()
	     << "  Z=  " << fNoVoxelZ << " =? " << rhs.GetNoVoxelZ()
	     << G4endl;
      G4Exception("DicomVFileImage::DicomVFileImage",
		  "",FatalErrorInArgument,"");
    }
  //----- Check that both slices has the same extensions
  if( fMinX != rhs.GetMinX() || fMaxX != rhs.GetMaxX()
      || fMinY != rhs.GetMinY() || fMaxY != rhs.GetMaxY() )
    {
      G4cerr << "DicomVFileImage error adding two slice headers: !!! Different extensions: "
	     << "  Xmin= " << fMinX << " =? " << rhs.GetMinX()
	     << "  Xmax= " << fMaxX << " =? " << rhs.GetMaxX()
	     << "  Ymin= " << fMinY << " =? " << rhs.GetMinY()
	     << "  Ymax= " << fMaxY << " =? " << rhs.GetMaxY()
	     << G4endl;
      G4Exception("DicomVFileImage::operator+","",
		  FatalErrorInArgument,"");
    }
  
  //----- Check that both slices has the same orientations
  if( fOrientationRows != rhs.GetOrientationRows() ||
      fOrientationColumns != rhs.GetOrientationColumns() )
    {
      G4cerr << "DicomVFileImage error adding two slice headers: !!! Slices have different orientations "
	     << "  Orientation Rows = " << fOrientationRows << " & " << rhs.GetOrientationRows()
	     << "  Orientation Columns " << fOrientationColumns << " & "
	     << rhs.GetOrientationColumns() << G4endl;
      G4Exception("DicomVFileImage::operator+","",
		  FatalErrorInArgument,"");
    }
  
  //----- Check that the slices are contiguous in Z
  if( std::fabs( fMinZ - rhs.GetMaxZ() ) > G4GeometryTolerance::GetInstance()->GetRadialTolerance() &&
        std::fabs( fMaxZ - rhs.GetMinZ() ) > G4GeometryTolerance::GetInstance()->GetRadialTolerance() )
    {
      G4cerr << "DicomVFileImage error adding two slice headers: !!! Slices are not contiguous in Z "
	     << "  Zmin= " << fMinZ << " & " << rhs.GetMinZ()
	     << "  Zmax= " << fMaxZ << " & " << rhs.GetMaxZ()
	     << G4endl;
      G4Exception("DicomVFileImage::operator+","",
		  JustWarning,"");
    }
  
  //----- Build slice header copying first one
  BDSDicomFileCT temp( *this );
  
  //----- Add data from second slice header
  temp.SetMinZ( std::min( fMinZ, rhs.GetMinZ() ) );
  temp.SetMaxZ( std::max( fMaxZ, rhs.GetMaxZ() ) );
  temp.SetNoVoxelZ( fNoVoxelZ + rhs.GetNoVoxelZ() );
  
  return temp;
}

void BDSDicomFileCT::DumpHeaderToTextFile(std::ofstream& fout)
{
  if( BDSDicomFileMgr::verbose >= warningVerb )
    {G4cout << fLocation << " DumpHeaderToTextFile " << fFileName << " " << fHounsfieldV.size() << G4endl;}
  
  G4String fName = fFileName.substr(0,fFileName.length()-3) + "g4dcm";
  std::ofstream out(fName.c_str());
  
  if( BDSDicomFileMgr::verbose >= warningVerb )
    {G4cout << "### DicomVFileImage::Dumping Z Slice header to Text file " << G4endl;}
  
  G4int fCompress = theFileMgr->GetCompression();
  fout << fNoVoxelX/fCompress << " " << fNoVoxelY/fCompress << " " << fNoVoxelZ << std::endl;
  fout << fMinX << " " << fMaxX << std::endl;
  fout << fMinY << " " << fMaxY << std::endl;
  fout << fMinZ << " " << fMaxZ << std::endl;
}

void BDSDicomFileCT::Print(std::ostream& out )
{
  G4int fCompress = theFileMgr->GetCompression();
  out << "@@ CT Slice " << fLocation << G4endl;
  
  out << "@ NoVoxels " << fNoVoxelX/fCompress << " " << fNoVoxelY/fCompress << " "
      << fNoVoxelZ << G4endl;
  out << "@ DIM X: " << fMinX << " " << fMaxX
      << " Y: " << fMinY << " " << fMaxY
      << " Z: " << fMinZ << " " << fMaxZ << G4endl;
}

void BDSDicomFileCT::BuildMaterials()
{
  G4int fCompress = theFileMgr->GetCompression();
  if( fNoVoxelX%fCompress != 0 || fNoVoxelY%fCompress != 0 )
    {
      G4Exception("DicompFileMgr.:BuildMaterials",
		  "DFC004",
		  FatalException,
		  ("Compression factor = " + std::to_string(fCompress)
		   + " has to be a divisor of Number of voxels X = " + std::to_string(fNoVoxelX)
		   + " and Y " + std::to_string(fNoVoxelY)).c_str());
    }
  
  //  if( DicomVerb(debugVerb) ) G4cout << " BuildMaterials " << fFileName << G4endl;
  double meanHV = 0.;
  for( int ir = 0; ir < fNoVoxelY; ir += fCompress )
    {
      for( int ic = 0; ic < fNoVoxelX; ic += fCompress )
	{
	  meanHV = 0.;
	  int isumrMax = std::min(ir+fCompress,fNoVoxelY);
	  int isumcMax = std::min(ic+fCompress,fNoVoxelX);
	  for( int isumr = ir; isumr < isumrMax; isumr ++ )
	    {
	      for( int isumc = ic; isumc < isumcMax; isumc ++ )
		{
		  meanHV += fHounsfieldV[isumc+isumr*fNoVoxelX];
		  // G4cout << isumr << " " << isumc << " GET mean " << meanHV << G4endl;
		}
	    }
	  meanHV /= (isumrMax-ir)*(isumcMax-ic);
	  G4double meanDens = theFileMgr->Hounsfield2density(meanHV);
	  //      G4cout << ir << " " << ic << " FINAL mean " << meanDens << G4endl;
	  
	  fDensities.push_back(meanDens);
	  size_t mateID;
	  if( theFileMgr->IsMaterialsDensity() )
	    {mateID = theFileMgr->GetMaterialIndexByDensity(meanDens);}
	  else
	    {mateID = theFileMgr->GetMaterialIndex(meanHV);}
	  fMateIDs.push_back(mateID);
        }
    }
}

void BDSDicomFileCT::DumpMateIDsToTextFile(std::ofstream& fout)
{
  G4int fCompress = theFileMgr->GetCompression();
  if( BDSDicomFileMgr::verbose >= warningVerb )
    {G4cout << fLocation << " DumpMateIDsToTextFile " << fFileName << " " << fMateIDs.size() << G4endl;}
  for( int ir = 0; ir < fNoVoxelY/fCompress; ir++ )
    {
      for( int ic = 0; ic < fNoVoxelX/fCompress; ic++ )
	{
	  fout << fMateIDs[ic+ir*fNoVoxelX/fCompress];
	  if( ic != fNoVoxelX/fCompress-1)
	    {fout << " ";}
        }
      fout << G4endl;
    }
}

void BDSDicomFileCT::DumpDensitiesToTextFile(std::ofstream& fout)
{
  G4int fCompress = theFileMgr->GetCompression();
  if( BDSDicomFileMgr::verbose >= warningVerb )
    {G4cout << fLocation << " DumpDensitiesToTextFile " << fFileName << " " << fDensities.size() << G4endl;}
  
  G4int copyNo = 0;
  for( int ir = 0; ir < fNoVoxelY/fCompress; ir++ )
    {
      for( int ic = 0; ic < fNoVoxelX/fCompress; ic++ )
	{
	  fout << fDensities[ic+ir*fNoVoxelX/fCompress];
	  if( ic != fNoVoxelX/fCompress-1)
	    {fout << " ";}
	  if( copyNo%8 == 7 )
	    {fout << G4endl;}
	  copyNo++;
        }
      if( copyNo%8 != 0 )
	{fout << G4endl;}
    }
}

void BDSDicomFileCT::BuildStructureIDs()
{
  G4int fCompress = theFileMgr->GetCompression();
  std::vector<BDSDicomFileStructure*> dfs = theFileMgr->GetStructFiles();
  if( dfs.size() == 0 )
    {return;}

  G4int NMAXROI = BDSDicomFileMgr::GetInstance()->GetStructureNMaxROI();
  G4int NMAXROI_real = std::log(INT_MAX)/std::log(NMAXROI);
  
  //  fStructure = new G4int(fNoVoxelX*fNoVoxelY);
  for( int ir = 0; ir < fNoVoxelY/fCompress; ir++ )
    {
      for( int ic = 0; ic < fNoVoxelX/fCompress; ic++ )
	{
	  //      fStructure[ic+ir*fNoVoxelX] = 0;
	  fStructure.push_back(0);
        }
    }
  
  std::set<double> distInters;
  
  //  std::fill_n(fStructure,fNoVoxelX*fNoVoxelY,0);
  //
  for( size_t ii = 0; ii < dfs.size(); ii++ )
    {
      std::vector<BDSDicomROI*> rois = dfs[ii]->GetROIs();
        for( size_t jj = 0; jj < rois.size(); jj++ )
	  {
            if( BDSDicomFileMgr::verbose >= debugVerb )
	      {std::cout << " BuildStructureIDs checking ROI " << rois[jj]->GetNumber() << " " << rois[jj]->GetName() << G4endl;}
            std::vector<BDSDicomROIContour*> contours = rois[jj]->GetContours();
            //      G4int roi = jj+1;
            G4int roiID = rois[jj]->GetNumber();
            for( size_t kk = 0; kk < contours.size(); kk++ )
	      {
                // check contour corresponds to this CT slice
                BDSDicomROIContour* roic = contours[kk];
                // if( DicomVerb(-debugVerb) ) G4cout << jj << " " << kk << " INTERS CONTOUR " << roic
                //                << " " << fLocation << G4endl;
                if( BDSDicomFileMgr::verbose >= debugVerb )
		  {G4cout << " " << roiID << " " << kk << " INTERS CONTOUR " << roic->GetZ() << " SLICE Z " << fMinZ << " " << fMaxZ << G4endl;}
                // Check Contour correspond to slice
		
                if( roic->GetZ() > fMaxZ || roic->GetZ() < fMinZ )
		  {continue;}
                if( BDSDicomFileMgr::verbose >= debugVerb )
		  {G4cout << " INTERS CONTOUR WITH Z SLIZE " << fMinZ << " < " << roic->GetZ() << " < " << fMaxZ << G4endl;}
                if( roic->GetGeomType() == "CLOSED_PLANAR" )
		  {
                    // Get min and max X and Y, and corresponding slice indexes
                    std::vector<G4ThreeVector> points = roic->GetPoints();
                    if( BDSDicomFileMgr::verbose >= debugVerb )
		      {G4cout << jj << " " << kk << " NPOINTS " << points.size() << G4endl;}
                    std::vector<G4ThreeVector> dirs = roic->GetDirections();
                    double minXc = DBL_MAX;
                    double maxXc = -DBL_MAX;
                    double minYc = DBL_MAX;
                    double maxYc = -DBL_MAX;
                    for( size_t ll = 0; ll < points.size(); ll++ )
		      {
                        minXc = std::min(minXc,points[ll].x());
                        maxXc = std::max(maxXc,points[ll].x());
                        minYc = std::min(minYc,points[ll].y());
                        maxYc = std::max(maxYc,points[ll].y());
		      }
                    if( minXc < fMinX || maxXc > fMaxX || minYc < fMinY || maxYc > fMaxY )
		      {
                        G4cerr << " minXc " << minXc << " < " << fMinX
                               << " maxXc " << maxXc << " > " << fMaxX
                               << " minYc " << minYc << " < " << fMinY
                               << " maxYc " << maxYc << " > " << fMaxY << G4endl;
                        G4Exception("DicomFileCT::BuildStructureIDs",
                                    "DFCT001",
                                    JustWarning,
                                    "Contour limits exceed Z slice extent");
		      }
                    int idMinX = std::max(0,int((minXc-fMinX)/fVoxelDimX/fCompress));
                    int idMaxX = std::min(fNoVoxelX/fCompress-1,int((maxXc-fMinX)/fVoxelDimX/fCompress+1));
                    int idMinY = std::max(0,int((minYc-fMinY)/fVoxelDimY/fCompress));
                    int idMaxY = std::min(fNoVoxelY/fCompress-1,int((maxYc-fMinY)/fVoxelDimY/fCompress+1));
                    if( BDSDicomFileMgr::verbose >= debugVerb )
		      {
			G4cout << " minXc " << minXc << " < " << fMinX
                               << " maxXc " << maxXc << " > " << fMaxX
                               << " minYc " << minYc << " < " << fMinY
                               << " maxYc " << maxYc << " > " << fMaxY << G4endl;
		      }
                    if( BDSDicomFileMgr::verbose >= debugVerb )
		      {
                        G4cout << " idMinX " << idMinX
                               << " idMaxX " << idMaxX
                               << " idMinY " << idMinY
                               << " idMaxY " << idMaxY << G4endl;
		      }
                    //for each voxel: build 4 lines from the corner towards the center
                    // and check how many contour segments it crosses, and the minimum distance to a segment
                    for( int ix = idMinX; ix <= idMaxX; ix++ )
		      {
                        for( int iy = idMinY; iy <= idMaxY; iy++ )
			  {
                            G4bool bOK = false;
                            G4int bOKs;
                            if( BDSDicomFileMgr::verbose >= debugVerb )
			      {
				G4cout << "@@@@@ TRYING POINT (" <<  fMinX + fVoxelDimX*fCompress*(ix+0.5) << ","
				       <<  fMinY + fVoxelDimY*fCompress*(iy+0.5) << ")" << G4endl;
			      }
                            // four corners
                            for( int icx = 0; icx <= 1; icx++ )
			      {
                                for( int icy = 0; icy <= 1; icy++ )
				  {
                                    bOKs = 0;
                                    if( bOK )
				      {continue;}
                                    double p0x = fMinX + fVoxelDimX*fCompress * (ix+icx);
                                    double p0y = fMinY + fVoxelDimY*fCompress * (iy+icy);
                                    double v0x = 1.;
                                    if( icx == 1 )
				      {v0x = -1.;}
                                    double v0y = 0.99*fVoxelDimY/fVoxelDimX*std::pow(-1.,icy);
                                    if( BDSDicomFileMgr::verbose >= testVerb )
				      {
					G4cout << ix << " + " << icx << " "
					       << iy << " + " << icy << " CORNER (" << p0x << "," << p0y << ") "
					       << " DIR= (" << v0x << "," << v0y << ") " << G4endl;
				      }
                                    int NTRIES = theFileMgr->GetStructureNCheck();
                                    for( int ip = 0; ip < NTRIES; ip++)
				      {
                                        distInters.clear();
                                        v0y -= ip*0.1*v0y;
                                        G4double halfDiagonal = 0.5*fVoxelDimX*fCompress;
                                        if( BDSDicomFileMgr::verbose >= testVerb )
					  {
					    G4cout << ip
						   << " TRYING WITH DIRECTION (" << " DIR= (" << v0x << ","
						   << v0y << ") " << bOKs << G4endl;
					  }
					for( size_t ll = 0; ll < points.size(); ll++ )
					  {
                                            double d0x = points[ll].x() - p0x;
                                            double d0y = points[ll].y() - p0y;
                                            double w0x = dirs[ll].x();
                                            double w0y = dirs[ll].y();
                                            double fac1 = w0x*v0y - w0y*v0x;
                                            if( fac1 == 0 )
					      { // parallel lines
                                                continue;
					      }
                                            double fac2 = d0x*v0y - d0y*v0x;
                                            double fac3 = d0y*w0x - d0x*w0y;
                                            double lambdaq = -fac2/fac1;
                                            if( lambdaq < 0. || lambdaq >= 1. )
					      {continue;}
                                            // intersection further than segment length
                                            double lambdap = fac3/fac1;
                                            if( lambdap > 0. )
					      {
                                                distInters.insert(lambdap);
                                                if( BDSDicomFileMgr::verbose >= testVerb )
						  {
						    G4cout << " !! GOOD INTERS "
							   <<lambdaq << "  (" << d0x+p0x+lambdaq*w0x << ","
							   << d0y+p0y+lambdaq*w0y
							   << ")  =  (" << p0x+lambdap*v0x << ","
							   << p0y+lambdap*v0y << ") "
							   << " N " << distInters.size() << G4endl;
						  }
					      }
                                            if( BDSDicomFileMgr::verbose >= testVerb )
					      {
						G4cout << " INTERS LAMBDAQ "
						       << lambdaq << " P " << lambdap << G4endl;
					      }
                                            if( BDSDicomFileMgr::verbose >= debugVerb )
					      {
						G4cout << " INTERS POINT ("
						       << d0x+p0x+lambdaq*w0x << "," << d0y+p0y+lambdaq*w0y << ")  =  ("
						       << p0x+lambdap*v0x << "," << p0y+lambdap*v0y << ") " << G4endl;
					      }
					  }
                                        if( distInters.size() % 2 == 1 )
					  {
                                            if( *(distInters.begin() ) > halfDiagonal )
					      {
                                                //                      bOK = true;
                                                bOKs += 1;
                                                if( BDSDicomFileMgr::verbose >= debugVerb )
						  {
						    G4cout << " OK= " << bOKs
							   << " N INTERS " << distInters.size() << " " << *(distInters.begin())
							   << " > " << halfDiagonal <<  G4endl;
						  }
					      }
					  }
				      }
                                    if(bOKs == NTRIES )
				      {
                                        bOK = true;
                                        if( BDSDicomFileMgr::verbose >= debugVerb )
					  {
					    G4cout << "@@@@@ POINT OK ("
						   <<  fMinX + fVoxelDimX*fCompress*(ix+0.5) << ","
						   <<  fMinY + fVoxelDimY*fCompress*(iy+0.5) << ")" << G4endl;
					  }
				      }
				    
				  }
			      }
			    // loop to four corners
                            if( bOK )
			      {
                                // extract previous ROI value
                                int roival = fStructure[ix+iy*fNoVoxelX/fCompress];
                                //                roival = 2 + NMAXROI*3 + NMAXROI*NMAXROI*15;
                                if(roival != 0 && roival != int(roiID) ) {
                                    std::set<G4int> roisVoxel;
                                    int nRois = std::log10(roival)/std::log10(NMAXROI)+1;
                                    if( nRois > NMAXROI_real ) { // another one cannot be added
                                        G4Exception("DicomFileCT:BuildStructureIDs",
                                                    "DFC0004",
                                                    FatalException,
                                                    G4String("Too many ROIs associated to a voxel: \
" + std::to_string(nRois) + " > " + std::to_string(NMAXROI_real) + " TRY CHAN\
GING -NStructureNMaxROI argument to a lower value").c_str());
                                    }
                                    for( int inr = 0; inr < nRois; inr++ ) {
                                        roisVoxel.insert( int(roival/std::pow(NMAXROI,inr))%NMAXROI );
                                    }
                                    roisVoxel.insert(roiID);
                                    roival = 0;
                                    size_t inr = 0;
                                    for( std::set<G4int>::const_iterator ite = roisVoxel.begin();
                                         ite != roisVoxel.end(); ite++, inr++ ) {
                                        roival += (*ite)*std::pow(NMAXROI,inr);
                                    }
                                    fStructure[ix+iy*fNoVoxelX/fCompress] = roival;
                                    if( BDSDicomFileMgr::verbose >= testVerb ){
                                        G4cout << " WITH PREVIOUS ROI IN VOXEL " << roival << G4endl;
                                    }
                                } else {
                                    fStructure[ix+iy*fNoVoxelX/fCompress] = roiID;
                                }
                            }

                        }
                    }
                }
            }
        }
    }

    if( BDSDicomFileMgr::verbose >= 1 ) {
        //@@@@ PRINT structures
        //@@@ PRINT points of structures in this Z slice
        if( BDSDicomFileMgr::verbose >= 0 ) G4cout << " STRUCTURES FOR SLICE " << fLocation << G4endl;
        for( size_t ii = 0; ii < dfs.size(); ii++ ){
            std::vector<BDSDicomROI*> rois = dfs[ii]->GetROIs();
            for( size_t jj = 0; jj < rois.size(); jj++ ){
                int roi = rois[jj]->GetNumber();
                std::vector<BDSDicomROIContour*> contours = rois[jj]->GetContours();
                for( size_t kk = 0; kk < contours.size(); kk++ ){
                    BDSDicomROIContour* roic = contours[kk];
                    // check contour corresponds to this CT slice
                    if( roic->GetZ() > fMaxZ || roic->GetZ() < fMinZ ) continue;
                    if( roic->GetGeomType() == "CLOSED_PLANAR" ){
                        if( BDSDicomFileMgr::verbose >= testVerb ) G4cout << " PRINTING CONTOUR? " << roi << " "
                                                                       << kk << " INTERS CONTOUR " << roic->GetZ() << " SLICE Z "
                                                                       << fMinZ << " " << fMaxZ << G4endl;
                        if( roic->GetZ() > fMaxZ || roic->GetZ() < fMinZ ) continue;
                        std::vector<G4ThreeVector> points = roic->GetPoints();
                        std::vector<G4ThreeVector> dirs = roic->GetDirections();
                        if( BDSDicomFileMgr::verbose >= 0 ) std::cout << " STRUCTURE Z " << roic->GetZ()
                                                                   << std::endl;
                        for( size_t ll = 0; ll < points.size(); ll++ ){
                            if( BDSDicomFileMgr::verbose >= 0 ) G4cout << roi << " : " << ll
                                                                    << " STRUCTURE POINT (" << points[ll].x() << "," << points[ll].y() << ") "
                                                                    << " (" << dirs[ll].x() << "," << dirs[ll].y() << ") = " << roi << G4endl;
                        }
                    }
                }
            }
        }
        //@@@ PRINT points in slice inside structure
        for( int ir = 0; ir < fNoVoxelY/fCompress; ir++ ) {
            for( int ic = 0; ic < fNoVoxelX/fCompress; ic++ ) {
                if( fStructure[ic+ir*fNoVoxelX/fCompress] != 0 ) {
                    if( BDSDicomFileMgr::verbose >= 0 ) G4cout << ic+ir*fNoVoxelX/fCompress << " = " << ic
                                                            << " " << ir << " STRUCTURE VOXEL (" << fMinX + fVoxelDimX*fCompress * (ic+0.5)
                                                            << "," << fMinY + fVoxelDimY*fCompress * (ir+0.5) << ") = "
                                                            << fStructure[ic+ir*fNoVoxelX/fCompress] << G4endl;
                }
            }
        }
    }

}

void BDSDicomFileCT::DumpStructureIDsToTextFile(std::ofstream& fout)
{
    G4int fCompress = theFileMgr->GetCompression();
    if( BDSDicomFileMgr::verbose >= 0 ) G4cout << fLocation << " DumpStructureIDsToTextFile "
                                            << fFileName << " " << fStructure.size() << G4endl;
    std::vector<BDSDicomFileStructure*> dfs = theFileMgr->GetStructFiles();
    if( dfs.size() == 0 ) return;

    for( int ir = 0; ir < fNoVoxelY/fCompress; ir++ ) {
        for( int ic = 0; ic < fNoVoxelX/fCompress; ic++ ) {
            fout << fStructure[ic+ir*fNoVoxelX/fCompress];
            if( ic != fNoVoxelX/fCompress-1) fout << " ";
        }
        fout << G4endl;
    }
}
