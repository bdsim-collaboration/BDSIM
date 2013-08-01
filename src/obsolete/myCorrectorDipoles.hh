/*
 * Merlin C++ Class Library for Charged Particle Accelerator Simulations
 * 
 * Class library version 2.0 (1999)
 * 
 * file AcceleratorModel\StdComponent\CorrectorDipoles.h
 * last modified 07/06/99 23:07:04
 */
/*
 * This file is derived from software bearing the following
 * restrictions:
 *
 * MERLIN C++ class library for 
 * Charge Particle Accelerator Simulations
 * Copyright (c) 1999 by N.J.Walker.  ALL RIGHTS RESERVED. 
 *
 * Permission to use, copy, modify, distribute and sell this
 * software and its documentation for any purpose is hereby
 * granted without fee, provided that the above copyright notice
 * appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation.
 * No representations about the suitability of this software for
 * any purpose is made. It is provided "as is" without express
 * or implied warranty.
 */

#ifndef myCorrectorDipoles_h
#define myCorrectorDipoles_h 1
#include<vector>

// gab:
//#include "merlin_config.h"

// RectMultipole
//#include "AcceleratorModel/StdComponent/RectMultipole.h"
#include "myRectMultipole.hh"
#include "globals.hh"

//class Tracker;

//	A horizontal corrector dipole.
class XCor : public myRectMultipole  
{
  public:
      //	Constructor taking the identifier for the corrector, the
      //	length and the field (in Tesla).
      XCor (const G4String& id, G4double len, G4double B = 0);

      //	Virtual constructor.
  //      virtual ModelElement* Copy () const;

      //	Returns the unique index for this class of accelerator
      //	components.
  //      virtual IndexType GetIndex () const;

      //	Returns the type G4String for this component.
      virtual const G4String& GetType () const;

      //	Primary tracking interface. Prepares the specified
      //	Tracker object for tracking this component.
  //      virtual void PrepareTracker (Tracker& aTracker);

    // Data Members for Class Attributes

  //      static const IndexType ID;

  protected:
  private:
  private:
};

//	A vertical corrector dipole.

class YCor : public myRectMultipole  
{
  public:
      //	Constructor taking the identifier for the corrector, the
      //	length and the field (in Tesla).
      YCor (const G4String& id, G4double len, G4double B = 0);


      //	Virtual constructor.
  //      virtual ModelElement* Copy () const;

      //	Returns the unique index for this class of accelerator
      //	components.

  //      virtual IndexType GetIndex () const;

      //	Returns the type G4String for this component.
      virtual const G4String& GetType () const;

      //	Primary tracking interface. Prepares the specified
      //	Tracker object for tracking this component.
  //      virtual void PrepareTracker (Tracker& aTracker);

    // Data Members for Class Attributes

  //      static const IndexType ID;

  protected:
  private:
  private:
};

// Class XCor 

inline XCor::XCor (const G4String& id, G4double len, G4double B)
  : myRectMultipole(id,len,0,B){}


// Class YCor 

inline YCor::YCor (const G4String& id, G4double len, G4double B)
  : myRectMultipole(id,len,0,B,true)
{}

#endif
