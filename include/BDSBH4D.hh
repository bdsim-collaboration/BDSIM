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
#ifndef BDSBH4D_HH
#define BDSBH4D_HH

#include "BDSBH4DBase.hh"
#include "BDSBH4DTypedefs.hh"

#ifdef USE_BOOST
#include <boost/histogram.hpp>
#endif

#include "Rtypes.h"
#include "TH1D.h"
#include "TTree.h"

/** @brief 4D histogram class.
 *
 * @author Eliott Ramoisiaux
 */

template<class T>
class BDSBH4D : public BDSBH4DBase {
public:
    BDSBH4D();
    BDSBH4D(std::string& name, std::string& title, const std::string& eScale,
            unsigned int nxbins, double xmin, double xmax,
            unsigned int nybins, double ymin, double ymax,
            unsigned int nzbins, double zmin, double zmax,
            unsigned int nebins, double emin, double emax);
    BDSBH4D(std::string& name, std::string& title, const std::string& eScale, std::vector<double> eBinsEdges,
            unsigned int nxbins, double xmin, double xmax,
            unsigned int nybins, double ymin, double ymax,
            unsigned int nzbins, double zmin, double zmax);

    T h;
    T h_err;

    BDSBH4DBase& operator+=(const BDSBH4DBase& other) override {
      h += dynamic_cast<const BDSBH4D<T>&>(other).h;
      return *this;
    }
    void Initialize(std::string& name, std::string& title, const std::string& eScale,
                                unsigned int nxbins, double xmin, double xmax,
                                unsigned int nybins, double ymin, double ymax,
                                unsigned int nzbins, double zmin, double zmax,
                                unsigned int nebins, double emin, double emax);
    void Initialize(std::string& name, std::string& title, const std::string& eScale,std::vector<double> eBinsEdges,
                                unsigned int nxbins, double xmin, double xmax,
                                unsigned int nybins, double ymin, double ymax,
                                unsigned int nzbins, double zmin, double zmax);
    void to_PyROOT(const std::string&, const std::string&);
    void Reset_BDSBH4D() override;
    BDSBH4D* Clone(const char*) const override;
    void Fill_BDSBH4D(double, double, double, double) override;
    void Set_BDSBH4D(int, int, int, int, double) override;
    void SetError_BDSBH4D(int, int, int, int, double) override;
    void Add_BDSBH4D(BDSBH4DBase*) override;
    double At(int, int, int, int) override;
    double AtError(int, int, int, int) override;
    double LowBinEdgeAt(int, int, int, int) override;
    double HighBinEdgeAt(int, int, int, int) override;
    void Print_BDSBH4D(bool with_zero_values = true) override;
    void Print_BDSBH4D(int , int , int , int) override;

ClassDef(BDSBH4D,1);

};

#endif // BDSBH4D_HH
