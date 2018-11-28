// -----------------------------------
// CSCI 340 - Operating Systems
// Fall 2018
// memory.h header file
//
// Homework 1
//
// -----------------------------------

// --------------------------------
// CONSTANTS
// --------------------------------
// RETURN VALUES
#define OK 0
#define READ_ERROR 0
#define FAIL -1

// DEBUG FLAGS
#define MEM_DEBUG 0

// Simulator Parameters
#define NUM_BLOCK_OFFSET_BITS 2
#define MAX_SIZE 1000



// --------------------------------
// GLOBAL Variables
// --------------------------------
unsigned int* phy_memory;
unsigned int* block_location;
unsigned int* addresses;
unsigned int addr_bits;
unsigned int num_addresses;


// --------------------------------
// Prototype definitions
// --------------------------------

// ------------------------------------
// Function prototype that creates the
// phy_memory array by reading the bytes
// defined in the memory.txt file.
//
//
// Arguments:	character array (i.e. string)
//
// Return:     	Number of addressable bits (no error)
//		READ_ERROR (error)
//

unsigned int readMemoryFile( char* file_name );


// ------------------------------------
// Function prototype that computes the
// number of blocks given the number of
// addressable bits.
//
//
// Arguments:	number of addressable bits
//		number of block offset bits
//
// Return:     	number of blocks (no error)
//		FAIL (error)
//

int numberOfBlocks( unsigned int addr_bits, unsigned int num_block_offset_bits );

// ------------------------------------
// Function prototype that identifies the
// starting physcial memory address location
// for each block.
//
//
// Arguments:	number of blocks
//		number of block offset bits
//
// Return:     	nothing
//

void initialzeBlockPointers( unsigned int num_of_blocks, unsigned int num_block_offset_bits  );
