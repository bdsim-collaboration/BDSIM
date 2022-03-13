/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
#include "particlefilter.h"

#include <iostream>
#include <list>

using namespace GMAD;

ParticleFilter::ParticleFilter()
{
  clear();
  PublishMembers();
}

void ParticleFilter::clear()
{
  name = "";
  partIDToStop.clear();
  partIDToAllow.clear();
  minimumEk.clear();
  samplerType = "plane";
  referenceElement = "";
  referenceElementNumber = 0;
  s     = 0;
  x     = 0;
  y     = 0;
  z     = 0;
  phi   = 0;
  theta = 0;
  psi   = 0;
  axisX = 0;
  axisY = 0;
  axisZ = 0;
  angle = 0;
  axisAngle = false;
  apertureModel = "";
  shape = "circular";
  aper1 = 0;
  aper2 = 0;
  aper3 = 0;
  aper4 = 0;
  startAnglePhi = 0;
  sweepAnglePhi = -1;   // -1 to flag we should use 2pi later when we have units
  startAngleTheta = 0;
  sweepAngleTheta = -1; // -1 to flag we should use 2pi later when we have units
}

void ParticleFilter::PublishMembers()
{
  publish("name",          &ParticleFilter::name);
  publish("partIDToStop",  &ParticleFilter::partIDToStop);
  publish("partIDToAllow", &ParticleFilter::partIDToAllow);
  publish("minimumEk",     &ParticleFilter::minimumEk);
  publish("samplerType",   &ParticleFilter::samplerType);
  publish("referenceElement", &ParticleFilter::referenceElement);
  publish("referenceElementNumber", &ParticleFilter::referenceElementNumber);
  publish("s",             &ParticleFilter::s);
  publish("x",             &ParticleFilter::x);
  publish("y",             &ParticleFilter::y);
  publish("z",             &ParticleFilter::z);
  publish("phi",           &ParticleFilter::phi);
  publish("theta",         &ParticleFilter::theta);
  publish("psi",           &ParticleFilter::psi);
  publish("axisX",         &ParticleFilter::axisX);
  publish("axisY",         &ParticleFilter::axisY);
  publish("axisZ",         &ParticleFilter::axisZ);
  publish("angle",         &ParticleFilter::angle);
  publish("axisAngle",     &ParticleFilter::axisAngle);
  publish("apertureModel", &ParticleFilter::apertureModel);
  publish("shape",         &ParticleFilter::shape);
  publish("apertureType",  &ParticleFilter::shape);
  publish("aper1",         &ParticleFilter::aper1);
  publish("aper2",         &ParticleFilter::aper2);
  publish("aper3",         &ParticleFilter::aper3);
  publish("aper4",         &ParticleFilter::aper4);
  publish("startAnglePhi", &ParticleFilter::startAnglePhi);
  publish("sweepAnglePhi", &ParticleFilter::sweepAnglePhi);
  publish("startAngleTheta", &ParticleFilter::startAngleTheta);
  publish("sweepAngleTheta", &ParticleFilter::sweepAngleTheta);
}

void ParticleFilter::print()const
{
  std::cout << "particlefilter: "
	    << "name "          << name          << std::endl
    	    << "partIDToStop list {";
  for (auto v : partIDToStop)
    {std::cout << v << ", ";}
  std::cout << "}" << std::endl
	    << "partIDToAllow list {";
  for (auto v : partIDToAllow)
    {std::cout << v << ", ";}
  std::cout << "}" << std::endl
    	    << "minimumEk list {";
  for (auto v : minimumEk)
    {std::cout << v << ", ";}
  std::cout << "}" << std::endl
	    << "samplerType "   << samplerType   << std::endl
	    << "referenceElement" << referenceElement << std::endl
	    << "referenceElementNumber" << referenceElementNumber << std::endl
	    << "s"              << s             << std::endl
	    << "x "             << x             << std::endl
    	    << "y "             << y             << std::endl
    	    << "z "             << z             << std::endl
    	    << "phi "           << phi           << std::endl
    	    << "theta "         << theta         << std::endl
    	    << "psi "           << psi           << std::endl
    	    << "axisX "         << axisX         << std::endl
    	    << "axisY "         << axisY         << std::endl
    	    << "axisZ "         << axisZ         << std::endl
    	    << "angle "         << angle         << std::endl
	    << "axisAngle "     << axisAngle     << std::endl
    	    << "apertureMode "  << apertureModel << std::endl
	    << "shape "         << shape         << std::endl
	    << "aper1 "         << aper1         << std::endl
	    << "aper2 "         << aper2         << std::endl
	    << "aper3 "         << aper3         << std::endl
	    << "aper4 "         << aper4         << std::endl
	    << "startAnglePhi " << startAnglePhi << std::endl
	    << "sweepAnglePhi " << sweepAnglePhi << std::endl
	    << "startAngleTheta " << startAngleTheta << std::endl
	    << "sweepAngleTheta " << sweepAngleTheta << std::endl;
}
