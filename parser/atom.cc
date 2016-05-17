#include "atom.h"

using namespace GMAD;

Atom::Atom()
{
  clear();
  PublishMembers();
}

void Atom::clear()
{
  name = "";
  A = 0.0;
  Z = 0.0;
  symbol = "";
}

void Atom::PublishMembers()
{
  publish("name",   &Atom::name);
  publish("A",      &Atom::A);
  publish("Z",      &Atom::Z);
  publish("symbol", &Atom::symbol);
}

void Atom::print()const
{
  std::cout << "atom: "
	    << name   << " "
	    << A      << " "
	    << Z      << " "
	    << symbol << " "
	    << std::endl;
}
