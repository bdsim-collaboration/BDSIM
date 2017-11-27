/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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

//typedef std::vector<TRKElement*>::iterator TRKLineIter;
typedef std::vector<TRKElement*>::const_iterator TRKLineConstIter;

/**
 * @brief beamline
 */
class TRKLine {
private: 
  // in case we use FindElement a lot, probably good to make an index also (but something for later)
  std::vector<TRKElement*> elements;
  std::string name;               ///< name of element
  bool circular;                  ///< circular flag for rings
  
public:
  explicit TRKLine(std::string name, bool circular=false);
  ~TRKLine();
  /// Append TRKElement to Line
  void AddElement(TRKElement *e);
  /// Find first element with name eName
  TRKElement* FindElement(std::string eName)const;

  std::string GetName()const{return name;}
  bool GetCircular()const{return circular;}

  // TRKLineIter begin() {return elements.begin();}
  // TRKLineIter end() {return elements.end();}

  TRKLineConstIter begin()const {return elements.begin();}
  TRKLineConstIter end()const {return elements.end();}

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKLine &element);

private:
  TRKLine(); ///< not implemented
};

#endif
