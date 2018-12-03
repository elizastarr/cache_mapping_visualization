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

int initializeCaches( unsigned int number_of_lines ) {

	int line, retVal;
	retVal = OK;
	int unk_block[4] = {UNK, UNK, UNK, UNK};

	dm_cache = malloc( sizeof( cache_line ) * number_of_lines );
	fa_cache = malloc( sizeof( cache_line ) * number_of_lines );
	sa_cache = malloc( sizeof( cache_line ) * number_of_lines );

	if ( dm_cache != UNK ) {

		// loops through lines of cache from 0-7
		for ( line=0; line<number_of_lines; line++ ) {

			dm_cache[line] = (cache_line*) malloc( sizeof( cache_line ) );
			dm_cache[line]->tag = UNK;
			dm_cache[line]->hit_count = ZERO;
			dm_cache[line]->cache_block = &unk_block;

			fa_cache[line] = (cache_line*) malloc( sizeof( cache_line ) );
			fa_cache[line]->tag = UNK;
			fa_cache[line]->hit_count = ZERO;
			fa_cache[line]->cache_block = &unk_block;

			sa_cache[line] = (cache_line*) malloc( sizeof( cache_line ) );
			sa_cache[line]->tag = UNK;
			sa_cache[line]->hit_count = ZERO;
			sa_cache[line]->cache_block = &unk_block;

		}

	} else
		retVal = FAIL;

	return retVal;

} // end initializeCache function


// begin dm_simulation function
void dm_simulation(){

	int found = MISS;
	int replace = NO;

	// Initalize cache-monitoring variables
	int cache_hit_count = 0;
	int cache_miss_count = 0;
	int cache_replace_count = 0;

	/*
	// Initalize the cache
	cache_line** dm_cache;
	initializeCache(dm_cache, NUM_OF_LINES);
	*/

	// Begin the dm_simulation
	for(int i = 0; i < num_addresses; i++){

		sem_wait(&dm_sem);
		fprintf(stderr, " DM: %d\n", i);
		//---------------------------------

		int tag_num = (addresses[i]) >> 5;
    	int line_num = ( (addresses[i]) & 28 ) >> 2;
    	int offset_num = (addresses[i]) & 3;

		if(dm_cache[line_num]->tag == UNK){
			if( CACHE_DEBUG ){ printf("Line %d is empty!\n", line_num); }

			found = MISS;
			replace = NO;
			dm_cache[line_num]->cache_block = cwrite(tag_num);
			cache_miss_count++;

			//fetch from physical memory
			//int block_num = (((1 << 6) - 1) & (addresses[i] >> (3 - 1)));
			//int start_addr = block_location[block_num];
			//printf("%X\n", phy_memory[start_addr + offset_num]);

			//update the cache
			dm_cache[line_num]->tag = tag_num;
		}

		else if(dm_cache[line_num]->tag == tag_num){
			if(CACHE_DEBUG){ printf("Cache hit!\n"); }

			found = HIT;
			replace = NO;
			cache_hit_count++;

			//get the value from the cache
        	printf("%X\n", dm_cache[line_num]->cache_block[offset_num]);
        	//printf("%X\n", phy_memory[start_addr + offset_num]);
		}

		else{
			if(CACHE_DEBUG){ printf("Cache miss!\n"); } // this printf always runs

			found = MISS;
			replace = YES;
			cache_miss_count++;
			cache_replace_count++;

			//int block_num = (((1 << 6) - 1) & (addresses[i] >> (3 - 1)));
        	//int start_addr = block_location[block_num];
        	dm_cache[line_num]->cache_block = cwrite(tag_num);
        	dm_cache[line_num]->tag = tag_num;
		}

		/* PRINT CACHE */
		printf("%-10s%-15s%-10s\n", "Index", "hit_count", "tag");
		for(int i = 0; i < NUM_OF_LINES; i++){
			printf("%-10d%-15d%-10d\n", i, dm_cache[i]->hit_count, dm_cache[i]->tag);
		}

		sem_post(&fa_sem);

	}
} // end dm_simulation function


// begin fa_simulation function
void fa_simulation(int repl_algo){

	int found = MISS;
	int replace = NO;

	// Initalize cache-monitoring variables
	int cache_hit_count = 0;
	int cache_miss_count = 0;
	int cache_replace_count = 0;

	/*
	// Initalize the cache
	cache_line** fa_cache;
	initializeCache(fa_cache, NUM_OF_LINES);
	*/

	// Begin fa_simulation
	for(int i = 0; i < num_addresses; i++){

		sem_wait(&fa_sem);
		fprintf(stderr, " FA: %d\n", i);
		//---------------------------------

		//Initalize address bit values
		int tag_num = (addresses[i]) >> 5;
    	int offset_num = (addresses[i]) & 3;

		for(int line = 0; line < NUM_OF_LINES; line++){
			if(fa_cache[line]->tag == tag_num){
				if(CACHE_DEBUG){printf("Cache hit!\n");}

				found = HIT;
				replace = NO;
				cache_hit_count++;

				//get value cache
				//printf(fa_cache[line]->cache_block[offset_num]);

				//update cache hit_count
				fa_cache[line]->hit_count += 1;
				break;
			}
		}

		if(found == MISS){
			found = MISS;
			cache_miss_count++;

			//replace based on LRU
			if(repl_algo == LRU){
				int min = fa_cache[0]->hit_count;
				int min_inx = 0;
				for(int i = 0; i < NUM_OF_LINES; i++){
					if(fa_cache[i]->hit_count < min){
						min = fa_cache[i]->hit_count;
						min_inx = i;
					}
				}

				if(min == 0) {
					printf("Empty line found\n");
					replace = NO;
				} else {
					replace = YES;
					cache_replace_count++;
				}
				//update cache and set hit_count to 1
				fa_cache[min_inx]->tag = tag_num;
				fa_cache[min_inx]->hit_count = 1;

			}
			else if(repl_algo == RR){
				int rand_line = rand() % 8;
				fa_cache[rand_line]->cache_block = cwrite(tag_num);
				fa_cache[rand_line]->hit_count = 1;
				fa_cache[rand_line]->tag = tag_num;
				cache_replace_count++;
			}
		}

		/* PRINT CACHE */
		printf("%-10s%-15s%-10s\n", "Index", "hit_count", "tag");
		for(int i = 0; i < NUM_OF_LINES; i++){
			printf("%-10d%-15d%-10d\n", i, fa_cache[i]->hit_count, fa_cache[i]->tag);
		}

		sem_post(&sa_sem);

	}
} // end fa_simulation function


// begin sa_simulation function
void sa_simulation(unsigned int* set_size, int repl_algo){

	int found = MISS;
	int replace = NO;
	int num_sets = NUM_OF_LINES / *set_size;

	//TODO: We should add a check that the set_size is an acceptable divisor of NUM_OF_LINES

	// Initalize cache-monitoring variables
	int cache_hit_count = 0;
	int cache_miss_count = 0;
	int cache_replace_count = 0;

	/*
	// Initalize the cache
	cache_line** sa_cache;
	initializeCache(sa_cache, NUM_OF_LINES);
	*/

	// Begin the simulation
	for(int i = 0; i < num_addresses; i++){

		sem_wait(&sa_sem);
		fprintf(stderr, " SA: %d\n", i);
		//---------------------------------

		//Initalize address bit values
		int index_num = UNK;
		int tag_num = UNK;
		int offset_num = UNK;

		if(*set_size == TWO_WAY){
			index_num = ((addresses[i]) & 12) >> 2;
			tag_num = (addresses[i]) >> 4;
			offset_num = (addresses[i]) & 3;
		}
		else if(*set_size == FOUR_WAY){
			index_num = ((addresses[i]) & 4) >> 2;
			tag_num = (addresses[i]) >> 3;
			offset_num = (addresses[i]) & 3;
		}

		//loop through the sets
		for (int set = 0; set < num_sets; set++){
			if (set == index_num){
				//loop through each line in the set to check the tags
				for ( int set_line = 0; set_line < *set_size; set_line ++ ){
					int sa_line = (2 * set) + set_line;

					if ( sa_cache[sa_line]->tag == tag_num ){
						if (CACHE_DEBUG) { printf("Cache hit!\n"); }
						found = HIT;
						replace = NO;
						cache_hit_count++;

						//int start_addr = block_location[tag_num];
						//printf("%X\n", phy_memory[start_addr + offset_num]);
						// access the value via
						// sa_cache[sa_line]->cache_block[offset_num];

						sa_cache[sa_line]->hit_count += 1;
						break;
					}
					else {
						if (sa_cache[sa_line]->tag == UNK){
							if (CACHE_DEBUG){printf("Empty line!\n");}
							cache_miss_count++;
							found = NO;
							replace = NO;

							int start_addr = block_location[tag_num];
							//printf("%X\n", phy_memory[start_addr + offset_num]);

							sa_cache[sa_line]->tag = tag_num;
							sa_cache[sa_line]->hit_count = 1;
							sa_cache[sa_line]->cache_block = cwrite(tag_num);
							break;
						}
					}
				}

				//replace a line in the set using LRU
				if (found == MISS){

					if ( CACHE_DEBUG ) { printf("Cache miss!\n"); }
					found = MISS;
					cache_miss_count++;

					if(repl_algo == LRU){

						int min_inx = 0;
						int min = sa_cache[(*set_size) * set]->hit_count;
						for (int i = 0; i < *set_size; i++){
							int sa_cache_line = ((*set_size) * set) + i;
							if (sa_cache[sa_cache_line]->hit_count < min){
								min = sa_cache[sa_cache_line]->hit_count;
								min_inx = i;
							}
						}

						if ( min == 0 ) {
							printf("Empty line found!\n");
							replace = NO;
						} else {
							replace = YES;
							cache_replace_count++;
						}

						//update the cache and set hit count to 1
						sa_cache[(*set_size) * set + min_inx]->tag = tag_num;
						sa_cache[(*set_size) * set + min_inx]->hit_count = 1;
						sa_cache[(*set_size) * set + min_inx]->cache_block = cwrite(tag_num);
					}
					else if(repl_algo == RR){
						int rand_line = rand() % 2;
						sa_cache[set + rand_line]->cache_block = cwrite(tag_num);
						sa_cache[set + rand_line]->tag = tag_num;
						sa_cache[set + rand_line]->hit_count = 1;
					}

				}
			}
		}

		/* PRINT CACHE */
		printf("%-10s%-15s%-10s\n", "Index", "hit_count", "tag");
		for(int i = 0; i < NUM_OF_LINES; i++){
			printf("%-10d%-15d%-10d\n", i, sa_cache[i]->hit_count, sa_cache[i]->tag);
		}

		sem_post(&dm_sem);

	}
} // end sa_simulation function

void cprint(cache_line ** cache) {

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

int* cwrite(int tag_num){
	static int mem_line[4] = {UNK, UNK, UNK, UNK};
	int start_addr = block_location[tag_num];
	for(int word = 0; word < 4; word++){
		mem_line[word] = phy_memory[start_addr + word];
	}
	return &mem_line;
}
