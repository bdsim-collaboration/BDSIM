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
#include <iostream>
#include <fstream>
#include <string>

void ProcessFile(std::string filename, std::string prepend);

int main ()
{
  std::string fn = "/Users/simonwilliams/Documents/BDSIM Intern/Dipole/dipole.gdml";
  std::string name = "40";
  ProcessFile(fn, name);
  return 0;
}

std::string FileName(std::string str)
{
  std::size_t found = str.find_last_of(".");
  std::string name = str.substr(0, found);
  return name;
}

void ProcessFile(std::string filename, std::string prepend)
{
  // open input file
  std::ifstream ifstr(filename);

  // check file opened
  
  // prepare output file name
  std::string name = FileName(filename);
  
  // open output file
  std::ofstream fout(name + "_param.gdml");
  
  // loop over and replace
  std::string buffer;
  while (std::getline(ifstr, buffer))
    {
      int e  = buffer.find("variable");
      
      if (e != -1)
	{
	  int f   = buffer.find("wg_outerd");
      
	  if((f != -1) && (f < 36))
	    {
	      int g = buffer.find_last_of("=") + 2;
	      int h = buffer.find_last_of("/") - 1;
	      int i = h - g;
	      std::cout << g << " " << buffer << std::endl;
	      std::string outputString = std::string(buffer);
	      outputString.replace(g, i, prepend);
	      
	      fout << outputString << "\n";
	    }
	  else
	    {
	      std::cout << buffer << std::endl;
	      
	      fout << buffer << "\n";
	    }
	}
      else
	{
	  std::cout << buffer << std::endl;
	  
	  fout << buffer << "\n";
	}
      
    }

  // clean up
  fout.close();
}
