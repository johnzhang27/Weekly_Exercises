/* --------------------------------------------------
*  Name: YongQuan Zhang
*  ID: 1515873
*  CMPUT 275, Winter 2020
*  Weekly Exercise #4: Dynamic Hashing
*---------------------------------------------------*/

#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_

#include "linked_list.h"
#include <cassert>
#include <algorithm>

// forward declaration of HashTable class
template <typename T>
class HashTable;


template <typename T>
class HashTableIterator {
public:
  // constructor, need to know a pointer to the hash table we are iterating over
  // this initialize this iterator to the first entry of the table (if any)
  HashTableIterator(const HashTable<T>* hashTable);

  // no need for a destructor, we don't allocate dynamic memory here

  // signals if we are at the end
  bool atEnd() const;

  // post increment operator for the iterator, look it up on the
  // page on operator overloading linked from the lecture slides to see
  // why we use this specific syntax
  HashTableIterator<T> operator++(int);

  // get the item the iterator is looking at
  const T& operator*() const;

private:
  // the bucket we are currently examining, we are at the end of iteration
  // if and only if bucket == the number of buckets held by *tablePtr
  unsigned int bucket;

  // the table itself that we are iterating over
  const HashTable<T>* tablePtr;

  // the node in the list we are examining
  ListNode<T>* node;

  // advance the iterator
  void advance();
};

template <typename T>
HashTableIterator<T>::HashTableIterator(const HashTable<T>* hashTable) {
  bucket = 0;
  tablePtr = hashTable;
  node = tablePtr->table[0].getFirst();
  if (node == NULL) {
    // if the first bucket was empty, advance to the next item in the table
    advance();
  }
}

template <typename T>
bool HashTableIterator<T>::atEnd() const {
  return bucket == tablePtr->tableSize;
}

template <typename T>
HashTableIterator<T> HashTableIterator<T>::operator++(int) {
  HashTableIterator<T> copy = *this; // create a copy
  advance();
  return copy;
}

template <typename T>
const T& HashTableIterator<T>::operator*() const {
  assert(!atEnd());
  return node->item;
}

template <typename T>
void HashTableIterator<T>::advance() {
  assert(!atEnd());
  if (node != NULL)
    node = node->next;
  // if we advanced past the end of the list in this bucket,
  // then scan for the next nonempty bucket
  while (node == NULL && bucket < tablePtr->tableSize) {
    ++bucket;
    node = tablePtr->table[bucket].getFirst();
  }
}

/*
  A hash table for storing items. It is assumed the type T of the item
  being stored has a hash method, eg. you can call item.hash(), which
  returns an unsigned integer.

  Also assumes the != operator is implemented for the item being stored,
  so we could check if two items are different.

  If you just want store integers int for the key, wrap it up in a struct
  with a .hash() method and both == and != operator.
*/

template <typename T>
class HashTable {
  friend class HashTableIterator<T>; // now we can access the private variables of HashTable<T>

public:
  // creates an empty hash table with the given number of buckets.
  // Give the tablesize a default value 10.
  HashTable(unsigned int tableSize = 10);
  ~HashTable();

  // Check if the item already appears in the table.
  bool contains(const T& item) const;

  // Insert the item, do nothing if it is already in the table.
  // Returns true iff the insertion was successful (i.e. the item was not there).
  bool insert(const T& item);

  // Removes the item after checking, via assert, that the item was in the table.
  void remove(const T& item);

  unsigned int size() const;

private:
  LinkedList<T> *table; // start of the array of linked lists (buckets)
  unsigned int numItems; // # of items in the table
  unsigned int tableSize; // # of buckets
  // Computes the hash table bucket that the item maps into
  // by calling it's .hash() method.
  unsigned int getBucket(const T& item) const;
  void reSize();
};

template <typename T>
HashTable<T>::HashTable(unsigned int tableSize){
  // make sure there is at least one bucket
  assert(tableSize > 0);
  // calls the constructor for each linked list
  // so each is initialized properly as an empty list
  table = new LinkedList<T>[tableSize];

  // we are not storing anything
  numItems = 0;
  this->tableSize = tableSize;
}

template <typename T>
HashTable<T>::~HashTable() {
  // this will call the destructor for each linked
  // list before actually deleting this table from
  // the heap
  delete[] table;
}

template <typename T>
bool HashTable<T>::contains(const T& item) const {
  unsigned int bucket = getBucket(item);

  return table[bucket].find(item) != NULL;
}


template <typename T>
bool HashTable<T>::insert(const T& item) {
  // if the item is here, return false
  if (contains(item)) {
    return false;
  }
  else {
    // otherwise, insert it into the front of the list
    // in this bucket and return true
    unsigned int bucket = getBucket(item);
    table[bucket].insertFront(item);
    ++numItems;
    reSize();
    return true;
  }
}

template <typename T>
void HashTable<T>::remove(const T& item) {
  unsigned int bucket = getBucket(item);

  ListNode<T>* link = table[bucket].find(item);

  // make sure the item was in the list
  assert(link != NULL);

  table[bucket].removeNode(link);

  --numItems;
  reSize();
}

template <typename T>
unsigned int HashTable<T>::size() const {
  return numItems;
}

template <typename T>
unsigned int HashTable<T>::getBucket(const T& item) const {
  return item.hash() % tableSize;
}

template <typename T>
void HashTable<T>::reSize(){
  LinkedList<T> *temptable;
  unsigned int temptableSize;
  bool flag = false;
  if(numItems > tableSize){
    temptableSize = tableSize*2;
    flag = true;
  }
  else if(numItems<(tableSize/4) && tableSize > 10){
    temptableSize = max(tableSize/2,(unsigned int)10);
    flag = true;
  }
  if(flag){
    unsigned int oldSize = tableSize;
    // Create a temporary "hashtable", it is an array of linked
    // lists.
    temptable = new LinkedList<T>[temptableSize];
    // Create a hashiterator variable called hash.
    HashTableIterator<T> Iterator = HashTableIterator<T>(this);
    for(int i = 0;i<numItems;i++){
      // Use the operator * to get the 1st item in 1st bucket.
      const T& item = *Iterator;
      // Set tablesize to the new tablesize so we could get
      // bucket for the temporary hashtable.
      tableSize = temptableSize;
      unsigned int tempbucket = getBucket(item);
      temptable[tempbucket].insertFront(item);
      // Set the tablesize back to old tablesize because 
      // we still need to fetch item from the old table.
      tableSize = oldSize;
      Iterator++;
    }
    delete[] table;
    table = temptable;
    tableSize = temptableSize;
  }
}
#endif