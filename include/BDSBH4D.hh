//
// Created by Cedric Hernalsteens on 23/07/2020.
//

#ifndef BDSBH4D_HH
#define BDSBH4D_HH

#include "BDSBH4DBase.hh"
#include "BDSBHTypedefs.hh"
#include <boost/histogram.hpp>

#include "Rtypes.h"
#include "TH1D.h"
#include "TTree.h"

template<class T>
class BDSBH4D : public BDSBH4DBase {
public:
    BDSBH4D();
    BDSBH4D(std::string& name, std::string& title, const std::string& eScale,
            unsigned int nxbins, double xmin, double xmax,
            unsigned int nybins, double ymin, double ymax,
            unsigned int nzbins, double zmin, double zmax,
            unsigned int nebins, double emin, double emax);

    T h;
    T h_err;

    BDSBH4DBase& operator+=(const BDSBH4DBase& other) override {
      h += dynamic_cast<const BDSBH4D<T>&>(other).h;
      return *this;
    }
    void to_PyROOT(std::string, std::string);
    void Reset() override;
    BDSBH4D* Clone(const char*) const override;
    void Fill(double, double, double, double) override;
    void Set(int, int, int, int, double) override;
    void SetError(int, int, int, int, double) override;
    void Add(BDSBH4DBase*) override;
    double At(int, int, int, int) override;
    double AtError(int, int, int, int) override;
    double LowBinEdgeAt(int, int, int, int) override;
    double HighBinEdgeAt(int, int, int, int) override;
    void Print(bool show_not_empty = false) override;
    void Print(int , int , int , int) override;

ClassDef(BDSBH4D,1);

};

#endif // BDSBH4D_HH
