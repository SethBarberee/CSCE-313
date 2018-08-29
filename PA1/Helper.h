#ifndef _Helper_h_                   // include file only once
#define _Helper_h_

typedef unsigned int uint;

uint power_of_two(uint number){
    uint new_block_size;
    // Test for power of two
    if(number > 0 && ((number & (number - 1)) == 0)){
        new_block_size = number; // size is fine so just copy it
    } else {
        // Somehow round it up to the closest power of two..
        new_block_size = number;
        // Most accurate way to compute vs another method
        // see graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
        new_block_size--;
        new_block_size |= new_block_size >> 1;
        new_block_size |= new_block_size >> 2;
        new_block_size |= new_block_size >> 4;
        new_block_size |= new_block_size >> 8; 
        new_block_size |= new_block_size >> 16; 
        new_block_size++;
    }
    return new_block_size;
}

#endif
