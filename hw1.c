#include <stdio.h>	// IO functions
#include <stdlib.h> 	// std lib function
#include <unistd.h>	// close function
#include <pthread.h> 
#include "memory.h"	// our code that represents physical memory
#include "cache.h"	// our code that represents cache



/**********************************************************************************************

HW1: CSCI 340 Operating Systems - Fall 2018

Develop a cache simulator that implements the two cache mapping functions (CMF):
1) direct mapping (DM)
2) fully associative (FA)

This will be an interactive simulator that allows the user to:
1) Select the CMF
2) Enter hex memory address locations
3) For the CMF selected in (1) determine if it's a cache-hit or -miss (and display to the user)
4) if a cache-miss, determine the cache line to be replaced for the choosen CMF.
   Note: for FA CMF, the replacement algorithm is least frequently used (LFU) that is based on 
	 minimum number of cache hits.

***********************************************************************************************/


#define RUNFOEVA 1


char* CMFS[] = { "Direct Mapping", "Fully Associative", "Set Associative" };
char* OP[] = { "MISS", "HIT" };
char* ROP[] = { "No Replacement", "Replacement" };


int main( int argc, char *argv[] ) {

	unsigned int num_blocks;
	unsigned int hex_addr;
	
	int byte;
	unsigned int found, replace;

	addr_bits = readMemoryFile( argv[1] );

	if ( addr_bits != READ_ERROR ) {

		/*
			---------------------------------------------------
				STEP 1:
			---------------------------------------------------
			Calculate the number of blocks then display to the
			user the number of physical memory addressable bits 
			and the total number of physical memory blocks.
		*/

		printf("\n------------------------\n");
		printf("[STEP 1] Setting up physical memory\n");
		printf("------------------------\n");

		num_blocks = numberOfBlocks( addr_bits, NUM_BLOCK_OFFSET_BITS );

		printf("Physical memory addressable bits = %d, total number of blocks = %d\n", addr_bits, num_blocks );

		/*
			---------------------------------------------------
				STEP 2:
			---------------------------------------------------
			Initialize the block points, i.e. determine the physical
			memory starting address for each block.
			
		*/

		printf("\n------------------------\n");
		printf("[STEP 2] Determining physical memory block locations\n");
		printf("------------------------\n");

		initialzeBlockPointers( num_blocks, NUM_BLOCK_OFFSET_BITS );

		printf("\n------------------------\n");
		printf("[STEP 3] Collecting addresses to be requested\n");
		printf("------------------------\n");

		num_addresses = readAddressesFile( "addresses.txt" );

		if ( num_addresses != READ_ERROR ) {

			printf("\n------------------------\n");
			printf("[STEP 3] Starting simulation\n");
			printf("------------------------\n");
			printf("To exit simulation press the 'Ctrl C' keys on your keyboard\n");

			/* CREATE AND RUN THREADS */
			pthread_t dm_t, fa_t, sa_t;
			pthread_create(&dm_t, NULL, (void *) dm_simulation, NULL );
			pthread_create(&fa_t, NULL, (void *) fa_simulation, NULL );
			pthread_create(&sa_t, NULL, (void *) sa_simulation, NULL );

		} else {

			printf("Unable to read the address file ( %s ) ... exiting\n", argv[1] );

		}

	} else {

		printf("Unable to read the memory file ( %s ) ... exiting\n", argv[1] );

	}

	return OK;
     
} // end main function

