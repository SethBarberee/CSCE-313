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
    new_block = power_of_two(_basic_block_size);
    new_total = power_of_two(_total_memory_length);
    // create the base blockheader for the big chunk
    base = new char[new_total];
    BlockHeader* base_head = (BlockHeader*) base;
    base_head->block_size = new_total; // assign the block size
    levels = log2(new_total) - log2(new_block) + 1;
    // TODO check this.... IDK if this is right
    free_lists = new LinkedList[levels]; // make the array of pointers to LinkedList

    // The last cell is the array is for the biggest size
    free_lists[levels-1].insert(base_head);
    block_size = new_block;
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
  cout << endl;
  cout << "Length requested: " << _length << "\tBlockHeader size: " << sizeof(BlockHeader) << endl;
  uint req_length = _length + sizeof(BlockHeader);
  req_length = power_of_two(req_length);
  cout << "Adjusted length: " << req_length << endl;
  uint current_size = block_size;
  uint size_avaliable = 0; // best size available
  uint biggest_indice = 0;
  char* new_block = nullptr;
  // If req_length is smaller, allocate the basic block
  if(req_length < block_size){
    req_length = block_size;
  }
  // Check if we have a block available to alloc else split blocks to the size
  uint indice_check = log2(req_length) - log2(block_size);
  if(free_lists[indice_check].get_size() > 0){
      size_avaliable = block_size * pow(2,indice_check);
      cout << "We need " << size_avaliable << " bytes and we have it so allocate it" << endl;
  }
  else {
      // TODO faster way to calculate?
      for(uint i = 0; i < levels; i++){
        if(free_lists[i].get_size() > 0){ // check if a free block is available
            size_avaliable = current_size;
        }
        current_size = current_size * 2;
      } 
  }
  

    biggest_indice = log2(size_avaliable) - log2(block_size); // calculate indice of biggest available
    current_size = free_lists[biggest_indice].get_head()->block_size;
    new_block = (char*)free_lists[biggest_indice].get_head();

  // check if we have enough space else return nullptr
  if(size_avaliable < req_length){
    fprintf(stderr, "Not enough memory for allocation of %d bytes\n", req_length);
    fprintf(stdout, "Biggest available is %d bytes\n", size_avaliable);
    return nullptr;
  }
  else {
    if(req_length < block_size){
        req_length = block_size; // make sure it gets the basic block size and nothing less
        cout << "Adjusted to be size of " << req_length << endl;
    }
    else if(free_lists[indice_check].get_size() > 0){
        new_block = (char*)free_lists[indice_check].get_head();
        biggest_indice = indice_check;
    }
    else {
        while(current_size > req_length){
            new_block = split((char*)free_lists[biggest_indice].get_head());
            biggest_indice--;
            current_size = free_lists[biggest_indice].get_head()->block_size;
        }
    }
  }
    // TODO once block is found, remove from free_lists
  free_lists[biggest_indice].remove((BlockHeader*)new_block);
  BlockHeader* block_head = (BlockHeader*) new_block;
  block_head->block_size = req_length;
  cout << "Allocating a block of size " << block_head->block_size << " bytes" << endl;
  debug();
  return new_block; 
}

int BuddyAllocator::free(char* _a) {
  // TODO find how much memory needs to be freed
  char *free_pointer = _a;
  free_pointer = _a-sizeof(BlockHeader);
  BlockHeader* header = (BlockHeader*) free_pointer;
  cout << "Freeing block of size: " << header->block_size << endl;
  cout << endl;
  header->in_use = false;
  // TODO get buddy of block
  //char *buddy = getbuddy(free_pointer);
  // TODO merge block if buddy is free
  //merge(free_pointer, buddy);
  // TODO add to free_lists
  // log(merged_block) should be indice to insert
  // free_lists[indice].insert(merged_block)
  delete _a;
  return 0;
}

void BuddyAllocator::debug (){
    cout << "Block size: " << block_size << "\t Total Memory: " << memory << endl;
    for(uint i = 0; i < levels; i++){
        cout << block_size*pow(2,i) << ": " << free_lists[i].get_size() << endl;
    }
}
/*
char* BuddyAllocator::getbuddy(char *block1){
    BlockHeader* free_pointer = (BlockHeader*) block1;
    // TODO gives error about char * and unsigned long being invalid operands
    // TODO cast char to an int??
    char *buddy = (block1 - sizeof(BlockHeader)) ^ free_pointer->block_size + sizeof(BlockHeader)
    // TODO check address for validity
    return buddy;
}

bool BuddyAllocator::arebuddies(char *block1, char *block2){
    // if the buddy equals each other, then they surely are buddies
    return ((getbuddy(block1) == block2) && (getbuddy(block2) == block1)); 
}

char* BuddyAllocator::merge (char* block1, char* block2){
    BlockHeader* header1 = nullptr;
    BlockHeader* header2 = nullptr;
    header1 = (BlockHeader*) block1;
    header2 = (BlockHeader*) block2;
    uint level = log2(header1->block_size) - log2(block_size);
    // check buddy of new merged block
    char* buddy = getbuddy(block1);
    BlockHeader* buddy_header = (BlockHeader*) buddy;
    while(!(buddy_header->in_use)){
        // Buddy is free
        header1->block_size = header2->block_size * 2;
        // remove both blocks from free_list
        free_lists[level].remove(header1);
        free_lists[level].remove(header2);
        // insert new block into free_list
        free_lists[level+1].insert(header1);
        // Recurse back....
        buddy = getbuddy(block1);
        merge(block1, buddy);
    }
    // TODO else return the address
    return block1; // TODO check if this is right
}
*/
char* BuddyAllocator::split (char* block){
    BlockHeader* header1 = nullptr;
    BlockHeader* header2 = nullptr;
    header1 = (BlockHeader*) block;
    uint level = log2(header1->block_size) - log2(block_size);
    cout << endl;
    cout << "Operating on the " << level << "th cell of free_list" << endl;
    free_lists[level].remove(header1);
    // set it equal to half
    header1->block_size = header1->block_size / 2;
    // move pointer to middle
    char* block2 = block;
    block2 = block2 + header1->block_size;
    // insert header2
    header2 = (BlockHeader*) block2;
    header2->block_size = header1->block_size;
    // Add the other to free_list
    cout << "Added new block on " << level-1 << "th cell of free_list" << endl;
    free_lists[level-1].insert(header2);
    free_lists[level-1].insert(header1);
    return block;
}
