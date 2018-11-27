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

int initializeCache(cache_line** cache, unsigned int number_of_lines ) {

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


//begin dm_simulation function
int dm_simulation() {
	int found = MISS;
	int replace;
	int num_addrs = sizeof addresses / sizeof *addresses;
	int retVal = FAIL;

	//Initalize variables for monitoring cache performance
	int cache_hit_count = 0;
	int cache_miss_count = 0;
	int cache_replace_count = 0;

	//Initalize the cache for the simulation
	cache_line** dm_cache; 
	initializeCache(dm_cache, NUM_OF_LINES);

	//Begin the simulation
	for(int addr = 0; addr < num_addrs; addr++){

		int tag_num = (addresses[addr]) >> 5;
    	int line_num = ( (addresses[addr]) & 28 ) >> 2;
    	int offset_num = (addresses[addr]) & 3;

		if(dm_cache[line_num]->tag == UNK){
			if(CACHE_DEBUG){printf("Line %d is empty!\n", line_num);}

			found = MISS;
			replace = NO;

			//fetch from physical memory
			int block_num = (((1 << 6) - 1) & (addresses[addr] >> (3 - 1)));
			int start_addr = block_location[block_num];
			retVal = phy_memory[start_addr + offset_num];

			//update the cache
			dm_cache[line_num]->tag = tag_num;
		}

		else if(dm_cache[line_num]->tag == tag_num){
			if(CACHE_DEBUG){printf("Cache hit!\n");}

			found = HIT;
			replace = NO;
			cache_hit_count++;

			//get the value from the cache
			int block_num = (((1 << 6) - 1) & (addresses[addr] >> (3 - 1)));
        	int start_addr = block_location[block_num];
        	retVal = phy_memory[start_addr + offset_num];
		}

		else{
			if(CACHE_DEBUG){printf("Cache miss!\n");}

			found = MISS;
			replace = YES;
			cache_miss_count++;
			cache_replace_count++;

			int block_num = (((1 << 6) - 1) & (addresses[addr] >> (3 - 1)));
        	int start_addr = block_location[block_num];
        	retVal = phy_memory[start_addr + offset_num];

        	dm_cache[line_num]->tag = tag_num;
		}
	}

	return retVal;

}//end dm_simulation function


//begin fa_simulation function
int fa_simulation(){
	int found = MISS;
	int replace;
	int num_addrs = sizeof addresses / sizeof *addresses;
	int retVal = FAIL;

	//Initalize variables for monitoring cache performance
	int cache_hit_count = 0;
	int cache_miss_count = 0;
	int cache_replace_count = 0;

	///Initalize the cache for the simulation
	cache_line** fa_cache; 
	initializeCache(fa_cache, NUM_OF_LINES);

	//begin fa_simulation
	for(int addr = 0; addr < num_addrs; addr++){

		//Initalize address bit values
		int tag_num = (addresses[addr]) >> 5;
    	int offset_num = (addresses[addr]) & 3;

		for(int line = 0; line < NUM_OF_LINES; line++){
			if(fa_cache[line]->tag == tag_num){
				if(CACHE_DEBUG){printf("Cache hit!\n");}

				found = HIT;
				replace = NO;
				cache_hit_count++;

				//get value from memory
				int start_addr = block_location[tag_num];
				retVal = phy_memory[start_addr + offset_num];

				//update cache hit_count
				fa_cache[line]->hit_count += 1;
				break;
			}
		}

		if(found == MISS){
			found = MISS;
			cache_miss_count++;

			//replace based on LRU
			int min = fa_cache[0]->hit_count;
			int min_inx = 0;
			for(int i = 0; i < NUM_OF_LINES; i++){
				if(fa_cache[i]->hit_count < min){
					min = fa_cache[i]->hit_count;
					min_inx = i;
				}
			}

			if ( min == 0 ) {
				printf("Empty line found\n"); 
				replace = NO;
			} else {
				replace = YES; 
				cache_replace_count++;
			}

			//fetch the value from memory
			int start_addr = block_location[tag_num];
			retVal = phy_memory[start_addr + offset_num];

			//update cache and set hit_count to 1
			fa_cache[min_inx]->tag = tag_num;
			fa_cache[min_inx]->hit_count = 1;

		}
	}

	return retVal;

}


//begin sa_simulation function
int sa_simulation(unsigned int* set_size){
	int found = MISS;
	int replace;
	int num_addrs = sizeof addresses / sizeof *addresses;
	int retVal = FAIL;
	int num_sets = NUM_OF_LINES / *set_size;

	//Initalize variables for monitoring cache performance
	int cache_hit_count = 0;
	int cache_miss_count = 0;
	int cache_replace_count = 0;

	//Initalize the cache for the simulation
	cache_line** sa_cache; 
	initializeCache(sa_cache, NUM_OF_LINES);

	//Begin the simulation
	for(int addr = 0; addr < num_addrs; addr++){

		//Initalize address bit values for 2-way SA
		if(*set_size == TWO_WAY){
			int index_num = ((addresses[addr]) & 12) >> 2;
			int tag_num = (addresses[addr]) >> 4;
			int offset_num = (addresses[addr]) & 3;
		}
		else if(*set_size == FOUR_WAY){
			int index_num = ((addresses[addr]) & 4) >> 2;
			int tag_num = (addresses[addr]) >> 3;
			int offset_num = (addresses[addr]) & 3;
		}

		//loop through the sets
		for(int set = 0; set < num_sets; set++){
			if(set == index_num){
				//loop through each line in the set to check the tags
				for(int set_line = 0; set_line < *set_size; set_line ++){
					int sa_line = (2 * set) + set_line;

					if(sa_cache[sa_line]->tag == tag_num){
						if(CACHE_DEBUG){printf("Cache hit!\n");}
						found = HIT;
						replace = NO;
						cache_hit_count++;

						int start_addr = block_location[tag_num];
						retVal = phy_memory[start_addr + offset_num];

						sa_cache[sa_line]->hit_count += 1;
						break;
					}
					else{
						if(sa_cache[sa_line]->tag = UNK){
							if(CACHE_DEBUG){printf("Empty line!\n");}
							cache_miss_count++;
							found = NO;
							replace = NO;

							int start_addr = block_location[tag_num];
							retVal = phy_memory[start_addr + offset_num];

							sa_cache[sa_line]->tag = tag_num;
							sa_cache[sa_line]->hit_count = 1;
							break;
						}
					}
				}
				//replace a line in the set using LRU
				if(found == MISS){
					if(CACHE_DEBUG){printf("Cache miss!\n");}
					found = MISS;
					cache_miss_count++;

					int min_inx = 0;
					for(int i = 0; i < *set_size; i++){
						sa_cache_line = (*set_size * set) + i;
						if(sa_cache[sa_cache_line]->hit_count < min){
							min = sa_cache[sa_cache_line]->hit_count;
							min_inx = i;
						}
					}

					if(min == 0){printf("Empty line found!\n"); replace = NO;}

					else{replace = YES; cache_replace_count++;}

					//fetch the value from memory
					int start_addr = block_location[tag_num];
					retVal = phy_memory[start_addr + offset_num];

					//update the cache and set hit count to 1
					sa_cache[(*set_size) * set + min_inx]->tag = tag_num;
					sa_cache[(*set_size) * set + min_inx]->hit_count = 1;
				}
			}
		}
	}

	return retVal;

}

void cprint( cache_line** cache ) {

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
