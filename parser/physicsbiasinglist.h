#ifndef __PHYSICSBIASINGLIST_H
#define __PHYSICSBIASINGLIST_H

#include <vector>
#include <string> 
#include <map>

#include "physicsbiasing.h"

namespace GMAD {
class PhysicsBiasingList {

 public: 
  typedef std::list<PhysicsBiasing>::iterator PhysicsBiasingListIterator; 
  typedef std::multimap<std::string, PhysicsBiasingListIterator>::iterator PhysicsBiasingMapIterator;

  ///@{
  template <typename PhysicsBiasingListInputIterator> 
    PhysicsBiasingListIterator insert(PhysicsBiasingListInputIterator position, const PhysicsBiasing& val);
  template <typename PhysicsBiasingListInputIterator> 
    void insert(PhysicsBiasingListInputIterator position,
		PhysicsBiasingListInputIterator first,
		PhysicsBiasingListInputIterator last);
  ///}
  /// insert element at end of list 
  void push_back(PhysicsBiasing& pb, bool unique=false); 

  /// size of list 
  int size() const; 
  /// empty lists 
  void clear();
  ///@{
  void erase();
  PhysicsBiasingListIterator erase(PhysicsBiasingListIterator position);
  PhysicsBiasingListIterator erase(PhysicsBiasingListIterator first, PhysicsBiasingListIterator last); 
  ///@}
  PhysicsBiasingListIterator begin();
  PhysicsBiasingListIterator end();
  
  PhysicsBiasingListIterator findByName(std::string name, unsigned int count=1);
  PhysicsBiasing findByVolume(std::string name);

 private: 
  /// the physics biasing list
  std::list<PhysicsBiasing> itsList;
  /// multimap for name lookup
  std::multimap<std::string, PhysicsBiasingListIterator> itsMap;
  /// multimap for logical volume lookup
  std::multimap<std::string, PhysicsBiasingListIterator> itsLogVolMap;
};

 template<typename PhysicsBiasingListInputIterator> 
   PhysicsBiasingList::PhysicsBiasingListIterator PhysicsBiasingList::insert(PhysicsBiasingListInputIterator position, 
									     const PhysicsBiasing& val) {
   PhysicsBiasingListIterator it = itsList.insert(position,val);
   itsMap.insert(std::pair<std::string,PhysicsBiasingListIterator>(val.name,it));
 }
 
 template<typename PhysicsBiasingListInputIterator> 
   void PhysicsBiasingList::insert(PhysicsBiasingListInputIterator position, PhysicsBiasingListInputIterator first, 
				   PhysicsBiasingListInputIterator last) {
   
   for(;first!=last; ++first) {
     PhysicsBiasingList::insert(position,*first);
   }
 }
 
 extern PhysicsBiasingList xsecbias_list;
 
}
#endif

