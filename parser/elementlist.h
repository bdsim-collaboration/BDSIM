#ifndef __ELEMENTLIST_H
#define __ELEMENTLIST_H

#include <list>
#include <map>
#include <string>

#include "element.h"

/**
 * @brief Element List with Efficient Lookup
 * 
 * This class keeps a list of Elements
 * It has efficient lookup on an Element's name (log n) by having a multimap 
 *
 * Used for beamline
 *
 * If needed it is fairly easy to template Element
 *
 * @author Jochem Snuverink <Jochem.Snuverink@rhul.ac.uk>
 */

class ElementList {

 public:
  /// for ease of reading
  // chosen not to distinguish between non-const and const cases
  typedef std::list<Element>::iterator ElementListIterator;
  //typedef std::list<Element>::InputIterator ElementListInputIterator;
  typedef std::multimap<std::string, ElementListIterator>::iterator ElementMapIterator;

  ///@{
  /// insert options (classic list inserts):
  /// inputiterator templated to account for reverse iterators
  template <typename ElementListInputIterator>
  ElementListIterator insert (ElementListInputIterator position, const Element& val);
  template <typename ElementListInputIterator>
  void insert (ElementListIterator position, ElementListInputIterator first, ElementListInputIterator last);
  ///@}
  /// insert element at end of list
  /// option to check for unique element name (exits in case name is not unique), false by default
  void push_back(Element& el, bool unique=false);
  
  /// size of list
  int size()const;
  /// empty lists
  void clear();
  ///@{ erase elements
  void erase();
  ElementListIterator erase (ElementListIterator position);
  ElementListIterator erase (ElementListIterator first, ElementListIterator last);
  ///@}
  /// begin/end iterator:
  ElementListIterator begin();
  ElementListIterator end();

  /// lookup method, returns iterator of list pointing to Element with name
  // TOOD: better list of iterators?
  ElementListIterator find(std::string name,unsigned int count=1);

  /// print method
  void print(int ident=0);

 private:
  /// the Element list
  std::list<Element> itsList;
  /// multimap for name lookup
  std::multimap<std::string, ElementListIterator> itsMap;
};

/// template definitions need to be in header
template <typename ElementListInputIterator>
ElementList::ElementListIterator ElementList::insert(ElementListInputIterator position, const Element& val) {
  // insert in list
  ElementListIterator it = itsList.insert(position,val);
  // insert iterator in map with key name
  itsMap.insert(std::pair<std::string,ElementListIterator>(val.name,it));
  return it;
}

template <typename ElementListInputIterator>
void ElementList::insert(ElementListIterator position, ElementListInputIterator first, ElementListInputIterator last) {
  for (;first!=last; ++first) {
    // insert one by one before position
    ElementList::insert(position,*first);
  }
}

extern ElementList beamline_list;
extern std::list<Element> material_list;
extern std::list<Element> atom_list;

#endif
