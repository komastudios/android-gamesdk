#ifndef AGDC_ANCER_INTERNALLIST_H_
#define AGDC_ANCER_INTERNALLIST_H_

namespace ancer {

class InternalList {
 public:
  inline InternalList() : next(this), prev(this) { }
  inline ~InternalList() {
    Remove();
  }
  inline void Remove() {
    next->prev = prev;
    prev->next = next;
    next = this;
    prev = this;
  }
  inline void InsertFront(InternalList &list) {
    Add(this, &list, list.next);
  }
  inline void InsertBack(InternalList &list) {
    Add(this, list.prev, &list);
  }
  inline bool ListPopFront(InternalList *&item) {
    if(next == this) return false;
    item = next;
    item->Remove();
    return true;
  }
  template<class T>
  T &Get(InternalList T::* member) {
    uint8_t data[sizeof(T)];
    T * obj = reinterpret_cast<T*>(&data[0]);
    size_t offset = size_t(&(obj->*member)) - size_t(obj);
    return *reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(this) - offset);
  }
  template<class T>
  const T &Get(InternalList T::* member) const {
    uint8_t data[sizeof(T)];
    T * obj = reinterpret_cast<T*>(&data[0]);
    size_t offset = size_t(&(obj->*member)) - size_t(obj);
    return *reinterpret_cast<const T*>(reinterpret_cast<const uint8_t*>(this)
                                       - offset);
  }
 private:
  static inline void Add(InternalList *item, InternalList *prev,
                         InternalList *next) {
    next->prev = item;
    item->next = next;
    item->prev = prev;
    prev->next = item;
  }
  InternalList *next;
  InternalList *prev;
};

}

#endif
