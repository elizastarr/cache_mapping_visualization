// -----------------------------------
// CSCI 340 - Operating Systems
// Fall 2018
// cache.h header file
//
// Final Project: Joshua Glass & Eliza Starr
//
// -----------------------------------


// --------------------------------
// CONSTANTS
// --------------------------------
// RETURN VALUES
#define OK 0
#define FAIL -1

// DEBUG FLAGS
#define CACHE_DEBUG 0

// Simulator parameters
#define NUM_OF_LINE_BITS_DM 3
#define NUM_OF_TAG_BITS_DM 3

#define NUM_OF_INDEX_BITS_2WAY_SA 2
#define NUM_OF_TAG_BITS_2WAY_SA 4
#define TWO_WAY 4
#define FOUR_WAY 2

#define NUM_OF_INDEX_BITS_4WAY_SA 1
#define NUM_OF_TAG_BITS_4WAY_SA 5

#define NUM_OF_TAG_BITS_FA 6
#define NUM_OF_LINES 8

#define HIT 1
#define MISS 0
#define YES 1
#define NO 0
#define UNK -1
#define ZERO 0


// --------------------------------
// Structure definitions
// --------------------------------

typedef struct {

	int tag;
	int hit_count;

} cache_line;

/* --------------------------------
// GLOBAL Variables
// --------------------------------

// ------------------------------------
// Function prototype that initializes the
// cache array. Each element in the cache
// array is cache_line struct (see above).
//
//
// Arguments:	pointer to a cache variable (cache)
//              int number_of_lines
//
// Return:     	OK on success, FAIL on error
*/

int initializeCache( cache_line** cache, unsigned int number_of_lines );


/* ------------------------------------
// Function prototype that creates and
// runs a simulation for a Direct Mapped
// cache. Tracks cache hits and misses and
// the number of line replacements
//
// Arguments: int pointer to the address (hex_addr)
//            pointer to array of addresses to access
//
// Return: OK if successful, FAIL on error
*/

int dm_simulation(unsigned int* addresses[]);


/* ------------------------------------
// Function prototype that creates and
// runs a simulation for a Set Associative
// cache. Tracks cache hits and misses and
// the number of line replacements
//
// Arguments: set associativity value (set_size)
//
// Return: OK if successful, FAIL on error
*/

int sa_simulation(unsigned int* set_size, unsigned int* addresses[]);


// ------------------------------------
// Function prototype that creates and
// runs a simulation for a Fully
// Associative cache. Tracks cache hits and misses and
// the number of line replacements
//
// Return: OK if successful, FAIL on error

int fa_simulation(unsigned int* addresses[]);


// ------------------------------------
// Function prototype that you can use to
// visualize the cache. This can be used
// for debugging purposes.
//
//
// Arguments:	none
//
// Return:     	none
//

void cprint();
