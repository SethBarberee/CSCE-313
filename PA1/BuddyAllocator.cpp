/* 
    File: my_allocator.cpp
*/
#include "BuddyAllocator.h"
#include "Helper.h"
#include <iostream>
#include "math.h"
using namespace std;

BuddyAllocator::BuddyAllocator (uint _basic_block_size, uint _total_memory_length){
    // do stuff with _basic_block_size
    uint new_block, new_total;
    uint base_counter = 0;
    new_block = power_of_two(_basic_block_size);
    new_total = power_of_two(_total_memory_length);
    // create the base blockheader for the big chunk
    base = new BlockHeader;
    base->block_size = new_total; // assign the block size
    base_counter = new_block;
    // TODO find a faster way to calculate
    while(base_counter < new_total){
        base_counter = base_counter * 2;
        levels++;
    }
    // TODO check this.... IDK if this is right
    free_lists = new LinkedList[levels]; // make the array of pointers to LinkedList

    // The last cell is the array is for the biggest size
    free_lists[levels-1].insert(base);
    block = new_block;
    memory = new_total;
}

BuddyAllocator::~BuddyAllocator (){
    // TODO destroy all the things
    delete[] free_lists;
    delete base;
}

char* BuddyAllocator::alloc(uint _length) {
  /* This preliminary implementation simply hands the call over the 
     the C standard library! 
     Of course this needs to be replaced by your implementation.
  */
  // make sure it doesn't return the header block. Has to return the actual block
  // check that _length is a valid power of two
  cout << "Length requested: " << _length << "\tBlockHeader size: " << sizeof(BlockHeader) << endl;
  uint req_length = _length + sizeof(BlockHeader);
  req_length = power_of_two(req_length);
  cout << "Adjusted length: " << req_length << endl;
  uint current_size = block;
  uint size_avaliable = 0;
  // TODO check if we have memory to alloc else return null
  for(uint i = 0; i < levels; i++){
    if(free_lists[i].get_size() > 0){ // check if a free block is available
        size_avaliable = current_size;
    }
    current_size = current_size * 2;
  }
  // check if we have enough space else return nullptr
  if(size_avaliable < req_length){
    fprintf(stderr, "Not enough memory for allocation of %d bytes\n", req_length);
    return nullptr;
  }
  else {
    if(req_length < block){
        req_length = block; // make sure it gets the basic block size and nothing less
        cout << "Adjusted to be size of " << req_length << endl;
    }
    // TODO split blocks until good size
    // TODO once block is found, remove from free_lists
  }
  cout << endl;
  return new char [req_length];
}

int BuddyAllocator::free(char* _a) {
  // TODO find how much memory needs to be freed
  void *free_pointer = _a;
  free_pointer = (char*)(free_pointer)-sizeof(BlockHeader);
  cout << "Freeing block of size: " << static_cast<BlockHeader*>(free_pointer)->block_size << endl;
  cout << endl;
  static_cast<BlockHeader*>(free_pointer)->in_use = false;
  // TODO get buddy of block
  //void *buddy = getbuddy((char*)free_pointer);
  // TODO merge block if buddy is free
  //merge((char*)free_pointer, (char*)buddy);
  // TODO add to free_lists
  // log(merged_block) should be indice to insert
  // free_lists[indice].insert(merged_block)
  delete _a;
  return 0;
}

void BuddyAllocator::debug (){
    cout << "Block size: " << block << "\t Total Memory: " << memory << endl;
    for(uint i = 0; i < levels; i++){
        cout << block*pow(2,i) << ": " << free_lists[i].get_size() << endl;
    }
}
/*
char* BuddyAllocator::getbuddy(char *block1){
    void *free_pointer = block1;
    // TODO gives error about char * and unsigned long being invalid operands
    // char *buddy = (block1 - sizeof(BlockHeader)) ^ static_cast<BlockHeader*>(free_pointer)->block_size + sizeof(BlockHeader)
    // TODO check address for validity
    return (char*) buddy;
}

bool BuddyAllocator::arebuddies(char *block1, char *block2){
    // if the buddy equals each other, then they surely are buddies
    return (getbuddy(block1) == block2 && getbuddy(block2) == block1); 
}

char* BuddyAllocator::merge (char* block1, char* block2){
    // TODO merge block
    // block1->block_size = block2->block_size * 2;
    // delete block2;
    // TODO check buddy of new merged block
    // void *buddy = getbuddy(block1);
    // TODO recurse if buddy is free
    // while(!(buddy->in_use)){
    //      merge(block1, buddy);
    //      buddy = getbuddy(merged_block);
    // }
    // TODO else return the address
    // return block1; TODO check if this is right
}

char* BuddyAllocator::split (char* block){
    // TODO recursively check if the size is good enough
    // TODO else keep splitting by putting a new header halfway between
    // TODO add to free lists
}

*/
