#ifndef HASH_H
#define HASH_H

#define DELETED_HASH_SLOT ((HashEntry<KeyType,ValueType>*)1)

//#ifdef WIN32
//#include <windows.h>
//#endif

#include <stdio.h>
#include <math.h>

extern FILE *dbgfp;

template<class KeyType, class ValueType>
class HashEntry {
 public:
  HashEntry(KeyType *aKey, ValueType *anItem) {
    key = aKey;
    item = anItem;
  }
  KeyType *key;
  ValueType *item;
};

template<class KeyType, class ValueType>
class Hash {
 public:
  Hash(int (*hashFunc)(KeyType&, int)) {
    table = NULL;
    load = 0;
    capacity = 0;
    hash = hashFunc;
  }

  ~Hash() {

    //    fprintf(dbgfp,"~Hash: table=%x\n",table);

    clear();
    delete[] table;
  }

  int nextPrimeGreaterThan(int n) {
    for (int p=n; ; p++) {
      if (p % 2 == 0) // check if even
	continue;
      else {
	int maxDivisor = (int)sqrt((double)(p+1));
	bool found = false;
	for (int d=3; d<=maxDivisor; d+=2) { // check odd divisors only
	  if (p % d == 0) {
	    found = true;
	    break;
	  }
	}
	if (!found) // found no divisors for p.  It's prime!
	  return p;
      }
    }
    //
    // Shouldn't happen!
    //
    return n+1;
  }

  int size() {
    return load;
  }

  void clear() {

    //    fprintf(dbgfp,"Hash::clear\n");

    for (int i=0; i<capacity; i++) {
      HashEntry<KeyType,ValueType>* entry = table[i];
      if (entry != NULL &&
	  entry != DELETED_HASH_SLOT)
	delete entry;
      table[i] = NULL;
    }
    load = 0;
  }
  
  void reserve(int n) {
    allocate(n);
  }

  void add(KeyType& key, ValueType& item) {

    //    fprintf(dbgfp,"Hash::add. key=%d item=%d\n",key,item);
    
    if (table == NULL) {
      allocate(100);
    }
    else if (load > maxLoad) {
      rehash();
    }

    int loc = getSlot(key);

    //        printf("item %d (key %d) goes into %d\n",item,key,loc);
    //    fprintf(dbgfp,"item %d (key %d) goes into %d\n",item,key,loc);

    load++;
    table[loc] = new HashEntry<KeyType,ValueType>(&key, &item);
  }

  void allocate(int n) {

    //    fprintf(dbgfp,"Hash::allocate(%d)\n",n);

    capacity = nextPrimeGreaterThan(n);
    maxLoad = (int)(capacity * 0.66);
    table = new HashEntry<KeyType,ValueType>*[capacity];

    //    fprintf(dbgfp,"Hash::allocate: table=%x\n",table);

    load = 0;

    for (int i=0; i<capacity; i++) {
      table[i] = NULL;
    }
  }

  //
  // Search for the item in the table.
  //
  int getLocation(KeyType& key) {

    //    fprintf(dbgfp,"Hash::getLocation(%d)\n",key);

    int loc = hash(key, capacity);

    if (table[loc] == NULL) {
      return -1; // nothing where item should be.
    }
    else if (*(table[loc]->key) == key) {
      return loc; // found it!
    }
    else { // slot occupied with something else.  Do double hashing.

      int increment = (1 + hash(key, capacity-1)) % capacity;

      //      fprintf(dbgfp,"capacity=%d\n",capacity);
      //      fprintf(dbgfp,"increment=%d\n",increment);
      //      dump();

      for (int i=1; i<capacity; i++) {
	loc = (loc + increment) % capacity;
	if (table[loc] == NULL) {
	  return -1; // nothing here!
	}
	else if (table[loc] != DELETED_HASH_SLOT &&
		 *(table[loc]->key) == key) { // found it!
	  return loc;
	}
      }
      return -1; // table full, but key not found.
    }
  }

  //
  // Find a valid place to stash the item.
  //
  int getSlot(KeyType& key) {

    //    fprintf(dbgfp,"Hash::getSlot(%d)\n",key);

    int loc = hash(key, capacity);
    if (table[loc] == NULL ||
	table[loc] == DELETED_HASH_SLOT) { // empty or deleted slot
      return loc; // stash it here!
    }
    else { // slot occupied.  Do double hashing.
      int increment = (1 + hash(key, capacity-1)) % capacity;
      for (int i=1; i<capacity; i++) {
	loc = (loc + increment) % capacity;
	if (table[loc] == NULL || 
	    table[loc] == DELETED_HASH_SLOT) { // empty or deleted slot
	  return loc;
	}
      }
      // went through whole table, found no free slots. table full!
      // (should never happen, because we have limit load factor to 2/3
      return -1;
    }
  }

  void rehash() {

    //    fprintf(dbgfp,"Hash::rehash\n");

    int oldCapacity = capacity;
    HashEntry<KeyType,ValueType>** oldTable = table;

    allocate(capacity * 2);

    //    printf("rehash: new capacity=%d oldTable=%x\n",capacity,oldTable);

    if (oldTable != NULL) {
      for (int i=0; i<oldCapacity; i++) {
	HashEntry<KeyType,ValueType> *entry = oldTable[i];
	if (entry != NULL &&
	    entry != DELETED_HASH_SLOT) {
	  add(*(entry->key), *(entry->item));

	  delete entry;
	}
      }
      delete[] oldTable;
    }
  }

  bool contains(KeyType& key) {
    int loc = getLocation(key);
    return loc != -1;
  }

  ValueType* get(KeyType& key) {
    int loc = getLocation(key);
    if (loc == -1) {
      return NULL;
    }
    else {
      return table[loc]->item;
    }
  }

  ValueType& remove(KeyType& key) {
    int loc = getLocation(key);
    if (loc != -1) {
      HashEntry<KeyType,ValueType>* deletedEntry = table[loc];
      ValueType& result = *(deletedEntry->item);
      table[loc] = DELETED_HASH_SLOT;
      load--;
      delete deletedEntry;
      return result;
    }
    else
      return *(table[0]->item);  // gotta return something!
  }

  void dump() {
    for (int i=0; i<capacity; i++) {
      //      printf("table[%d]=",i);
      fprintf(dbgfp,"table[%d]=",i);
      if (table[i] == NULL) {
	//	printf("NULL\n");
	fprintf(dbgfp,"NULL\n");
      }
      else if (table[i] == DELETED_HASH_SLOT) {
	//	printf("DELETED\n");
	fprintf(dbgfp,"DELETED\n");
      }
      else {
	//	printf("(key value)=(%d %d) addr=%x\n",
	//	       *(table[i]->key),*(table[i]->item),
	//	       table[i]);
	fprintf(dbgfp,"(key value)=(%d %d) addr=%x\n",
	       *(table[i]->key),*(table[i]->item),
	       table[i]);
      }
    }
  }

  class iterator;
  friend class iterator;
  class iterator {
    HashEntry<KeyType,ValueType>** table;
    int capacity;
    int load;
    int maxLoad;
    int nextLoc;

  public:

    iterator(HashEntry<KeyType,ValueType>** hashTable,
     	     int hashCapacity,
     	     int hashLoad,
     	     int initialLocation) {
     
      table = hashTable;
      capacity = hashCapacity;
      load = hashLoad;

      nextLoc = nextNonemptyLocation(initialLocation);

      //      fprintf(dbgfp,"Hash::iterator. initialLoc=%d nextLoc=%d\n",
      //      	      initialLocation,nextLoc);
      //      fflush(dbgfp);

    }

    int nextNonemptyLocation(int loc) {
      //
      // If necessary, advance loc until it finds a filled slot.
      // If table is completely empty, it won't find one!
      //
      for (int i=0; i<capacity; i++) {
	//	HashEntry<KeyType,ValueType><T>* nextEntry = table[loc];
	//	loc = (loc + 1) % capacity;
	loc++;
	if (loc >= capacity)
	  return -1;

	HashEntry<KeyType,ValueType>* nextEntry = table[loc];
	if (nextEntry != NULL &&
	    nextEntry != DELETED_HASH_SLOT)
	  return loc;
      }
      return -1;
    }

    int prevNonemptyLocation(int loc) {
      //
      // loc points after the prev slot, so back it up, before looking
      // at the slot.
      //
      for (int i=1; i<capacity; i++) {
	//	loc = (loc - 1 + capacity) % capacity;
	loc--;
	if (loc < 0)
	  return -1;

	//	HashEntry<KeyType,ValueType><T>* nextEntry = table[loc];
	HashEntry<KeyType,ValueType>* nextEntry = table[loc];
	if (nextEntry != NULL &&
	    nextEntry != DELETED_HASH_SLOT)
	  return loc;
      }

      return -1;
    }

    ValueType& getNext() {
      //
      // Hopefully, the current slot is occupied.
      // (that is true on the first call to this method).
      // If not, advance it until we find an occupied slot.
      //
      if (nextLoc == -1)
	return *(table[0]->item);
      else {
	int n = nextLoc;
	nextLoc = nextNonemptyLocation(nextLoc);
	return *(table[n]->item);
      }
    }

    ValueType& getPrev() {
      //
      // curLoc points after the prev slot, so back it up, before looking
      // at the slot.
      //
      nextLoc = prevNonemptyLocation(nextLoc);
      if (nextLoc == -1)
	return *(table[0]->item);
      else
	return *(table[nextLoc]->item);
    }

    bool hasNext() {
      return nextLoc != -1;
    }
  
    bool hasPrev() {
      return prevNonemptyLocation(nextLoc) != -1;
    }
  };

  iterator* first() {
    return new iterator(table, capacity, load, -1);
  }

 private:
  HashEntry<KeyType,ValueType>** table;
  int capacity;
  int load;
  int maxLoad;
  int (*hash)(KeyType&,int);
};

#endif
