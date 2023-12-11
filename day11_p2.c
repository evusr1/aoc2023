#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct galaxy {
    uint32_t x;
    uint32_t y;
} galaxy_t;


#define EXPAND_BY 1000000

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

    int galaxies_amt = 0,
        width = 0,
        height = 0;

    char *c = input;

    while(c && c < input + size) {
        if(*c=='#')
            galaxies_amt++;
        c++;
        if(!height) {
            width++;
            if(*c == '\n') {
                width++;
                height = size / width;
            }
        }

    }

    galaxy_t *galaxies = (galaxy_t *)malloc(galaxies_amt * sizeof(galaxy_t) + width*height*sizeof(uint32_t));

    if(!galaxies) {
        printf("Could not allocate memory\n");
        goto error;
    }

    uint32_t *expanses_cols = (uint32_t*)&galaxies[galaxies_amt];
    uint32_t *expanses_rows = (uint32_t*)&expanses_cols[width];

    int current_galaxy = 0,
        expanses_col_amt = 0,
        expanses_row_amt = 0;


    for(int y = 0; y < height; y++) {
        int galaxies_found = 0;

        for(int x = 0; x < width; x++) {
            if(input[y*width + x]=='#') {
                galaxies_found++;
                galaxies[current_galaxy].x = x;
                galaxies[current_galaxy++].y = y;

            }
        }

        if(galaxies_found == 0)
            expanses_row_amt++;
        else
            expanses_rows[y] = expanses_row_amt;

    }


    for(int x = 0; x < width; x++) {
        int galaxies_found = 0;

        for(int y = 0; y < height; y++) {
            if(input[y*width + x]=='#' || input[y*width + x]=='\n')
                galaxies_found++;
        }

        if(galaxies_found == 0)
            expanses_col_amt++;
        else
            expanses_cols[x] = expanses_col_amt;

    }


    uint64_t answer = 0;
    for(int i = 0; i < galaxies_amt; i++) {
        for(int j = i + 1; j < galaxies_amt; j++) {
            answer += abs(galaxies[j].x - galaxies[i].x)
                    + abs(galaxies[j].y - galaxies[i].y)
                    + abs(expanses_cols[galaxies[j].x] - expanses_cols[galaxies[i].x]) * (EXPAND_BY - 1)
                    + abs(expanses_rows[galaxies[j].y] - expanses_rows[galaxies[i].y]) * (EXPAND_BY - 1);
        }
    }

    printf("The answer is %ld\n", answer);


    free(input);
    free(galaxies);
    return 0;
error:
    return 1;
}

