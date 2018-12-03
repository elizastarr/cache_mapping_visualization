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

#include <string.h>

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
			dm_cache[line]->cache_block = " ";

			fa_cache[line] = (cache_line*) malloc( sizeof( cache_line ) );
			fa_cache[line]->tag = UNK;
			fa_cache[line]->hit_count = ZERO;
			fa_cache[line]->cache_block = " ";
			
			sa_cache[line] = (cache_line*) malloc( sizeof( cache_line ) );
			sa_cache[line]->tag = UNK;
			sa_cache[line]->hit_count = ZERO;
			sa_cache[line]->cache_block = " ";
			
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

	// Begin the dm_simulation
	for(int i = 0; i < num_addresses; i++){

		sem_wait(&dm_sem);
		fprintf(stderr, "-------------------------------------------\nADDRESS %d\n-------------------------------------------\n\n", i);
		fprintf(stderr,"Direct Mapping:\n");
		
		//---------------------------------

		int tag_num = (addresses[i]) >> 5;
    	int line_num = ( (addresses[i]) & 28 ) >> 2;
    	int offset_num = (addresses[i]) & 3;

		if(dm_cache[line_num]->tag == UNK){
			if( CACHE_DEBUG ){ printf("Line %d is empty!\n", line_num); }

			found = MISS;
			replace = NO;
			dm_cache[line_num]->cache_block = cwrite(tag_num);
			//fprintf("value of cache_block %s", dm_cache[line_num]->cache_block);
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
        	//printf("%X\n", dm_cache[line_num]->cache_block[offset_num]);
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
			//fprintf("value of cache_block %s", dm_cache[line_num]->cache_block);
			//fprintf(stderr,"HEREE %s", dm_cache[line_num]->cache_block);
        	dm_cache[line_num]->tag = tag_num;
		}

		cprint(dm_cache);
		//sleep(3);
		sem_post(&fa_sem);

	}

	sem_wait(&dm_sem);

	fprintf(stderr, "-------------------------------------------\nSUMMARY STATISTICS\n-------------------------------------------\n\n");
	printf("Direct Mapping:\n");
	printf("%-15s%-15s%-10s\n", "Total hits", "Total misses", "Total replacements");
	printf("%-15d%-15d%-10d\n", cache_hit_count, cache_miss_count, cache_replace_count);

	sem_post(&fa_sem);

} // end dm_simulation function


// begin fa_simulation function
void fa_simulation(int repl_algo){


	int found = MISS;
	int replace = NO;

	// Initalize cache-monitoring variables
	int cache_hit_count = 0;
	int cache_miss_count = 0;
	int cache_replace_count = 0;

	// Begin fa_simulation
	for(int i = 0; i < num_addresses; i++){

		sem_wait(&fa_sem);
		fprintf(stderr, "Fully Associative:\n");
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
				fa_cache[min_inx]->cache_block = cwrite(tag_num);
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

		cprint(fa_cache);
		//sleep(3);
		sem_post(&sa_sem);

	}

	sem_wait(&fa_sem);

	printf("\nFully Associative:\n");
	printf("%-15s%-15s%-10s\n", "Total hits", "Total misses", "Total replacements");
	printf("%-15d%-15d%-10d\n", cache_hit_count, cache_miss_count, cache_replace_count);

	sem_post(&sa_sem);

} // end fa_simulation function


// begin sa_simulation function
void sa_simulation(unsigned int* set_size, unsigned int* repl_algo){

	int found = MISS;
	int replace = NO;
	int num_sets = NUM_OF_LINES / *set_size;

	// Initalize cache-monitoring variables
	int cache_hit_count = 0;
	int cache_miss_count = 0;
	int cache_replace_count = 0;

	// Begin the simulation
	for(int i = 0; i < num_addresses; i++){

		sem_wait(&sa_sem);
		fprintf(stderr, "Set Associative:\n");
		//---------------------------------

		//Initalize address bit values
		int index_num = UNK;
		int tag_num = UNK;
		int offset_num = UNK;

		if(*set_size == TWO_WAY){
			tag_num = (addresses[i]) >> 4;
			index_num = ((addresses[i]) & 12) >> 2;
			offset_num = (addresses[i]) & 3;
		}
		else if(*set_size == FOUR_WAY){
			tag_num = (addresses[i]) >> 3;
			index_num = ((addresses[i]) & 4) >> 2;
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
							found = MISS;
							replace = NO;

							//int start_addr = block_location[tag_num];
							//printf("%X\n", phy_memory[start_addr + offset_num]);

							sa_cache[sa_line]->tag = tag_num;
							sa_cache[sa_line]->hit_count = 1;
							sa_cache[sa_line]->cache_block = cwrite((addresses[i]) >> 5);
							break;
						}
						else{replace = YES; }
					}
				}

				//replace a line in the set using LRU
				if (found == MISS && replace == YES){

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
						sa_cache[(*set_size) * set + min_inx]->cache_block = cwrite((addresses[i]) >> 5);
					}
					else if(repl_algo == RR){
						int rand_line = rand() % 2;
						sa_cache[set + rand_line]->cache_block = cwrite((addresses[i]) >> 5);
						sa_cache[set + rand_line]->tag = tag_num;
						sa_cache[set + rand_line]->hit_count = 1;
					}

				}
			}
		}

		/* PRINT CACHE */

		cprint(sa_cache);
		fprintf(stderr, "\n\n");
		sleep(3);
		sem_post(&dm_sem);

	}

	sem_wait(&sa_sem);

	printf("\nSet Associative:\n");
	printf("%-15s%-15s%-10s\n", "Total hits", "Total misses", "Total replacements");
	printf("%-15d%-15d%-10d\n", cache_hit_count, cache_miss_count, cache_replace_count);

	sem_post(&dm_sem);

} // end sa_simulation function

void cprint(cache_line ** cache) {

	printf("%-10s%-15s%-10s%-10s\n", "Index", "hit_count", "tag", "data");
		//fprintf(stderr, "%s", cache[1]->cache_block);
		
		for(int i = 0; i < NUM_OF_LINES; i++){
			printf("%-10d%-15d%-10d%-10s\n", i, cache[i]->hit_count, cache[i]->tag, cache[i]->cache_block);
		}
	printf("\n");

} // end cprint function

char* cwrite(int tag_num){
	
	static char mem_line[50];
	strcpy(mem_line, "");
	char temp[50] = "";
	int start_addr = block_location[tag_num];

	for(int word = 0; word < 4; word++){
		
		sprintf(temp, "%x", phy_memory[start_addr + word]);
		strcat(mem_line, temp);
		strcat(mem_line, " ");

	}
	//fprintf(stderr, "mem %s", mem_line);

	return mem_line;
}
