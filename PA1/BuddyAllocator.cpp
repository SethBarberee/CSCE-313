/* 
    File: my_allocator.cpp
*/
#include "BuddyAllocator.h"
#include "Helper.h"
#include <iostream>
using namespace std;

BuddyAllocator::BuddyAllocator (uint _basic_block_size, uint _total_memory_length){
    // do stuff with _basic_block_size
    uint new_block, new_total;
    uint levels = 1; // we start with one with is the basic block size
    uint base_counter = 0;
    new_block = power_of_two(_basic_block_size);
    new_total = power_of_two(_total_memory_length);
    printf("blocksize = %d, memsize = %d\n", new_block, new_total);
    BlockHeader* base = new BlockHeader;
    base->block_size = new_total; // assign the block size
    base_counter = new_block;
    while(base_counter != new_total || base_counter < new_total){
        base_counter = base_counter * 2; // go up a power of two
        printf("Base is now: %d\t", base_counter);
        levels++; // increase the width of the aray
        printf("Level count: %d\n", levels);
    }
    free_lists = new BlockHeader[levels]; // make the array
    // start with basic block size as the smallest block size...
    // TODO assign last cell to base
 
}

BuddyAllocator::~BuddyAllocator (){
    // TODO destroy all the things
    delete[] free_lists;
}

char* BuddyAllocator::alloc(uint _length) {
  /* This preliminary implementation simply hands the call over the 
     the C standard library! 
     Of course this needs to be replaced by your implementation.
  */
  // make sure it doesn't return the header block. Has to return the actual block
  // TODO find required length
  // TODO split blocks until good size
  // TODO remove block from free_lists
  return new char [_length];
}

int BuddyAllocator::free(char* _a) {
  /* Same here! */
  // TODO add block back to free_lists
  // TODO merge block if free
  delete _a;
  return 0;
}

/*
void BuddyAllocator::debug (){
    // TODO loop through array of free_lists
    // TODO print the blocksize of the head
    // TODO print the size of the lists
    for(uint i = 0; i < sizeof(free_lists); i++){
        cout << <LinkedList>->head.block_size >> endl;
        cout << <LinkedList>.size() << endl;
    }
}

char* BuddyAllocator::getbuddy(char *block1){
    // TODO find block size of the block
    // TODO Block XOR Block size = Buddy Address
    // TODO check address for validity
}

bool BuddyAllocator::arebuddies(char *block1, char *block2){
    // if the buddy equals each other, then they surely are buddies
    return (getbuddy(block1) == block2 && getbuddy(block2) == block1); 
}
*/
