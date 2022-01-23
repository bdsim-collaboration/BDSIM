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
#ifndef TRKElementLine_h
#define TRKElementLine_h

#include <string>
#include <vector>
#include "TRKElement.hh"


/**
 * @brief A sequence of TRKElements used only during construction time. Different from the beamline class (TRKLine).
 * Based on BDSLine.
 */

class TRKElementLine: public TRKElement
{
private:
    typedef std::vector<TRKElement*> TRKLineVector;

    /// Member vector to store components
    TRKLineVector line;

public:
    explicit TRKElementLine(std::string name);

    /// Add a component to the line.
    void AddElement(TRKElement* component);

    /// Iterator mechanics
    typedef TRKLineVector::iterator       iterator;
    typedef TRKLineVector::const_iterator const_iterator;
    iterator       begin()       {return line.begin();}
    iterator       end()         {return line.end();}
    const_iterator begin() const {return line.begin();}
    const_iterator end()   const {return line.end();}
    bool           empty() const {return line.empty();}

    /// Size of line
    size_t size() const {return line.size();}


private:
    /// Private default constructor to force the use of the supplied one.
    TRKElementLine() = delete;

    /// @{ Assignment and copy constructor not implemented nor used.
    TRKElementLine& operator=(const TRKElementLine&) = delete;
    TRKElementLine(TRKElementLine&) = delete;

};

#endif
