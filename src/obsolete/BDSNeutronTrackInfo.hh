/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: BDSNeutronTrackInfo.hh,v 1.1 2005/01/22 17:05:31 agapov Exp $
// GEANT4 tag $Name:  $
//
//
//---------------------------------------------------------------
//
// G4VUserTrackInformation
//
// Class Description:
//
//  Abstract class which the user can derive his/her own concrete
// class for toring user's information associating with a G4Track
// class object.
//
//  It is user's responsibility that
//   1) Concrete class derived from this class MUST use G4Allocator
//     for memory management
//   2) Construct a concrete class object and set the pointer to
//     proper G4Track object
//
//  To set a pointer of a concrete class object to G4Track in
// G4UserTrackingAction concrete implementation, given the G4Track
// object is available only by "pointer to const", SetUserTrackInformation()
// method of G4TrackingManager is available.
//
//  The concrete class object is deleted by Geant4 kernel when
// associated G4Track object is deleted.


#ifndef BDSNeutronTrackInfo_H
#define BDSNeutronTrackInfo_H 1

#include "globals.hh" 
#include "G4VUserTrackInformation.hh"
#include "G4Allocator.hh"


class BDSNeutronTrackInfo:public G4VUserTrackInformation
{
public:
  //  BDSNeutronTrackInfo():G4VUserTrackInformation(){;};
  BDSNeutronTrackInfo(){;};
  ~BDSNeutronTrackInfo(){;};

  inline void *operator new(size_t);
  inline void operator delete(void* anInfo);


  inline G4bool GetIsLogged();
  inline void SetIsLogged(G4bool logger);


public:
  void Print() const;

private:
  G4bool IsLogged;
};

inline G4bool BDSNeutronTrackInfo::GetIsLogged()
{return IsLogged;}

inline void BDSNeutronTrackInfo::SetIsLogged(G4bool logger)
{IsLogged=logger;}

inline void BDSNeutronTrackInfo::Print() const
{G4cout<<" BDSNeutronTrackInfo: IsLogged="<<IsLogged<<G4endl;}


extern G4Allocator<BDSNeutronTrackInfo> theBDSNeutronTrackInfoAllocator;

inline void* BDSNeutronTrackInfo::operator new(size_t)
{
  void* anInfo;
  anInfo = (void*)theBDSNeutronTrackInfoAllocator.MallocSingle();
  return anInfo;
}

inline void BDSNeutronTrackInfo::operator delete(void* anInfo)
{
  theBDSNeutronTrackInfoAllocator.FreeSingle((BDSNeutronTrackInfo*)anInfo);
}


#endif

