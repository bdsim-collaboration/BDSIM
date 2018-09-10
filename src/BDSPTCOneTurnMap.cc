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

#include "BDSPTCOneTurnMap.hh"
#include "BDSDebug.hh"

#include "globals.hh"

#include <cmath>
#include <fstream>
#include <sstream>
#include <string>

BDSPTCOneTurnMap::BDSPTCOneTurnMap(std::string maptable_file) {
  std::ifstream infile(maptable_file);

  // The columns of the maptable TFS (read into below with the stringsteam).
  std::string name = "";
  double coefficient = 0;
  int n_vector = 0;
  int dimensionality = 0;
  int total_order = 0;
  int nx = 0;
  int npx = 0;
  int ny = 0;
  int npy = 0;
  int ndelta_p = 0;
  int nt = 0;

  std::string line = "";
  while (std::getline(infile, line)) {
    if (line.at(0) == '@' || line.at(0) == '*' || line.at(0) == '$') {
      continue;
    }
    std::istringstream stream(line);

    stream >> name >> coefficient >> n_vector >> dimensionality >>
        total_order >> nx >> npx >> ny >> npy >> ndelta_p >> nt;

    PTCMapTerm term{coefficient, nx, npx, ny, npy, ndelta_p};

    switch (n_vector) {
    case 1: {
      x_terms.push_back(term);
      break;
    }
    case 2: {
      px_terms.push_back(term);
      break;
    }
    case 3: {
      y_terms.push_back(term);
      break;
    }
    case 4: {
      py_terms.push_back(term);
      break;
    }
    case 5: {
      delta_p_terms.push_back(term);
      break;
    }
    default:
      std::string message = "Unrecognised PTC term index.  Your maptable file "
                            "is perhaps malformed.";
      G4cerr << __METHOD_NAME__ << message << G4endl;
      exit(1);
      break;
    }
  }
}

double BDSPTCOneTurnMap::evaluate_x(double x, double px, double y, double py,
                             double delta_p) {
  return evaluate(x_terms, x, px, y, py, delta_p);
}

double BDSPTCOneTurnMap::evaluate_px(double x, double px, double y, double py,
                              double delta_p) {
  return evaluate(px_terms, x, px, y, py, delta_p);
}

double BDSPTCOneTurnMap::evaluate_y(double x, double px, double y, double py,
                             double delta_p) {
  return evaluate(y_terms, x, px, y, py, delta_p);
}

double BDSPTCOneTurnMap::evaluate_py(double x, double px, double y, double py,
                              double delta_p) {
  return evaluate(py_terms, x, px, y, py, delta_p);
}

double BDSPTCOneTurnMap::evaluate_delta_p(double x, double px, double y, double py,
                                   double delta_p) {
  return evaluate(delta_p_terms, x, px, y, py, delta_p);
}

double BDSPTCOneTurnMap::evaluate(std::vector<PTCMapTerm> terms, double x, double px,
                           double y, double py, double delta_p) {
  double result = 0;
  for (auto term : terms) {
    result += (term.coefficient * std::pow(x, term.nx) *
               std::pow(px, term.npx) * std::pow(y, term.ny) *
               std::pow(py, term.npy) * std::pow(delta_p, term.ndelta_p));
  }
  return result;
}
