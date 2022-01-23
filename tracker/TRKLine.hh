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
#ifndef TRKLine_h
#define TRKLine_h

#include <string>
#include <vector>

class TRKElement;

typedef std::vector<double>::const_iterator TRKLineConstSIter;

/**
 * @brief beamline
 */
class TRKLine {
private:
  using Elements = std::vector<TRKElement*>;
  using ElementEnds = std::vector<double>;

  Elements elements;
  ElementEnds elementEndS;

  std::string name;               ///< name of element
  bool circular;                  ///< circular flag for rings
  
public:
  using iterator = Elements::iterator;
  using const_iterator = Elements::const_iterator;

  
  TRKLine() = delete;
  explicit TRKLine(std::string name, bool circular=false);

  std::string GetName() const { return name; }
  bool GetCircular() const { return circular; }

  /// Append TRKElement to Line
  void AddElement(TRKElement *e);
  void AddSingleElement(TRKElement *e);

  /// Find first element with name eName
  // const_iterator FindElement(std::string eName) const;
  const_iterator FindElement(double s) const;

  iterator begin() { return elements.begin(); }
  iterator end() { return elements.end(); }
  const_iterator begin() const { return elements.begin(); }
  const_iterator end() const { return elements.end(); }
  const_iterator cbegin() const { return elements.cbegin(); }
  const_iterator cend() const { return elements.cend(); }
  TRKLineConstSIter beginS()const {return elementEndS.begin();}
  TRKLineConstSIter endS()const {return elementEndS.end();}

};

#endif
