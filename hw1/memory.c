
// -----------------------------------
// CSCI 340 - Operating Systems
// Fall 2018
// memory.c header file
// 
// Homework 1
//
// -----------------------------------

#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

unsigned int readMemoryFile( char* file_name ) {

	// -------------------------------------
	// Please do not modify this code
	// -------------------------------------

	int addr_cnt = 0;
	int addr_bits = 0;
	int r_flag;

	FILE* fhnd;

    	fhnd = fopen( file_name, "r" );

    	if ( fhnd != NULL ) { // if the file is not empty

		phy_memory = malloc( sizeof(unsigned int)*MAX_SIZE );

		r_flag = fscanf( fhnd, "%x\n", &phy_memory[addr_cnt] );

		while ( r_flag != EOF ) {

			addr_cnt++;

			r_flag = fscanf( fhnd, "%x\n", &phy_memory[addr_cnt] );

		}

		phy_memory[addr_cnt] = '\0'; // end character?
        	

    	} else addr_cnt = READ_ERROR;

    	fclose( fhnd );

	if ( MEM_DEBUG ) {
		printf("Number of bytes read %d\n", addr_cnt );
	}

	if ( addr_cnt != READ_ERROR ) // ideally addr_cnt = 256
		addr_bits = log2( addr_cnt ); // ideally addr_bits = 8
	else
		addr_bits = addr_cnt; // set addr_bits to READ_ERROR

	return addr_bits;

} // end readMemoryFile function

int numberOfBlocks( unsigned int addr_bits, unsigned int num_block_offset_bits ) {

	/* TODO: You complete code
// ------------------------------------
// Function prototype that computes the
// number of blocks in physical memory given the number of 
// addressable bits.
// 
//
// Arguments:	number of addressable bits
//		number of block offset bits
//
// Return:     	number of blocks (no error)
//		FAIL (error)
	*/
	
	// NOTE: addr_bits is the number of bits in an address and a word

	// compute the block size
	int num_words_in_block = exp2(num_block_offset_bits);
	// compute the number of unique addresses in physical memory given the number of bits in an address
	int num_uniq_addr = exp2(addr_bits);
	//compute the number of blocks in physical memory
	return num_uniq_addr/num_words_in_block;

} // end numberOfBlocks function



void initialzeBlockPointers( unsigned int num_of_blocks, unsigned int num_block_offset_bits  ) {

	int i;

	// returns start address of memory allocated to block_location
	block_location = malloc( sizeof( unsigned int) * num_of_blocks );

	for ( i=0; i<num_of_blocks; i++ ) {

		// block location equals the block number times the block size
		block_location[i] = i*exp2( num_block_offset_bits );

		if ( MEM_DEBUG ) {
			printf("starting block address (base-16) %02X (%d base-10)\n", block_location[i], block_location[i] );
		}

	}

} // end initalizeBlockPointers function
