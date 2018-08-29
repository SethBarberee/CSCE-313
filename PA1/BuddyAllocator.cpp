/* 
    File: my_allocator.cpp
*/
#include "BuddyAllocator.h"
#include <iostream>
using namespace std;

BuddyAllocator::BuddyAllocator (uint _basic_block_size, uint _total_memory_length){
    uint new_block_size;
    // Test for power of two
    if(_basic_block_size > 0 && ((_basic_block_size & (_basic_block_size - 1)) == 0)){
        printf("%d is a power of two\n", _basic_block_size);
    } else {
        // Somehow round it up to the closest power of two..
        printf("%d isn't a power of two\n", _basic_block_size);
        new_block_size = _basic_block_size;
        // Most accurate way to compute vs another method
        // see graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
        new_block_size--;
        new_block_size |= new_block_size >> 1;
        new_block_size |= new_block_size >> 2;
        new_block_size |= new_block_size >> 4;
        new_block_size |= new_block_size >> 8; 
        new_block_size |= new_block_size >> 16; 
        new_block_size++;
        printf("New size of %d\n", new_block_size);
    }	
}

BuddyAllocator::~BuddyAllocator (){
	
}

char* BuddyAllocator::alloc(uint _length) {
  /* This preliminary implementation simply hands the call over the 
     the C standard library! 
     Of course this needs to be replaced by your implementation.
  */
  return new char [_length];
  // make sure it doesn't return the header block. Has to return the actual block
}

int BuddyAllocator::free(char* _a) {
  /* Same here! */
  delete _a;
  return 0;
}

void BuddyAllocator::debug (){
}

