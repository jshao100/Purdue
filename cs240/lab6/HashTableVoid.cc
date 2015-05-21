
//
// Implementation of a HashTable that stores void *
//
#include "HashTableVoid.h"

// Obtain the hash code of a key
int HashTableVoid::hash(const char * key)
{
	// Add implementation here
  	int h = 0;
  	const char * p = key;
  	while (*p) {
		h += *p;
		p++;
	  }
	return h % TableSize;
}

// Constructor for hash table. Initializes hash table
HashTableVoid::HashTableVoid()
{
  	// Add implementation here
	_buckets = (HashTableVoidEntry **)malloc(TableSize * sizeof(HashTableVoidEntry*));
	for (int i = 0; i < TableSize; i++) {
		_buckets[i] = NULL;
	}
}

// Add a record to the hash table. Returns true if key already exists.
// Substitute content if key already exists.
bool HashTableVoid::insertItem( const char * key, void * data)
{
  	//get key
	int h = hash(key);

	//search
  	HashTableVoidEntry * e = _buckets[h];
  	while (e != NULL) {
		if (0 == strcmp(key,e->_key)) { //found
			e->_data = data;
			return true;
		}
		e = e->_next;
	}

	//not found, add to hash table
	e = new HashTableVoidEntry;
	e->_key = strdup(key);
	e->_data = data;
	e->_next = _buckets[h];
	_buckets[h] = e;
	return false;
}

// Find a key in the dictionary and place in "data" the corresponding record
// Returns false if key is does not exist
bool HashTableVoid::find( const char * key, void ** data)
{
	//get hash bucket
	int h = hash(key);	

	//search
	HashTableVoidEntry * e = _buckets[h];
	while (e != NULL) {
		if (0 == strcmp(key, e->_key)) {
			*data = e->_data;
			return true;
		}
		e = e->_next;
	}
	return false;
}

// Removes an element in the hash table. Return false if key does not exist.
bool HashTableVoid::removeElement(const char * key)
{
	//get hash bucket
	int h = hash(key);

	//search
	HashTableVoidEntry * e = _buckets[h];
	HashTableVoidEntry * last = NULL;
	while (e != NULL) {
		if (0 == strcmp(key, e->_key)) {
			if (last != NULL) {
				last->_next = e->_next;	
			}
			else {
				_buckets[h] = e->_next;
			}
			delete e;
			return true;
		}
		last = e;
		e = e->_next;
	}
	return false;
}

// Creates an iterator object for this hash table
HashTableVoidIterator::HashTableVoidIterator(HashTableVoid * hashTable)
{
	_hashTable = hashTable;
	_currentEntry = NULL;
	_currentBucket = 0;
}

// Returns true if there is a next element. Stores data value in data.
bool HashTableVoidIterator::next(const char * & key, void * & data)
{
	if (_currentEntry != NULL) {
		_currentEntry = _currentEntry->_next;	
		if (_currentEntry != NULL) {
			key = _currentEntry->_key;
			data = _currentEntry->_data;
			return true;
		}
	}
	_currentBucket++;
	while (_currentBucket < HashTableVoid::TableSize) {
		if (_hashTable->_buckets[_currentBucket] != NULL) {
			_currentEntry = _hashTable->_buckets[_currentBucket];
			key = _currentEntry->_key;
			data = _currentEntry->_data;
			return true;
		}
		_currentBucket++;
	}
	return false;
}

