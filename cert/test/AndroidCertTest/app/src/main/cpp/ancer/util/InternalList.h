#ifndef AGDC_ANCER_INTERNALLIST_H_
#define AGDC_ANCER_INTERNALLIST_H_

namespace ancer {

class InternalList {
 public:
  inline InternalList() : next(this), prev(this) {}
  inline InternalList(InternalList &&other) {
    if (other.IsEmpty()) {
      next = this;
      prev = this;
    } else {
      other.next->prev = this;
      other.prev->next = this;
      next = other.next;
      prev = other.prev;
      other.next = &other;
      other.prev = &other;
    }
  }
  inline InternalList &operator=(InternalList &&other) {
    bool is_empty = IsEmpty();
    if (is_empty && !other.IsEmpty()) {
      InternalList *next = other.next;
      other.RemoveInit();
      InsertBack(*next);
    }
    return *this;
  }
  inline ~InternalList() { Remove(); }
  inline bool IsEmpty() const { return prev == next; }
  inline void Remove() {
    next->prev = prev;
    prev->next = next;
  }
  inline void Init() {
    next = this;
    prev = this;
  }
  inline void RemoveInit() {
    Remove();
    Init();
  }
  inline void InsertFront(InternalList &list) {
    Remove();
    Add(this, &list, list.next);
  }
  inline void InsertBack(InternalList &list) {
    Remove();
    Add(this, list.prev, &list);
  }
  inline bool ListPopFront(InternalList *&item) {
    if (!IsEmpty()) return false;
    item = next;
    item->Remove();
    return true;
  }
  template <class T>
  T &Get(InternalList T::*member) {
    uint8_t data[sizeof(T)];
    T *obj = reinterpret_cast<T *>(&data[0]);
    size_t offset = size_t(&(obj->*member)) - size_t(obj);
    return *reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(this) - offset);
  }
  template <class T>
  const T &Get(InternalList T::*member) const {
    uint8_t data[sizeof(T)];
    T *obj = reinterpret_cast<T *>(&data[0]);
    size_t offset = size_t(&(obj->*member)) - size_t(obj);
    return *reinterpret_cast<const T *>(
        reinterpret_cast<const uint8_t *>(this) - offset);
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

}  // namespace ancer

#endif
