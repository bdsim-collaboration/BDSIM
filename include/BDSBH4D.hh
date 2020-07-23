//
// Created by Eliott Ramoisiaux on 05/06/2020.
//

#ifndef BDSBH4D_HH
#define BDSBH4D_HH

#include <boost/variant.hpp>
#include <boost/histogram.hpp>

#include "Rtypes.h"
#include "TH1D.h"
#include "TTree.h"



typedef boost::histogram::histogram<std::__1::tuple<boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default>, boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default>, boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default>, boost::histogram::axis::regular<double, boost::histogram::axis::transform::log, boost::use_default, boost::use_default> >, boost::histogram::storage_adaptor<std::__1::vector<double, std::__1::allocator<double> > > > boost_histogram_Log;
typedef boost::histogram::histogram<std::__1::tuple<boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default>, boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default>, boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default>, boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default> >, boost::histogram::storage_adaptor<std::__1::vector<double, std::__1::allocator<double> > > > boost_histogram_Linear;

class HistogramAtGetVisitor : public boost::static_visitor<> {

public:
    int x, y, z, e;
    double result;
    HistogramAtGetVisitor(const int xx, const int yy, const int zz, const int ee) : x(xx), y(yy), z(zz), e(ee) {};
    template <typename T>
    void operator()(T& operand)
    {
        result =  operand.at( x, y, z, e);
    }

};

class HistogramAtSetVisitor : public boost::static_visitor<> {

public:
    int x, y, z, e, value;
    HistogramAtSetVisitor(const int xx, const int yy, const int zz, const int ee, const double v) : x(xx), y(yy), z(zz), e(ee), value(v) {};
    template <typename T>
    void operator()(T& operand) const
    {
        operand.at( x, y, z, e) = value;
    }

};

class HistogramOperatorParenthesesVisitor : public boost::static_visitor<> {

public:
    int x, y, z, e;
    HistogramOperatorParenthesesVisitor(const int xx, const int yy, const int zz, const int ee) : x(xx), y(yy), z(zz), e(ee) {};
    template <typename T>
    void operator()(T& operand) const
    {
        operand.operator()( x, y, z, e);
    }

};


/*class HistogramPlusEqualVisitor : public boost::static_visitor<> {

    histograms4D[histoId]->h.operator+=(otherHistogram->h);

public:
    otherHistogram = BDSBH4D::BDSBH4D();
    HistogramPlusEqualVisitor(BDSBH4D* ) {};
    template <typename T>
    void operator()(T& operand) const
    {
        return operand.operator+=( );
    }

}; */


class HistogramResetVisitor : public boost::static_visitor<> {

public:
    HistogramResetVisitor(){};
    template <typename T>
    void operator()(T& operand) const
    {
        operand.reset();
    }

};

class BDSBH4D : public TH1D {

public:
    BDSBH4D();
    BDSBH4D(const std::string name, const std::string title,
            unsigned int nxbins, double xmin, double xmax,
            unsigned int nybins, double ymin, double ymax,
            unsigned int nzbins, double zmin, double zmax,
            unsigned int nebins, double emin, double emax);

    void to_PyROOT(std::string filename , std::string path);

    virtual ~BDSBH4D() {;}

    int GetNbinsX();
    int GetNbinsY();
    int GetNbinsZ();
    int GetNbinsE();
    const std::string GetName();
    const std::string GetTitle();

    void SetName(const std::string name);
    void SetTitle(const std::string title);
    void SetEntries(unsigned long i);

    void Reset();

    BDSBH4D* Clone(std::string newname="");

    boost::variant<boost_histogram_Linear,boost_histogram_Log> h;
    boost::variant<boost_histogram_Linear,boost_histogram_Log> h_err;

    unsigned int h_nxbins;
    unsigned int h_nybins;
    unsigned int h_nzbins;
    unsigned int h_nebins;
    double h_xmin;
    double h_xmax;
    double h_ymin;
    double h_ymax;
    double h_zmin;
    double h_zmax;
    double h_emin;
    double h_emax;
    std::string h_name;
    std::string h_title;
    unsigned long entries = 0;

ClassDef(BDSBH4D,1);

};


#endif //BDSBH4D_HH
