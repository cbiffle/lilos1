#ifndef LILOS_LIST_HH_
#define LILOS_LIST_HH_

#include <stddef.h>

namespace lilos {

/*
 * A singly-linked list, intended for use in wait lists.
 */

namespace list {

template <typename Elt>
class List {
public:
  class Item {
    Elt _value;
    Item *_next;
  public:
    inline Item(Elt value) : _value(value), _next(0) {}
    inline Item *next() { return _next; }
  };

private:
  Item *_head;

public:
  inline List() : _head(0) {}

  inline void insert(Item *it) {
    it->next = _head;
    _head = it;
  }

  inline Item *head() { return _head; }
};

}  // namespace list
}  // namespace lilos

#endif  // LILOS_LIST_HH_
