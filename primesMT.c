// Alex Teav

// R. Jesse Chaney
// rchaney@pdx.edu

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <values.h>
#include <math.h>
#include <sys/time.h>
#include <stdint.h>
#include <getopt.h>
#include <math.h>

#ifndef FALSE
# define FALSE 0
#endif // FALSE
#ifndef TRUE
# define TRUE 1
#endif // TRUE

#define OPTIONS "t:u:hv"

typedef struct BitBlock_s {
    uint32_t bits;
    pthread_mutex_t mutex;
} BitBlock_t;

static int num_threads = 1;
static int upper_bound = 10240;
BitBlock_t *bitarray = NULL;
static int verbose = 0;

void alloc_bitarray(void);
void *prime_sieve(void *void_tid);
void display_primes(void);


void alloc_bitarray(void)
{
    int size = (upper_bound / 32) + 1;

    if (verbose == 1)
    {
        fprintf(stderr, "*** Number of uints to represent %d bits: %d ***\n", upper_bound, size);
    }

    bitarray = malloc(sizeof(BitBlock_t) * size);

    for (int i = 0; i < size; i++) // initialize bitarray members
    {
        bitarray[i].bits = 0;
        pthread_mutex_init(&bitarray[i].mutex, NULL);
    }
}

void *prime_sieve(void *void_tid)
{
    long tid = (long) void_tid;
    uint32_t mask = 0x0;

    if (verbose == 1)
    {
        fprintf(stderr, "*** Thread %ld starting at %ld ***\n", tid, (3 + (tid * 2)));
    }

    for (int i = 3 + (tid * 2); (i * i) <= upper_bound; i += (2 * num_threads))
    {
        for (int factors = i * 2; factors <= upper_bound; factors += i)
        {
            mask = 0x1 << (factors % 32);

            /*if (verbose == 1)
            {
                fprintf(stderr, "**TESTING %d FOR PRIMALITY**\n", factors);
            }*/

            if (factors % 2 == 0 || (bitarray[factors / 32].bits & mask)) // if number is even OR already set
            {
                continue;
            }

            pthread_mutex_lock(&bitarray[factors / 32].mutex);
            bitarray[factors / 32].bits |= mask;
            pthread_mutex_unlock(&bitarray[factors / 32].mutex);
        }
    }

    pthread_exit(EXIT_SUCCESS);
}

void display_primes(void)
{
    uint32_t mask = 0x0;

    if (verbose == 1)
    {
        fprintf(stderr, "*** Displaying primes... ***\n");
    }

    printf("2\n"); // 2 is only even prime number

    for (int i = 3; i <= upper_bound; i += 2)
    {
       mask = 0x1 << (i % 32);

       if ((bitarray[i / 32].bits & mask) == 0)
       {
            printf("%d\n", i);
       }
    }    
}

int
main(int argc, char *argv[])
{
    pthread_t *threads = NULL;
    long tid = 0;

    {
        int opt = 0;

        while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
            switch (opt) {
                case 't':
                    // number of threads
                    num_threads = atoi(optarg);
                    break;
                case 'u':
                    // upper bound
                    upper_bound = atoi(optarg);
                    break;
                case 'v':
                    // verbose
                    verbose = 1;
                    fprintf(stderr, "*** VERBOSE SET TO 1 ***\n");
                    break;
                case 'h':
                    // helpful stuff
                    printf("%s: -t # -u #\n", argv[0]);
                    printf("\t-t #: number of threads\n");
                    printf("\t-u #: upper bound of values to test\n");
                    printf("\t-v: verbose processing; for diagnostic purposes\n");
                    printf("\t-h: this help text; lists all command line options\n");
                    exit(0);
                    break;
                default:
                    exit(EXIT_FAILURE);
                    break;
            }
        }
    }
    
    if (verbose == 1)
    {
        fprintf(stderr, "*** COMMAND LINE OPTIONS PARSED ***\n");
        fprintf(stderr, "*** FINDING PRIME NUMBERS UP TO %d ***\n", upper_bound);
    }

    // allocate memory for threads
    threads = malloc(num_threads * sizeof(pthread_t));

    if (verbose == 1)
    {
        fprintf(stderr, "*** ALLOCATED MEMORY FOR %d THREADS ***\n", num_threads);
    }

    // allocate and initialize memory for BitBlock_t array
    alloc_bitarray();

    // create all the threads
    for (tid = 0; tid < num_threads; tid++)
    {
        pthread_create(&threads[tid], NULL, prime_sieve, (void *) tid);
    }

    if (verbose == 1)
    {
        fprintf(stderr, "*** ALL THREADS STARTED ***\n");
    }

    // join with all the threads
    for (tid = 0; tid < num_threads; tid++) 
    {
        if (verbose == 1)
        {
            fprintf(stderr, "*** THREAD %ld EXITING ***\n", tid);
        }

        pthread_join(threads[tid], NULL);
    }

    if (verbose == 1)
    {
        fprintf(stderr, "*** ALL THREADS JOINED ***\n");
    }


    // output all prime numbers
    display_primes();

    
    if (verbose == 1)
    {
        fprintf(stderr, "*** DEALLOCATING AND FREEING MEMORY... ***\n");
    }

    // deallocate memory
    for (int i = 0; i < ((upper_bound / 32) + 1); i++)
    {
        pthread_mutex_destroy(&bitarray[i].mutex);
    }

    free(bitarray);
    free(threads);

    return EXIT_SUCCESS;
}
