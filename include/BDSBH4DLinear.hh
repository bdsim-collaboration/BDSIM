//
// Created by Cedric Hernalsteens on 23/07/2020.
//

#ifndef BDSBH4DLINEAR_HH
#define BDSBH4DLINEAR_HH

#include "BDSBH4D.hh"
#include <boost/histogram.hpp>

#include "Rtypes.h"
#include "TH1D.h"
#include "TTree.h"

typedef boost::histogram::histogram<std::__1::tuple<boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default>, boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default>, boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default>, boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default> >, boost::histogram::storage_adaptor<std::__1::vector<double, std::__1::allocator<double> > > > boost_histogram_linear;

class BDSBH4DLinear : public BDSBH4D {
public:
    BDSBH4DLinear();
    BDSBH4DLinear(std::string& name, std::string& title,
            unsigned int nxbins, double xmin, double xmax,
            unsigned int nybins, double ymin, double ymax,
            unsigned int nzbins, double zmin, double zmax,
            unsigned int nebins, double emin, double emax);

    boost_histogram_linear h;
    boost_histogram_linear h_err;

    void Reset() override;
    BDSBH4DLinear* Clone(const char*) const override;
    void Fill(double, double, double, double) override;
    void Set(int, int, int, int, double) override;
    void SetError(int, int, int, int, double) override;
    double At(int, int, int, int) override;
    double AtError(int, int, int, int) override;

ClassDef(BDSBH4DLinear,1);

};

#endif // BDSBH4DLINEAR_HH
