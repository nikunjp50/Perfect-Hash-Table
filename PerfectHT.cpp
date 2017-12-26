// File: PerfectHT.cpp
//
// UMBC CMSC 341 Fall 2017 Project 5
//
// Implementation of HashFunction, SecondaryHT & PerfectHT classes.
//
// Version: 2017-11-16
//

#include <stdexcept>
#include <iostream>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <cmath>
using namespace std ;
//char *target;
#include "PerfectHT.h"
#include "primes.h"


// =====================================================================
// HashFunction class implementation 
// =====================================================================


// By default do not print info to screen
//
bool HashFunction::m_debug = false ;


// Constructor.
// The constructor picks the smallest prime # larger than n
// for the table size. Default value of n defined in header.
//  
// Initializes other hash function parameters randomly.
//
HashFunction::HashFunction(int n /* =100*/ ) {

   // note: maxPrime defined in prime.h
   //
   if (n > maxPrime) throw out_of_range("HashTable size too big.\n") ;

   m_tsize = findPrime(n) ;
   m_factor = ( rand() % (m_tsize - 1) ) + 1 ;
   m_shift =  rand() % m_tsize ;
   m_multiplier = 33 ;  // magic number from textbook
   m_reboots = 0 ;

}


// Function that maps string to unsigned int.
// Return value can be much larger than table size.
// Uses m_multiplier data member.
// Return value must be unsgined for overflow to work correctly.
//
unsigned int HashFunction::hashCode(const char *str) const {

   unsigned int val = 0 ; 

   int i = 0 ;
   while (str[i] != '\0') {
      val = val * m_multiplier + str[i] ;
      i++ ;
   }
   return val ; 
}


// Getter for table size
//
int HashFunction::tableSize() const {
   return m_tsize ;
}


// Overloaded () operator that makes this a "functor" class.
// Returns the slot in the hash table for str.
// Uses the MAD method: h(x) = (ax + b) % m where 
// the parameters are:
//   a = m_factor
//   b = m_shift
//   m = m_tsize
//
unsigned int HashFunction::operator() (const char *str) const {
   return ( m_factor * hashCode(str) + m_shift ) % m_tsize ;
}


// Pick new parameters for MAD method and the hashCode function.
// Note: can change table size.
//
void  HashFunction::reboot() {

   // Implement the reboot function
   // pick new values for m_factor and m_shift each reboot
   //   make sure you get *different* values
   // change m_multiplier to next odd number  every 3rd reboot
   // increase table size to next prime # every 5th reboot

  m_reboots++;
  if (m_reboots % 3 == 0)
    {
      m_multiplier += 2;
    }
  if (m_reboots % 5 == 0)
    {
      int index = binarySearch(primes, 0, numPrimes-1, m_tsize);
      index++;
      m_tsize = primes[index];
    }
  
  m_factor = (rand() % (m_tsize - 1)) + 1;
  m_shift = rand() % m_tsize;
  if(m_debug == true){
  cout << "----reboot # " << m_reboots << " :  m_tsize = " << m_tsize 
       << " m_multiplier = " << m_multiplier << " m_factor = " << m_factor 
       << " m_shift = " << m_shift << endl;}
  HashFunction h(m_tsize);

}


// Set random seed for the random number generator.
// Call once at the start of the main program.
// Uses srand() and rand() from cstdlib which is
// shared with other code. For compatibility with
// C++98, it does not have a private random number 
// generator (e.g., mt19337).
//
void HashFunction::setSeed(unsigned int seed) {
   srand(seed) ;
}


// Find the smallest prime number larger than m.  

int HashFunction::findPrime(int m) {

   // Prime numbers are in global array primes[]
   // included from "primes.h"
   //
   // Use binary search!!
  int index = binarySearch(primes, 0, numPrimes - 1, m);
  return primes[index];
}

//Search through an array to find an index which should be equal or larger than item
int HashFunction::binarySearch(const int myarr[], int start, int end, int item)
{
  while (start <= end)
    {
      int mid = (start + ((end - start) / 2));
      if (myarr[mid] == item)
	return mid;
      if (myarr[mid] < item)
	start = mid + 1;
      else
	end = mid - 1;
    }
  if (start > end)
    return start;
  else return end;
}




// =====================================================================
// SecondaryHT class implementation 
// =====================================================================


// By default do not print debugging info 
bool SecondaryHT::m_debug = false ;


// Create a secondary hash table using the char * strings
// stored in "words". Makes copy of each item in words.
//
SecondaryHT::SecondaryHT(vector<const char *> words):hash(words.size()*words.size()) {
  
   //
   // Keep trying until a table with no collisions is found
   // 
   // wrap debugging statements in "if (m_debug) { ... }"
   //
   // remember that calling HashFunction::reboot() can change 
   // the table size!
   //
   // use strdup to copy char * strings
   //

  cout << "==== Called Secondary HT  ====\n";

  m_attempts = 0;
  ++m_attempts;
  m_size = words.size();
  for(int i = 0; i < m_size; i++)
    {
      cout << words[i] <<"(" << hash.hashCode(words[i]) <<") " <<" ";
    }
  cout <<endl;
  cout <<"SecondaryHT Constructor, # of attempts = " << m_attempts <<endl;
  //  cout << "m_size is : " << m_size << endl;

  int i = 0;
  int count = 0;
  while (i < 1) 
    {
      int counter = 0;
      T2 = new char*[hash.tableSize()];
      for (int j = 0; j < hash.tableSize(); j++)
	{
	  T2[j] = NULL;
	}
      int k = 0;
      while (k < m_size)
	{
	  int slot = hash(words.at(k));
	  if (T2[slot] == NULL)
	    {
	      T2[slot] = strdup(words.at(k));
	      if (counter == m_size - 1)
		{
		  i++;
		  break;
		}
	      counter++;
	      k++;
	    }
	  else
	    {
	      if (count == maxAttempts) 
		{
		  i++;
		  throw very_unlucky("Too Many attempts");
		  break;
		}
	      count++;
	      for(int v =0; v < hash.tableSize(); v++)
		{
		  free(T2[v]);
		  T2[v] = NULL;
		}
	      delete[]T2;
	      m_attempts++;
	      hash.reboot();
	      i = 0;
	      break;
	    }
	}
    }
}


// Copy constructor. Remember to strdup.
//
SecondaryHT::SecondaryHT(const SecondaryHT& other) 
{
  m_attempts = other.m_attempts;
  m_size = other.m_size;
  hash = other.hash;
  T2 = new char*[other.hash.tableSize()];
  for (int i = 0; i < other.hash.tableSize(); i++)
    {
      T2[i] = NULL;
    }
  for (int i = 0; i < other.hash.tableSize(); i++)
    {
      if(other.T2[i] != NULL)
	{
	  T2[i] = strdup(other.T2[i]);
	}
      else
	T2[i] = NULL;
    }
}


// Destructor. Use free() to free strings.
//
SecondaryHT::~SecondaryHT() 
{
  for(int i =0; i < hash.tableSize(); i++)
    {
      free(T2[i]);
      T2[i] = NULL;
    }
  delete[]T2;
}


// Assignment operator. Remember to use strdup() and free()
//
const SecondaryHT& SecondaryHT::operator=(const SecondaryHT& rhs) 
{
  if (this != &rhs)
    {
      delete[] T2;
      m_attempts = rhs.m_attempts;
      m_size = rhs.m_size;
      hash = rhs.hash;
      T2 = new char*[rhs.hash.tableSize()];
      for (int i = 0; i < rhs.hash.tableSize(); i++)
	{
	  T2[i] = NULL;
	}
      for (int i = 0; i < rhs.hash.tableSize(); i++)
	{
	  if(rhs.T2[i] != NULL)
	    {
	      T2[i] = strdup(rhs.T2[i]);
	    }
	  else
	    T2[i] = NULL;
	}
    }
  return *this;
}


// returns whether given word is in this hash table.
//
bool SecondaryHT::isMember (const char *word) const {

   int slot = hash(word) ;

   assert ( 0 <= slot && slot < hash.tableSize() ) ;

   if (T2[slot] == NULL) return false ;

   if ( strcmp(word, T2[slot]) != 0 ) return false ;

   return true ; 
}


// getter. Actually, the hash function has the size.
//
int SecondaryHT::tableSize() const {
   return hash.tableSize() ;
}


// Pretty print for debugging
//
void SecondaryHT::dump() const {
   cout << "=== Secondary Hash Table Dump: ===\n" ;
   cout << "Table size = " << hash.tableSize() << endl ;
   cout << "# of items = " << m_size << endl ;
   cout << "# of attempts = " << m_attempts << endl ;
 
   for (int i=0 ; i < hash.tableSize() ; i++) {
      cout << "T2[" << i << "] = " ;
      if (T2[i] != NULL) cout << T2[i] ;
      cout << endl ;
   }

   cout << "==================================\n" ;
}



// =====================================================================
// PerfectHT class implementation 
// =====================================================================


// by default, do not print debugging info
//
bool PerfectHT::m_debug = false ;


// Create a Perfect Hashing table using the first n strings 
// from the words array.
//
PerfectHT::PerfectHT (const char *words[], int n):hash(n) {

   // Implement constructor for PerfectHT here.
   //
   // You will need an array of vectors of char * strings.
   // Something like;
   //
   //    vector<const char *> *hold = new vector<const char *>[m] ;
   //
   // Each hold[i] is a vector that holds the strings
   // that hashed to slot i. Then hold[i] can be passed
   // to the SecondaryHT constructor.
   //
  int prime = hash.findPrime(n);
  vector<const char *> *hold = new vector<const char *>[prime];
  for (int i = 0; i < prime; i++)
    {
      int slot = hash(words[i]);
      hold[slot].push_back(words[i]);
    }
  PHT1 = new char*[prime];
  PHT2 = new SecondaryHT*[prime];
  for (int i = 0; i < prime; i++)
    {

      PHT1[i] = NULL;
      PHT2[i] = NULL;
    }
  for (int j = 0; j < prime; j++)
    {
      if (hold[j].size() == 1)
	{
	  PHT1[j] = strdup(hold[j].at(0));
	  PHT2[j] = NULL;
	}
      
      else if(hold[j].size() != 0)
	{
	  PHT1[j] = NULL;
	  PHT2[j] = new SecondaryHT(hold[j]); 
	}
      else
	{
	  PHT1[j] = NULL;
	  PHT2[j] = NULL;
	}
    }
  delete[] hold;   
}


// Copy constructor. Use strdup or SecondaryHT copy constructor.
//
PerfectHT::PerfectHT(const PerfectHT& other) 
{
   hash = other.hash;
   
  PHT1 = new char*[other.hash.tableSize()];
  PHT2 = new SecondaryHT*[other.hash.tableSize()];
  for (int i = 0; i < other.hash.tableSize(); i++)
    {
      PHT1[i] = NULL;
      PHT2[i] = NULL;
    }
  for (int i = 0; i < other.hash.tableSize(); i++)
    {
      if (other.PHT1[i] != NULL) 
	PHT1[i] = strdup(other.PHT1[i]);
      
      if (other.PHT2[i] != NULL) 
	{ 
	  PHT2[i] = new SecondaryHT(*other.PHT2[i]);
	}
    }
}


// Destructor. Remember to use free() for strdup-ed strings.
//
PerfectHT::~PerfectHT() 
{
  cout <<"Coming to Destructor\n";
  for(int i = 0; i < hash.tableSize(); i++)
    {
      free(PHT1[i]);
    }
  delete[]PHT1;
  delete[]PHT2;
}


// Assignment operator. Use strdup() and free() for strings.
//
const PerfectHT& PerfectHT::operator=(const PerfectHT& rhs) {

  if (this != &rhs)
    {
      delete[]PHT2;
      delete[]PHT1;
       hash = rhs.hash;
      PHT1 = new char*[rhs.tableSize()];
      PHT2 = new SecondaryHT*[rhs.tableSize()];
      for (int i = 0; i < rhs.tableSize(); i++)
	{
	  PHT1[i] = NULL;
	  PHT2[i] = NULL;
	}
      for (int i = 0; i < rhs.hash.tableSize(); i++)
	{
	  if (rhs.PHT1[i] != NULL)
	    PHT1[i] = strdup(rhs.PHT1[i]);

	  if (rhs.PHT2[i] != NULL)
	    {
	      PHT2[i] = new SecondaryHT(*rhs.PHT2[i]);
	    }
	}
    }
  return *this;
}


// Returns whether word is stored in this hash table.
//
bool PerfectHT::isMember(const char * str) const {
  int slot = hash(str) ; 

  if (PHT1[slot] == NULL && PHT2[slot] == NULL) return false ;

  if (PHT1[slot] != NULL) return strcmp(str,PHT1[slot]) == 0 ;
  
  return PHT2[slot]->isMember(str) ;
}


int PerfectHT::tableSize()const
{
  return hash.tableSize();
}

// Pretty print for debugging.
//
void PerfectHT::dump() const {
   int m = hash.tableSize() ;   

   cout << "------------- PerfectHT::dump()  -------------\n" ;
   cout << "table size = " << hash.tableSize() << endl ;
   cout << endl ;
   for (int j=0 ; j < m ; j++) {
      cout << "[" << j << "]:  " ;
      if (PHT1[j] != NULL) cout << PHT1[j] ;
      if (PHT2[j] != NULL) {
         cout << endl ;
         PHT2[j]->dump() ;
      }
      cout << endl ;
   }

   cout << "----------------------------------------------\n" ;
}
