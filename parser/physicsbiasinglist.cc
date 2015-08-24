#include "physicsbiasinglist.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace GMAD;

void PhysicsBiasingList::push_back(PhysicsBiasing &pb, bool unique) { 
  if(unique && itsMap.find(pb.name) != itsMap.end()) { 
    std::cout << "ERROR: element already defined: " << pb.name << std::endl;
    exit(1);    
  }
  // insert at back of list (insert() instead of push_back() to get iterator for map):
  PhysicsBiasingListIterator it = itsList.insert(end(),pb);
  itsMap.insert(std::pair<std::string,PhysicsBiasingListIterator>(pb.name,it));
}

int PhysicsBiasingList::size() const {
  return itsList.size();
}

void PhysicsBiasingList::clear() { 
  itsList.clear();
  itsMap.clear();
}

void PhysicsBiasingList::erase() { 
  clear();
}

PhysicsBiasingList::PhysicsBiasingListIterator PhysicsBiasingList::erase(PhysicsBiasingListIterator it) { 
  // find entry in map to erase:
  std::string name = (*it).name;
  if (itsMap.count(name) == 1) {
    itsMap.erase(name);
  }
  else { // more than one entry with same name 
    std::pair<PhysicsBiasingMapIterator,PhysicsBiasingMapIterator> ret = itsMap.equal_range(name);
    for (PhysicsBiasingMapIterator pbit = ret.first; pbit!=ret.second; ++pbit) {
      if ((*pbit).second == it) // valid comparison? if not, how to find correct element?
      {
	itsMap.erase(pbit);
	break;
      }
    }
  }
  return itsList.erase(it);  
}

PhysicsBiasingList::PhysicsBiasingListIterator PhysicsBiasingList::begin() {
  return itsList.begin();
}

PhysicsBiasingList::PhysicsBiasingListIterator PhysicsBiasingList::end() {
  return itsList.end();
}

PhysicsBiasingList::PhysicsBiasingListIterator PhysicsBiasingList::findByName(std::string name, unsigned int count) {
  if (count==1) {
    PhysicsBiasingMapIterator pbit = itsMap.find(name);
    if(pbit==itsMap.end()) return itsList.end();
    return (*pbit).second;
  }
  else {
    std::pair<PhysicsBiasingMapIterator, PhysicsBiasingMapIterator> ret = itsMap.equal_range(name); 
    unsigned int i=1;
    for (PhysicsBiasingMapIterator pbit = ret.first; pbit!=ret.second; ++pbit, i++) {
      if(i == count) {
	return (*pbit).second;     
      }
    }
    return itsList.end();
  }
}

PhysicsBiasing PhysicsBiasingList::findByVolume(std::string name) { 
  for(auto pbit : itsList) { 
    if(pbit.logicalVolumes == name) 
      return pbit;
  }
  return PhysicsBiasing();
}
