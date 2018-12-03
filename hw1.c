#include <stdio.h>	// IO functions
#include <stdlib.h> 	// std lib function
#include <unistd.h>	// close function
#include <pthread.h> 
#include <semaphore.h>

#include "memory.h"	// our code that represents physical memory
#include "cache.h"	// our code that represents cache




/**********************************************************************************************

Project: CSCI 340 Operating Systems - Fall 2018
The work of: Josh Glass and Eliza Starr
Adapted code by: Dr. Munsell

Develop a cache simulator that implements the three cache mapping functions (CMF):
1) direct mapping (DM)
2) fully associative (FA)
3) set associative (SA)

This will be an interactive simulator that allows the user to:
1) Select one or all CMFs
2) Watch the chosen CMF(s) read given addresses from physical memory
3) Observe cache-hits or -misses and -replacements

***********************************************************************************************/

char* CMFS[] = { "Direct Mapping", "Fully Associative", "Set Associative" };

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
		printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
		printf("\n------------------------\n");
		printf("[STEP 1] Setting up physical memory\n");
		printf("------------------------\n");

		num_blocks = numberOfBlocks( addr_bits, NUM_BLOCK_OFFSET_BITS );
		printf("Success.\n");
		printf("Physical memory addressable bits = %d, total number of blocks = %d\n", addr_bits, num_blocks );

		/*
			---------------------------------------------------
				STEP 1.a:
			---------------------------------------------------
			Initialize the block points, i.e. determine the physical
			memory starting address for each block.
			Initialize the caches.
			
		*/

		initialzeBlockPointers( num_blocks, NUM_BLOCK_OFFSET_BITS );
		initializeCaches( NUM_OF_LINES );

		printf("\n------------------------\n");
		printf("[STEP 2] Collecting addresses to be read\n");
		printf("------------------------\n");

		num_addresses = readAddressesFile( "addresses.txt" );
		

		if ( num_addresses != READ_ERROR ) {

			printf("Success.\n");
			for(int i = 0; i < num_addresses; i++){
				printf("addresses[%d] = %d\n", i, addresses[i]);
			}

			printf("\n------------------------\n");
			printf("[STEP 3] User preferences\n");
			printf("------------------------\n");
			
			
			unsigned int* set_size = 2;
			unsigned int* repl_algo = 1;
			printf("Choose set size: 2 (two-way) or 4 (four-way): ");
			scanf( "%d", &set_size );
			printf("Choose replacement algorithm: 1 (LFU) or 2 (RR): ");
			scanf( "%d", &repl_algo );

			printf("\nReplacement Algorithm = %d, Set Size = %d \n", repl_algo, set_size);
			struct thread_args* args = malloc (sizeof (struct thread_args));
			args->set_size = set_size;
			args->repl_algo = repl_algo;


			printf("\n------------------------\n");
			printf("[STEP 4] Starting simulation\n");
			printf("------------------------\n");

			/* INITIALIZE SEMAPHORES */
			printf("Initializing semaphores.\n");

			sem_init(&dm_sem, 0, 1); // first DM runs
			sem_init(&fa_sem, 0, 0); // then FA runs
			sem_init(&sa_sem, 0, 0); // lastly SA runs

			/* CREATE AND RUN THREADS */
			printf("Running simulation threads.\n\n");
			
			pthread_t dm_t, fa_t, sa_t;

			if ( pthread_create(&dm_t, NULL, (void *) dm_simulation, NULL ) != 0)
        		perror("DM thread failed"), exit(1); 
			
			if ( pthread_create(&fa_t, NULL, (void *) fa_simulation, repl_algo ) != 0)
        		perror("FA thread failed"), exit(1); 
			
			if ( pthread_create(&sa_t, NULL, (void *) sa_simulation, args ) != 0)
        		perror("SA thread failed"), exit(1); 
			

			/* JOIN THREADS */

			if (pthread_join(dm_t, NULL) != 0)
        		perror("DM join failed"),exit(1);
			
			if (pthread_join(fa_t, NULL) != 0)
        		perror("FA join failed"),exit(1);

			if (pthread_join(sa_t, NULL) != 0)
        		perror("SA join failed"),exit(1);
			
		} else {

			printf("Unable to read the address file ( %s ) ... exiting\n", argv[1] );

		}

	} else {

		printf("Unable to read the memory file ( %s ) ... exiting\n", argv[1] );

	}

	return OK;
     
} // end main function

