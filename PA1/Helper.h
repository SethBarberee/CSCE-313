#ifndef _Helper_h_                   // include file only once
#define _Helper_h_

typedef unsigned int uint;

uint power_of_two(uint number){
    uint new_number;
    // Test for power of two
    // TODO try (number ^ (number - 1)) -- From Tanzir
    if(number > 0 && ((number & (number - 1)) == 0)){
        new_number = number; // size is fine so just copy it
    } else {
        // Somehow round it up to the closest power of two..
        new_number = number;
        // Most accurate way to compute vs another method
        // see graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
        new_number--;
        new_number |= new_number >> 1;
        new_number |= new_number >> 2;
        new_number |= new_number >> 4;
        new_number |= new_number >> 8;
        new_number |= new_number >> 16;
        new_number++;
    }
    return new_number;
}

#endif
