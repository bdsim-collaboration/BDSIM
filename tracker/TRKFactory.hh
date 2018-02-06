/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

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
#ifndef TRKFactory_H
#define TRKFactory_H

#include "TRKElement.hh"
#include "TRK.hh"

#include "parser/element.h"
#include "parser/fastlist.h"

class BDSParticleDefinition;

namespace GMAD
{
  class Beam;
  class Options;
}

#include <string>

struct Element;

class TRKAperture;
class TRKLine;
class TRKPlacement;
class TRKStrategy;

/**
 * @brief factory to create beamline elements
 */

class TRKFactory
{
 public:
  TRKFactory(const GMAD::Options& options,
	     BDSParticleDefinition* particle);

  TRKLine*     CreateLine(const GMAD::FastList<GMAD::Element>& beamline_list);
  TRKStrategy* CreateStrategy();

  friend std::ostream& operator<< (std::ostream& out, const TRKFactory &factory);

private:
  TRK::Strategy SetStrategyEnum(std::string sIn);
  TRK::Aperture SetApertureEnum(std::string aIn);
  
  TRKAperture* CreateAperture(GMAD::Element& element);
  TRKElement*  CreateElement(GMAD::Element& element);

  TRKElement* CreateLine(GMAD::Element& element);
  TRKElement* CreateDrift(GMAD::Element& element);
  TRKElement* CreateSBend(GMAD::Element& element);
  TRKElement* CreateRBend(GMAD::Element& element);
  TRKElement* CreateDipole(GMAD::Element& element);
  TRKElement* CreateQuadrupole(GMAD::Element& element);
  TRKElement* CreateSextupole(GMAD::Element& element);
  TRKElement* CreateOctupole(GMAD::Element& element);
  TRKElement* CreateDecapole(GMAD::Element& element);
  TRKElement* CreateSolenoid(GMAD::Element& element);
  //TRKElement* CreateMultipole(GMAD::Element& element);
  //TRKElement* CreateGmadElement(GMAD::Element& element);
  TRKElement* CreateSampler(GMAD::Element& element);
  //TRKElement* CreateTransform3D(GMAD::Element& element);

  /// set common properties
  void AddCommonProperties(TRKElement* trkel, GMAD::Element& el);

  /// particle properties
  int charge;
  /// beam momentum in GeV
  double momentum;
  double energy;
  double brho;

  /// global placement position
  TRKPlacement* placement;
  
  /// circular flag
  bool circular;
  
  /// tracking strategy
  TRK::Strategy strategy;
  int   trackingsteps;
  
  /// aperture
  TRKAperture* defaultaperture; /// default aperture
  TRK::Aperture aperturetype;   /// enum of aperture type
  double beampiperadius;
  bool   dontuseaperture;
};

#endif
