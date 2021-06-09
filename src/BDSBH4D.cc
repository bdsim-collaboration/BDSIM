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
#include "BDSBH4D.hh"

#include "TFile.h"

#ifdef USE_BOOST
#include "BDSBH4DTypeDefs.hh"
#include <boost/format.hpp>
#include <boost/histogram.hpp>
#endif

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifdef USE_BOOST
templateClassImp(BDSBH4D)

template <>
BDSBH4D<boost_histogram_linear>::BDSBH4D()
{
  h = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {3, 0.0, 1.0, "x"},
					    boost::histogram::axis::regular<double> {3, 0.0, 1.0, "y"},
					    boost::histogram::axis::regular<double> {3, 0.0, 1.0, "z"},
					    boost::histogram::axis::regular<double> {3, 1.0, 230.0, "energy"});
  
  h_err = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {3, 0.0, 1.0, "x"},
						boost::histogram::axis::regular<double> {3, 0.0, 1.0, "y"},
						boost::histogram::axis::regular<double> {3, 0.0, 1.0, "z"},
						boost::histogram::axis::regular<double> {3, 1.0, 230.0, "energy"});
  
  Initialize("BDSBH4D", "BDSBH4D", "linear", 3, 0, 1, 3, 0, 1, 3, 0, 1, 3, 0.001, 0.230);  
}

template <>
BDSBH4D<boost_histogram_log>::BDSBH4D()
{
  h = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {3, 0.0, 1.0, "x"},
					    boost::histogram::axis::regular<double> {3, 0.0, 1.0, "y"},
					    boost::histogram::axis::regular<double> {3, 0.0, 1.0, "z"},
					    boost::histogram::axis::regular<double, boost::histogram::axis::transform::log> {3, 1.0, 230.0, "energy"});
  
  h_err = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {3, 0.0, 1.0, "x"},
						boost::histogram::axis::regular<double> {3, 0.0, 1.0, "y"},
						boost::histogram::axis::regular<double> {3, 0.0, 1.0, "z"},
						boost::histogram::axis::regular<double, boost::histogram::axis::transform::log> {3, 1.0, 230.0, "energy"});
  
  Initialize("BDSBH4D", "BDSBH4D", "log", 3, 0, 1, 3, 0, 1, 3, 0, 1, 3, 0.001, 0.230);  
}

template <>
BDSBH4D<boost_histogram_variable>::BDSBH4D()
{
  h = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {3, 0.0, 1.0, "x"},
					    boost::histogram::axis::regular<double> {3, 0.0, 1.0, "y"},
					    boost::histogram::axis::regular<double> {3, 0.0, 1.0, "z"},
					    boost::histogram::axis::variable<double> {std::vector<double>{0.001,0.230}, "energy"});
  
  h_err = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {3, 0.0, 1.0, "x"},
						boost::histogram::axis::regular<double> {3, 0.0, 1.0, "y"},
						boost::histogram::axis::regular<double> {3, 0.0, 1.0, "z"},
						boost::histogram::axis::variable<double> {std::vector<double>{0.001,0.1,0.230}, "energy"});
  
  Initialize("BDSBH4D", "BDSBH4D", "user", std::vector<double>{0.001,0.1,0.230}, 3, 0, 1, 3, 0, 1, 3, 0, 1);
}

template <>
BDSBH4D<boost_histogram_linear>::BDSBH4D(std::string& name, std::string& title, const std::string& eScale,
					 unsigned int nxbins, double xmin, double xmax,
					 unsigned int nybins, double ymin, double ymax,
					 unsigned int nzbins, double zmin, double zmax,
					 unsigned int nebins, double emin, double emax)
{
  h = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {nxbins, xmin, xmax, "x"},
					    boost::histogram::axis::regular<double> {nybins, ymin, ymax, "y"},
					    boost::histogram::axis::regular<double> {nzbins, zmin, zmax, "z"},
					    boost::histogram::axis::regular<double> {nebins, emin, emax, "energy"});
  
  h_err = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {nxbins, xmin, xmax, "x"},
						boost::histogram::axis::regular<double> {nybins, ymin, ymax, "y"},
						boost::histogram::axis::regular<double> {nzbins, zmin, zmax, "z"},
						boost::histogram::axis::regular<double> {nebins, emin, emax, "energy"});
  
  Initialize(name, title, eScale, nxbins, xmin, xmax, nybins, ymin, ymax, nzbins, zmin, zmax, nebins, emin, emax); 
}

template <>
BDSBH4D<boost_histogram_log>::BDSBH4D(std::string& name, std::string& title, const std::string& eScale,
                    unsigned int nxbins, double xmin, double xmax,
                    unsigned int nybins, double ymin, double ymax,
                    unsigned int nzbins, double zmin, double zmax,
                    unsigned int nebins, double emin, double emax)
{
  h = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {nxbins, xmin, xmax, "x"},
					    boost::histogram::axis::regular<double> {nybins, ymin, ymax, "y"},
					    boost::histogram::axis::regular<double> {nzbins, zmin, zmax, "z"},
					    boost::histogram::axis::regular<double, boost::histogram::axis::transform::log> {nebins, emin, emax, "energy"});
  
  h_err = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {nxbins, xmin, xmax, "x"},
						boost::histogram::axis::regular<double> {nybins, ymin, ymax, "y"},
						boost::histogram::axis::regular<double> {nzbins, zmin, zmax, "z"},
						boost::histogram::axis::regular<double, boost::histogram::axis::transform::log> {nebins, emin, emax, "energy"});
  
  Initialize(name, title, eScale, nxbins, xmin, xmax, nybins, ymin, ymax, nzbins, zmin, zmax, nebins, emin, emax); 
}

template <>
BDSBH4D<boost_histogram_variable>::BDSBH4D(std::string& name, std::string& title, const std::string& eScale,
					   std::vector<double> eBinsEdges,
					   unsigned int nxbins, double xmin, double xmax,
					   unsigned int nybins, double ymin, double ymax,
					   unsigned int nzbins, double zmin, double zmax)
{
  h = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {nxbins, xmin, xmax, "x"},
					    boost::histogram::axis::regular<double> {nybins, ymin, ymax, "y"},
					    boost::histogram::axis::regular<double> {nzbins, zmin, zmax, "z"},
					    boost::histogram::axis::variable<double> {eBinsEdges, "energy"});
  
  h_err = boost::histogram::make_histogram_with(std::vector<double>(),boost::histogram::axis::regular<double> {nxbins, xmin, xmax, "x"},
						boost::histogram::axis::regular<double> {nybins, ymin, ymax, "y"},
						boost::histogram::axis::regular<double> {nzbins, zmin, zmax, "z"},
						boost::histogram::axis::variable<double> {eBinsEdges, "energy"});
  
  Initialize(name, title, eScale,eBinsEdges, nxbins, xmin, xmax, nybins, ymin, ymax, nzbins, zmin, zmax); 
}

template<class T>
BDSBH4D<T>::~BDSBH4D()
{;}

template <class T>
void BDSBH4D<T>::Initialize(std::string& name, std::string& title, const std::string& eScale,
                            unsigned int nxbins, double xmin, double xmax,
                            unsigned int nybins, double ymin, double ymax,
                            unsigned int nzbins, double zmin, double zmax,
                            unsigned int nebins, double emin, double emax){
  h_nxbins = nxbins;
  h_nybins = nybins;
  h_nzbins = nzbins;
  h_nebins = nebins;
  h_xmin = xmin;
  h_xmax = xmax;
  h_ymin = ymin;
  h_ymax = ymax;
  h_zmin = zmin;
  h_zmax = zmax;
  h_emin = emin;
  h_emax = emax;
  h_name = name;
  h_title = title;
  h_escale = eScale;
}

template <class T>
void BDSBH4D<T>::Initialize(std::string& name, std::string& title, const std::string& eScale,
			    std::vector<double> eBinsEdges,
                            unsigned int nxbins, double xmin, double xmax,
                            unsigned int nybins, double ymin, double ymax,
                            unsigned int nzbins, double zmin, double zmax){
  h_nxbins = nxbins;
  h_nybins = nybins;
  h_nzbins = nzbins;
  h_nebins = eBinsEdges.size()-1;
  h_xmin = xmin;
  h_xmax = xmax;
  h_ymin = ymin;
  h_ymax = ymax;
  h_zmin = zmin;
  h_zmax = zmax;
  h_emin = eBinsEdges[0];
  h_emax = eBinsEdges[h_nebins];
  h_name = name;
  h_title = title;
  h_escale = eScale;
  h_ebinsedges = eBinsEdges;
}

template <class T>
void BDSBH4D<T>::to_PyROOT(const std::string& filename, const std::string& histo_name)
{
  const char* filename_char = filename.c_str();
  
  std::string path ="Event/MergedHistograms/" +histo_name;
  const char* path_char = path.c_str();
  
  TFile *_file0 = TFile::Open(filename_char);
  auto* tree = (TTree*) _file0->Get(path_char);
  auto* Boost_histogram = new BDSBH4D<T>();
  tree->SetBranchAddress("BDSBH4DBase",&Boost_histogram);
  tree->GetEntry(0);
  
  this->h = Boost_histogram->h;
  this->h_err = Boost_histogram->h_err;
  this->h_nxbins = Boost_histogram->h_nxbins;
  this->h_nybins = Boost_histogram->h_nybins;
  this->h_nzbins = Boost_histogram->h_nzbins;
  this->h_nebins = Boost_histogram->h_nebins;
  this->h_xmin = Boost_histogram->h_xmin;
  this->h_xmax = Boost_histogram->h_xmax;
  this->h_ymin = Boost_histogram->h_ymin;
  this->h_ymax = Boost_histogram->h_ymax;
  this->h_zmin = Boost_histogram->h_zmin;
  this->h_zmax = Boost_histogram->h_zmax;
  this->h_emin = Boost_histogram->h_emin;
  this->h_emax = Boost_histogram->h_emax;
  this->h_name = Boost_histogram->h_name;
  this->h_title = Boost_histogram->h_title;
  this->h_escale = Boost_histogram->h_escale;
  this->h_ebinsedges = Boost_histogram->h_ebinsedges;  
}

template <class T>
void BDSBH4D<T>::Reset_BDSBH4D()
{
  h.reset();
  h_entries = 0;
}

template <class T>
BDSBH4D<T>* BDSBH4D<T>::Clone(const char* newname) const
{
  auto clone = new BDSBH4D<T>(*this);
  clone->SetName(newname);
  return clone;
}

template <class T>
void BDSBH4D<T>::Fill_BDSBH4D(double xValue,
			      double yValue,
			      double zValue,
			      double eValue)
{
  h(xValue, yValue, zValue, eValue);
}

template <class T>
void BDSBH4D<T>::Set_BDSBH4D(int x,
			     int y,
			     int z,
			     int e,
			     double value)
{
  h.at(x, y, z, e) = value;
}

template <class T>
void BDSBH4D<T>::SetError_BDSBH4D(int x,
				  int y,
				  int z,
				  int e,
				  double value)
{
  h_err.at(x, y, z, e) = value;
}

template <class T>
void BDSBH4D<T>::Add_BDSBH4D(BDSBH4DBase* otherHistogram)
{
  auto tmp = dynamic_cast<BDSBH4D<T>*>(otherHistogram);
  h += tmp->h;
}

template <class T>
double BDSBH4D<T>::At(int x, int y, int z, int e)
{
  return h.at(x, y, z, e);
}

template <class T>
double BDSBH4D<T>::LowBinEdgeAt(int x, int y, int z, int e)
{
  std::ostringstream os4;
  for (auto&& i : indexed(h))
    {
      if (i.index(0) == x and i.index(1) == y and i.index(2) == z and i.index(3) == e)
	{return i.bin(3).lower();}
    }
  return 0;
}

template <class T>
double BDSBH4D<T>::HighBinEdgeAt(int x, int y, int z, int e)
{
  std::ostringstream os4;
  for (auto&& i : indexed(h))
    {
      if (i.index(0) == x and i.index(1) == y and i.index(2) == z and i.index(3) == e)
	{return i.bin(3).upper();}
    }
  return 0;
}

template <class T>
double BDSBH4D<T>::AtError(int x, int y, int z, int e)
{
  return h_err.at(x, y, z, e);
}

template <class T>
void BDSBH4D<T>::Print_BDSBH4D(bool with_zero_values)
{
  std::ostringstream os4;
  for (auto&& x : indexed(this->h))
    {
      if (!with_zero_values and *x != 0)
	{
	  os4 << boost::format("(%i, %i, %i, %i) [%.3f, %.3f) [%.3f, %.3f) [%.3f, %.3f) [%.3f, %.3f) %i\n")
	    % x.index(0) % x.index(1) % x.index(2) % x.index(3)
	    % x.bin(0).lower() % x.bin(0).upper()
	    % x.bin(1).lower() % x.bin(1).upper()
	    % x.bin(2).lower() % x.bin(2).upper()
	    % x.bin(3).lower() % x.bin(3).upper()
	    % *x;
        }
      else if (with_zero_values)
	{
	  os4 << boost::format("(%i, %i, %i, %i) [%.3f, %.3f) [%.3f, %.3f) [%.3f, %.3f) [%.3f, %.3f) %i\n")
	    % x.index(0) % x.index(1) % x.index(2) % x.index(3)
	    % x.bin(0).lower() % x.bin(0).upper()
	    % x.bin(1).lower() % x.bin(1).upper()
	    % x.bin(2).lower() % x.bin(2).upper()
	    % x.bin(3).lower() % x.bin(3).upper()
	    % *x;	  
        }
    }
    std::cout << os4.str() << std::flush;
}

template <class T>
void BDSBH4D<T>::Print_BDSBH4D(int x, int y, int z, int e)
{
  std::ostringstream os4;
  for (auto&& i : indexed(this->h))
    {
      if (i.index(0) == x and  i.index(1) == y and i.index(2) ==z and i.index(3) ==e)
	{
	  os4 << boost::format("(%i, %i, %i, %i) [%.3f, %.3f) [%.3f, %.3f) [%.3f, %.3f) [%.3f, %.3f) %i\n")
	    % i.index(0) % i.index(1) % i.index(2) % i.index(3)
	    % i.bin(0).lower() % i.bin(0).upper()
	    % i.bin(1).lower() % i.bin(1).upper()
	    % i.bin(2).lower() % i.bin(2).upper()
	    % i.bin(3).lower() % i.bin(3).upper()
	    % *i;
        }
    }
  std::cout << os4.str() << std::flush;
}

template class BDSBH4D<boost_histogram_linear>;
template class BDSBH4D<boost_histogram_log>;
template class BDSBH4D<boost_histogram_variable>;

#endif
