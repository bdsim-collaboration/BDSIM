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

class BDSPTCOneTurnMap {
public:
  //! Default constructor
  BDSPTCOneTurnMap() = delete;

  //! Copy constructor
  BDSPTCOneTurnMap(const BDSPTCOneTurnMap &other) = default;

  //! Move constructor
  BDSPTCOneTurnMap(BDSPTCOneTurnMap &&other) noexcept = default;

  //! Destructor
  virtual ~BDSPTCOneTurnMap() noexcept = default;

  //! Copy assignment operator
  BDSPTCOneTurnMap &operator=(const BDSPTCOneTurnMap &other) = default;

  //! Move assignment operator
  BDSPTCOneTurnMap &operator=(BDSPTCOneTurnMap &&other) noexcept = default;

  BDSPTCOneTurnMap(std::string path); // path to maptable file

  // public-facing should_apply?
  // This should be used by the terminator to decide if the teleporter
  // should use this instance (one turn map)
  bool is_applicable();

  void set_turn_result(double x, double px, double y, double py,
		       double delta_p);
  // This should be used by the teleporter.
  void get_turn_result(double &x, double &px, double &y, double &py,
		       double &delta_p);

private:
  double evaluate(std::vector<PTCMapTerm> terms, double x, double px, double y,
                  double py, double delta_p);

  bool should_apply; // Should we apply this map ?  This is intended
                     // to be set by the terminator (primary?  S0 != 0
                     // and first turn?) and read by the teleporter.

  // Evaluate this one turn map for the given coordinates.
  double evaluate_x(double x, double px, double y, double py, double delta_p);
  double evaluate_px(double x, double px, double y, double py, double delta_p);
  double evaluate_y(double x, double px, double y, double py, double delta_p);
  double evaluate_py(double x, double px, double y, double py, double delta_p);
  double evaluate_delta_p(double x, double px, double y, double py,
                          double delta_p);

  std::vector<PTCMapTerm> x_terms;
  std::vector<PTCMapTerm> y_terms;
  std::vector<PTCMapTerm> px_terms;
  std::vector<PTCMapTerm> py_terms;
  std::vector<PTCMapTerm> delta_p_terms;
};

#endif /* BDSPTCMAP_H */
