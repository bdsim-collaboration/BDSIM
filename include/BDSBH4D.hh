//
// Created by Eliott Ramoisiaux on 05/06/2020.
//

#ifndef ROOT_TEST_BDSBH4D_HH
#define ROOT_TEST_BDSBH4D_HH

#include <boost/histogram.hpp>

#include "Rtypes.h"
#include "TObject.h"

typedef boost::histogram::histogram<std::__1::tuple<boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default>, boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default>, boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default>, boost::histogram::axis::regular<double, boost::histogram::axis::transform::log, boost::use_default, boost::use_default> >, boost::histogram::storage_adaptor<std::__1::vector<double, std::__1::allocator<double> > > > boost_histogram;

class BDSBH4D : public TObject {

public:
    BDSBH4D();
    virtual ~BDSBH4D() {;}

    boost_histogram h;

ClassDef(BDSBH4D,1);

};


#endif //ROOT_TEST_BDSBH4D_HH
