#include "elementlist.h"

#include <cstdio>

extern const char* current_line;
// extern const int VERBOSE;

void ElementList::push_back(Element& el) {
  // insert at back of list (insert() instead of push_back() to get iterator for map):
  ElementListIterator it = itsList.insert(end(),el);
  itsMap.insert(std::pair<std::string,ElementListIterator>(el.name,it));
}

int ElementList::size()const {
  return itsList.size();
}

void ElementList::clear() {
  itsList.clear();
  itsMap.clear();
}

ElementList::ElementListIterator ElementList::erase(ElementListIterator it) {

  // find entry in map to erase:
  std::string name = (*it).name;
  if (itsMap.count(name) == 1) {
    itsMap.erase(name);
  }
  else { // more than one entry with same name 
    std::pair<ElementMapIterator,ElementMapIterator> ret = itsMap.equal_range(name);
    for (ElementMapIterator emit = ret.first; emit!=ret.second; ++emit) {
      if ((*emit).second == it) // valid comparison? if not, how to find correct element?
      {
	itsMap.erase(emit);
	break;
      }
    }
  }
  return itsList.erase(it);
}

ElementList::ElementListIterator ElementList::erase(ElementListIterator first, ElementListIterator last) {
  ElementListIterator it=first;
  while (it!=last) {
    // erase one by one
    it = erase(it);
  }
  return it;
}

ElementList::ElementListIterator ElementList::begin() {
  return itsList.begin();
}

ElementList::ElementListIterator ElementList::end() {
  return itsList.end();
}

ElementList::ElementListIterator ElementList::back() {
  return itsList.end()--;
}

ElementList::ElementListIterator ElementList::find(std::string name,unsigned int count) {
  if (count==1) {
    ElementMapIterator emit = itsMap.find(name);
    if (emit==itsMap.end()) return itsList.end();
    return (*emit).second;
  } else {
    // if count > 1
    std::pair<ElementMapIterator,ElementMapIterator> ret = itsMap.equal_range(name);
    unsigned int i=1;
    for (ElementMapIterator emit = ret.first; emit!=ret.second; ++emit, i++) {
      if (i==count) {
	return (*emit).second;
      }
    }
    return itsList.end();
  }
}

void ElementList::print(int ident) {
  //  if(VERBOSE) 
  if(ident == 0) printf("using line %s\n",current_line);

  for(ElementListIterator it=begin();it!=end();it++)
    {
      (*it).print(ident);
    }
}
