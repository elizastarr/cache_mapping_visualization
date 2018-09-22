
// -----------------------------------
// CSCI 340 - Operating Systems
// Fall 2018
// cache.c file
// 
// Homework 1
//
// -----------------------------------

#include "cache.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int initializeCache( unsigned int number_of_lines ) {

	int line, retVal;

	retVal = OK;

	cache = malloc( sizeof( cache_line ) * number_of_lines );

	if ( cache != NULL ) {

		// loops through lines of cache from 0-7
		for ( line=0; line<number_of_lines; line++ ) {

			cache[line] = (cache_line*) malloc( sizeof( cache_line ) );
			// tag and hit_count is similar to keys in a python dictionary
			// puts UNK into the value of the key, tag (and dereferences?)
			cache[line]->tag = UNK;
			cache[line]->hit_count = ZERO;

		}

	} else
		retVal = FAIL;

	return retVal;

} // end initializeCache function
 
int cread( unsigned int cmf, unsigned int* hex_addr, unsigned int* found, unsigned int* replace ) {

	/* TODO: You complete

// ------------------------------------
// Function prototype that reads the cache 
// and returns the byte at the specified 
// physical memory address location.
// 
//
// Arguments:	cache mapping function (cmf)
//		memory address pointer (hex_addr)
//		found variable pointer (1=cache hit, 0=cache miss)
//		replace variable pointer (1=replacment, 0=no replacement)
//
// Return:     	byte value at specified memory location (no error)
//		FAIL (error)
//
	*/

int retVal = FAIL;
*found = MISS;

// Direct Mapping
if(cmf == 1) 
{
	/*
	https://www.geeksforgeeks.org/extract-k-bits-given-position-number/
	extract tag, line, and offset numbers as decimal from hex_addr
	NOTE: "*" dereferences hex_addr, giving the value at the address rather than the address
	*/
	// take 3 ints to the left starting at index 6 (inclusive, starting at 1)
	int tag_num = (((1 << 3) - 1) & (*hex_addr >> (6 - 1)));
	// take 3 ints to the left starting at index 3 (inclusive) 
	int line_num = (((1 << 3) - 1) & (*hex_addr >> (3 - 1)));
	// take 2 ints to the left starting at index 1 (inclusive)
	int offset_num = (((1 << 2) - 1) & (*hex_addr >> (1 - 1)));

	if( cache[line_num]->tag == UNK ) { // cache line is empty
		//printf("look in line %d\n", line_num);
		//printf("line is empty.\n");
		*found = MISS;
		*replace = NO;

		// fetch from physical memory
		// xxx xxx _ _ is the block_num ... IS THIS NECCESSARY?
		int block_num = (((1 << 6) - 1) & (*hex_addr >> (3 - 1)));
		int start_addr = block_location[block_num];
		retVal = phy_memory[start_addr + offset_num];

		//printf("set tag to %d", tag_num);
		// replace tag in cache
		cache[line_num]->tag = tag_num;
	}
	else if ( cache[line_num]->tag == tag_num ) { // CACHE HIT
		//printf("look in line %d\n", line_num);
		//printf("found tag num at line num. Hit.");
		*found = HIT;
		*replace = NO;

		// get value
		int block_num = (((1 << 6) - 1) & (*hex_addr >> (3 - 1)));
		int start_addr = block_location[block_num];
		retVal = phy_memory[start_addr + offset_num];
	}
	else { // MISS, but cache line is not empty
		//printf("look in line %d \n", line_num);
		//printf("Wrong tag num at line num. Miss and replace.");
		*found = MISS;
		*replace = YES;

		// fetch from physical memory
		int block_num = (((1 << 6) - 1) & (*hex_addr >> (3 - 1)));
		int start_addr = block_location[block_num];
		retVal = phy_memory[start_addr + offset_num];

		// replace tag in cache
		cache[line_num]->tag = tag_num;
	}
	
} 

// Fully Associative
else { 
	// starting at index 6 (inclusive), take 3 ints to the left
	int tag_num= (((1 << 6) - 1) & (*hex_addr >> (3 - 1)));
	// starting at index 1 (inclusive), take 2 ints to the left
	int offset_num= (((1 << 2) - 1) & (*hex_addr >> (1 - 1)));

	for ( int line=0; line < NUM_OF_LINES; line++ ) { // search cache for tag_num
		if(cache[line]->tag == tag_num) { // CACHE HIT
			*found = HIT;
			*replace = NO;

			// get value
			int start_addr = block_location[tag_num];
			retVal = phy_memory[start_addr + offset_num];

			// update cache hit_count
			cache[line]->hit_count += 1;

			break;
		}
	}
	if(*found == MISS) { // the address is not in the cache
		*found == MISS; // for clarity
		
		// replacement function (LFU)
		int min = cache[0]->hit_count;
		int min_inx = 0;
		for ( int i=1; i < NUM_OF_LINES; i++ ) {
			if( cache[i]->hit_count < min){
				min = cache[i]->hit_count;
				min_inx = i;
			}
		}

		if(min == 0) { // empty cache line
			*replace = NO;
		}
		else {
			*replace = YES; // not an empty cache line
		}

		// fetch value from main memory
		int start_addr = block_location[tag_num];
		retVal = phy_memory[start_addr + offset_num];

		// update cache with tag and set hit_count to 1
		cache[min_inx]->tag = tag_num;
		cache[min_inx]->hit_count = 1;
	}
}

/*
printf("\ncache\n");

for ( int i=0; i < NUM_OF_LINES; i++ ) {
			printf("%d hit_count %d   ", i, cache[i]->hit_count);
			printf("tag %d", cache[i]->tag);
			printf("\n");
		}
*/

return retVal;

} // end cread function


void cprint() {

	unsigned int line;

	printf("\n---------------------------------------------\n");
	printf("line\ttag\tnum of hits\n");
	printf("---------------------------------------------\n");

	for ( line=0; line<NUM_OF_LINES; line++ ) { 

		if ( cache[line]->tag == UNK ) {
			
			printf("%d\t%d\t%d\n", line, cache[line]->tag, cache[line]->hit_count );

		} else {

			printf("%d\t%02X\t%d\n", line, cache[line]->tag, cache[line]->hit_count );

		}

	}

} // end cprint function
