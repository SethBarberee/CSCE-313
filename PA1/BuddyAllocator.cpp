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
    base_head->in_use = false;
    // Calculate the levels needed
    levels = log2(new_total) - log2(new_block) + 1;
    free_lists = new LinkedList[levels]; // make the array of pointers to LinkedList

    // The last cell is the array is for the biggest size
    free_lists[levels-1].insert(base_head);
    block_size = new_block;
    memory = new_total;
}

BuddyAllocator::~BuddyAllocator (){
    // destroy all the things
    delete[] free_lists;
}

char* BuddyAllocator::alloc(uint _length) {
  /* This preliminary implementation simply hands the call over the 
     the C standard library! 
     Of course this needs to be replaced by your implementation.
  */
  // make sure it doesn't return the header block. Has to return the actual block
  // Make enough room for length and blockheder
  uint req_length = _length + sizeof(BlockHeader);
  // round up power of two if we need to
  req_length = power_of_two(req_length);
  uint current_size = block_size;
  uint size_avaliable = 0; // best size available
  uint biggest_indice = 0;
  char* new_block = nullptr;
  BlockHeader* block_head = nullptr;
  // If req_length is smaller, allocate the basic block
  if(req_length < block_size){
    req_length = block_size;
  }
  // Check if we have a block available to alloc else split blocks to the size
  uint indice_check = log2(req_length) - log2(block_size);
  // account for when indice check goes over levels
  if(indice_check > (levels-1)){
      indice_check = (levels-1);
  }
  if(free_lists[indice_check].get_size() > 0){
      // We've got the perfect block for it
      size_avaliable = block_size * pow(2,indice_check);
      current_size = size_avaliable;
  }
  else {
      // TODO faster way to calculate?
      for(uint i = 0; i < levels; i++){
        if(free_lists[i].get_size() > 0){ // check if a free block is available
            // make sure block is of enough size
            if(current_size > req_length){
                size_avaliable = current_size;
                break;
            }
        }
        current_size = current_size * 2;
      } 
  }
    biggest_indice = log2(size_avaliable) - log2(block_size); // calculate indice of biggest available
    BlockHeader* new_head = nullptr;
    new_head = free_lists[biggest_indice].get_head();
    new_block = (char*) new_head;

  // check if we have enough space else return nullptr
  if(size_avaliable < req_length){
    fprintf(stderr, "Not enough memory for allocation of %d bytes\n", req_length);
    fprintf(stdout, "Biggest available is %d bytes\n", size_avaliable);
    return nullptr;
  }
  else {
    if(free_lists[indice_check].get_size() > 0){
        new_block = (char*)free_lists[indice_check].get_head();
        biggest_indice = indice_check;
    }
    else {
       while(current_size > req_length){
            split(new_block);
            biggest_indice = biggest_indice - 1;
            new_head = free_lists[biggest_indice].get_head();
            new_block = (char*)new_head;
            current_size = new_head->block_size;
       } 
    }
  }

  block_head = (BlockHeader*) new_block;
  free_lists[biggest_indice].remove(block_head);
  // set the values
  block_head->in_use = true;
  // return pointer to block + blockheader
  return (new_block+sizeof(BlockHeader)); 
}

int BuddyAllocator::free(char* _a) {
  char* free_pointer = _a;
  char* buddy = nullptr;
  free_pointer = free_pointer-sizeof(BlockHeader);
  BlockHeader* header = (BlockHeader*) free_pointer;
  // set the block use flag to not in use
  header->in_use = false;
  // figure out what level we are on
  uint level = log2(header->block_size) - log2(block_size);
  while(true){
    if(free_lists[level].get_size() > 0 && level < (levels-1)){
        buddy = getbuddy(free_pointer);
        // check the buddy to make sure it's valid
        if(isvalid(buddy)){
            // it's all good to merge
            free_pointer = merge(free_pointer, buddy);
            level++;
        }
        else {
            // Buddy in use
            free_lists[level].insert(header);
            break;
        }
    } 
    else {
        // Nothing in the list to merge with anyway
        free_lists[level].insert(header);
        break;
    }
  }
  return 0;
}

void BuddyAllocator::debug (){
    cout << "Block size: " << block_size << "\t Total Memory: " << memory << endl;
    for(uint i = 0; i < levels; i++){
        cout << block_size*pow(2,i) << ": " << free_lists[i].get_size() << endl;
    }
}

char* BuddyAllocator::getbuddy(char *block1){
    BlockHeader* free_pointer = (BlockHeader*) block1;
    char* buddy = nullptr;
    //char* buddy = (block1 - sizeof(BlockHeader));
    buddy = (char*)(((block1-base) ^ free_pointer->block_size) + base);
    return buddy;
}

bool BuddyAllocator::arebuddies(char *block1, char *block2){
    char* buddy1 = nullptr;
    char* buddy2 = nullptr;
    buddy1 = getbuddy(block1);
    buddy2 = getbuddy(block2);
    // if the buddy equals each other, then they surely are buddies
    if((buddy1 == block2) && (buddy2 == block1)){
        return true;
    }
    else {
        return false;
    }
}

char* BuddyAllocator::merge (char* block1, char* block2){
    BlockHeader* header1 = nullptr;
    BlockHeader* header2 = nullptr;
    if(block1 > block2){
        header1 = (BlockHeader*) block2;
        header2 = (BlockHeader*) block1;
    } else {
        header1 = (BlockHeader*) block1;
        header2 = (BlockHeader*) block2;
    }
    uint level = log2(header1->block_size) - log2(block_size);
    // remove both blocks from free_list
    free_lists[level].remove(header2);
    free_lists[level].remove(header1);
    header1->block_size = header2->block_size * 2;
    // return the address
    return block1;
}

char* BuddyAllocator::split (char* block){
    BlockHeader* header1 = nullptr;
    BlockHeader* header2 = nullptr;
    header1 = (BlockHeader*) block;
    // Figure out what index we are in free_lists
    uint level = log2(header1->block_size) - log2(block_size);
    // Remove the header on that level
    free_lists[level].remove(header1);
    // set it equal to half
    header1->block_size = header1->block_size / 2;
    // move pointer to middle
    char* block2 = block;
    block2 = block2 + header1->block_size;
    // insert header2
    header2 = (BlockHeader*) block2;
    header2->block_size = header1->block_size;
    free_lists[level-1].insert(header2);
    free_lists[level-1].insert(header1);
    return block;
}
bool BuddyAllocator::isvalid(char *addr){
    BlockHeader* header = nullptr;
    header = (BlockHeader*) addr;
    // make sure this is a multiple of block_size
    if((header->block_size % block_size)  == 0){
        return true;
    }
    else {
        return false;
    }
}
