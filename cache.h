// -----------------------------------
// CSCI 340 - Operating Systems
// Fall 2018
// cache.h header file
//
// Final Project: Joshua Glass & Eliza Starr
//
// -----------------------------------

#include <semaphore.h>
#include <pthread.h>

// --------------------------------
// CONSTANTS
// --------------------------------
// RETURN VALUES
#define OK 0
#define FAIL -1

// DEBUG FLAGS
#define CACHE_DEBUG 1

// Simulator parameters
#define NUM_OF_LINE_BITS_DM 3
#define NUM_OF_TAG_BITS_DM 3

#define NUM_OF_INDEX_BITS_2WAY_SA 2
#define NUM_OF_TAG_BITS_2WAY_SA 4
#define TWO_WAY 2
#define FOUR_WAY 4

#define NUM_OF_INDEX_BITS_4WAY_SA 1
#define NUM_OF_TAG_BITS_4WAY_SA 5

#define NUM_OF_TAG_BITS_FA 6
#define NUM_OF_LINES 8
#define LRU 1
#define RR 2



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
    int* cache_block;

} cache_line;

struct thread_args {
       unsigned int* set_size;
	   unsigned int* repl_algo;
};

// --------------------------------
// GLOBAL Variables
// --------------------------------

sem_t dm_sem;
sem_t fa_sem;
sem_t sa_sem;

cache_line** dm_cache;
cache_line** fa_cache;
cache_line** sa_cache;

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


int initializeCache( cache_line** cache, unsigned int number_of_lines, int repl_algo );


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

void dm_simulation();


// ------------------------------------
// Function prototype that creates and
// runs a simulation for a Fully
// Associative cache. Tracks cache hits and misses and
// the number of line replacements
//
// Return: OK if successful, FAIL on error

void fa_simulation(int repl_algo);


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

void sa_simulation(unsigned int* set_size, unsigned int* repl_algo);


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

void cprint(cache_line** cache);


int* cwrite(int tag_num);
