#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

typedef struct range {
    uint32_t start;
    uint32_t length;
} range_t;

typedef struct seed {
    range_t temp_seed;
    range_t seed;
    uint8_t duplicate;
} __attribute__((packed)) seed_t;


int main(int argc, char *argv[]) {

    if(argc < 2) {
        printf("%s <input_file>\n", argv[0]);
        goto error;

    }

    FILE *file = fopen(argv[1], "r");
    if(!file) {
        printf("Could not open file\n");
        goto error;
    }

    fseek(file, 0, SEEK_END);

    size_t size = ftell(file);

    fseek(file, 0, SEEK_SET);


    char *input = malloc(size * sizeof(char));
    if(!input) {
        printf("Could not allocate memory\n");
        goto error;
    }

    fread(input, size, 1, file);

    char *c = input;

    seed_t *seeds_ptr = (seed_t *)input;

#define MODE_SEEDS 0
#define MODE_RANGE 1

#define RANGE_SET_DEST 0
#define RANGE_SET_SRC 1
#define RANGE_SET_LEN 2


    int mode = MODE_SEEDS,
        seeds_amt = 0,
        scanning = 0,
        range_set = RANGE_SET_SRC,
        temp_seeds_amt = 0,
        seeds_amt_new = 0;

    uint32_t source_range_start = 0,
            dest_range_start = 0;

    c = strchr(c, ':') + 1;

    while(c && c < input + size) {
        if(!scanning && isdigit(*c)) {
            if(!mode) {
                if(range_set==RANGE_SET_SRC) {
                    seeds_ptr[temp_seeds_amt].temp_seed.start = atoi(c);
                    range_set = RANGE_SET_LEN;
                } else {
                    seeds_ptr[temp_seeds_amt].temp_seed.length = atoi(c);
                    temp_seeds_amt++;
                    range_set = RANGE_SET_SRC;
                }
            }
            else {
                if(range_set == RANGE_SET_DEST) {
                    dest_range_start = atoi(c);
                    range_set = RANGE_SET_SRC;
                } else if(range_set == RANGE_SET_SRC) {
                    source_range_start = atoi(c);
                    range_set = RANGE_SET_LEN;
                } else {
                    int range_length = atoi(c);
                    uint32_t max = source_range_start + range_length;
                    uint32_t min = source_range_start;

                    for(int i = 0; i < seeds_amt; i++) {
                        if(seeds_ptr[i].seed.start < max
                            && seeds_ptr[i].seed.start >= min ||
                        seeds_ptr[i].seed.start + seeds_ptr[i].seed.length < max
                            && seeds_ptr[i].seed.start + seeds_ptr[i].seed.length >= min) //maybe
                        {
                            if(c - (input + sizeof(seed_t)*seeds_amt) < 0) {
                                printf("Out of memory! %d\n", seeds_amt);
                                return 1;
                            }

                            if(seeds_ptr[i].seed.start + seeds_ptr[i].seed.length > max) {
                                seeds_ptr[seeds_amt_new].seed.length = seeds_ptr[i].seed.start + seeds_ptr[i].seed.length - max;
                                seeds_ptr[seeds_amt_new].seed.start = max;
                                seeds_ptr[seeds_amt_new].duplicate = 1;

                                seeds_ptr[i].seed.length -=  seeds_ptr[seeds_amt_new].seed.length;

                                seeds_amt_new++;
                            }
                            if(seeds_ptr[i].seed.start < min) {

                                seeds_ptr[seeds_amt_new].seed.length = min - seeds_ptr[i].seed.start;
                                seeds_ptr[seeds_amt_new].seed.start =  seeds_ptr[i].seed.start;
                                seeds_ptr[seeds_amt_new].duplicate = 1;

                                seeds_ptr[i].seed.length -= seeds_ptr[seeds_amt_new].seed.length;
                                seeds_ptr[i].seed.start =  min;

                                seeds_amt_new++;
                            }

                            seeds_ptr[temp_seeds_amt].temp_seed.length = seeds_ptr[i].seed.length;
                            seeds_ptr[temp_seeds_amt].temp_seed.start = seeds_ptr[i].seed.start - min + dest_range_start;
                            temp_seeds_amt++;

                            seeds_ptr[i].duplicate = 0;
                        }
                    }

                    source_range_start = 0;
                    dest_range_start = 0;
                    range_set = RANGE_SET_DEST;
                }
            }
            scanning = 1;
        }
        if(*c == ':') {
            if(!mode) {
                mode = MODE_RANGE;
                range_set = RANGE_SET_DEST;
            } else {
                for(int i = 0; i < seeds_amt_new; i++) {
                    if(seeds_ptr[i].duplicate) {
                        seeds_ptr[temp_seeds_amt].temp_seed.start = seeds_ptr[i].seed.start;
                        seeds_ptr[temp_seeds_amt].temp_seed.length = seeds_ptr[i].seed.length;
                        temp_seeds_amt++;
                    }
                }
            }
            for(int i = 0; i < temp_seeds_amt; i++) {
                seeds_ptr[i].seed.start = seeds_ptr[i].temp_seed.start;
                seeds_ptr[i].seed.length = seeds_ptr[i].temp_seed.length;
                seeds_ptr[i].duplicate = 1;
            }

            seeds_amt = temp_seeds_amt;
            seeds_amt_new = seeds_amt;
            temp_seeds_amt = 0;

        }

        if(*c == ' ') {
            scanning = 0;
        }
        if(*c == '\n') {
            scanning = 0;
        }

        c++;
    }

    for(int i = 0; i < seeds_amt; i++) {
        if(seeds_ptr[i].duplicate) {
            seeds_ptr[temp_seeds_amt].temp_seed.start = seeds_ptr[i].seed.start;
            seeds_ptr[temp_seeds_amt].temp_seed.length = seeds_ptr[i].seed.length;
            temp_seeds_amt++;
        }
    }
    uint32_t answer = seeds_ptr[0].temp_seed.start;

    for(int i = 1; i < temp_seeds_amt; i++) {
        if(seeds_ptr[i].temp_seed.start < answer)
            answer = seeds_ptr[i].temp_seed.start;
    }

    printf("The answer is %u\n", answer);

    free(input);
    return 0;
error:
    return 1;
}
