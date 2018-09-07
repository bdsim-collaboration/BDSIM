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
#ifndef BDSPTCMAP_H
#define BDSPTCMAP_H

#include <vector>

namespace {
struct PTCMapTerm {
  double coefficient;
  int nx, npx, ny, npy, ndelta_p;
};

} // namespace

class BDSPTCMap {
public:
  //! Default constructor
  BDSPTCMap() = delete;

  //! Copy constructor
  BDSPTCMap(const BDSPTCMap &other) = default;

  //! Move constructor
  BDSPTCMap(BDSPTCMap &&other) noexcept = default;

  //! Destructor
  virtual ~BDSPTCMap() noexcept = default;

  //! Copy assignment operator
  BDSPTCMap &operator=(const BDSPTCMap &other) = default;

  //! Move assignment operator
  BDSPTCMap &operator=(BDSPTCMap &&other) noexcept = default;

  BDSPTCMap(std::string path); // path to maptable file
  double evaluate_x(double x, double px, double y, double py, double delta_p);
  double evaluate_px(double x, double px, double y, double py, double delta_p);
  double evaluate_y(double x, double px, double y, double py, double delta_p);
  double evaluate_py(double x, double px, double y, double py, double delta_p);
  double evaluate_delta_p(double x, double px, double y, double py,
                          double delta_p);

private:
  double evaluate(std::vector<PTCMapTerm> terms, double x, double px, double y,
                  double py, double delta_p);

  std::vector<PTCMapTerm> x_terms;
  std::vector<PTCMapTerm> y_terms;
  std::vector<PTCMapTerm> px_terms;
  std::vector<PTCMapTerm> py_terms;
  std::vector<PTCMapTerm> delta_p_terms;
};

#endif /* BDSPTCMAP_H */
