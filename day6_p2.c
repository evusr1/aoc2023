#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <math.h>



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

#define MODE_TIME 0
#define MODE_DISTANCE 1


    uint64_t answer = 0;
    uint64_t race_time = 0;
    uint64_t race_distance = 0;

    int mode = MODE_TIME;
    char *c = input + sizeof("Time:");
    while(c && c < input + size) {
        if(*c == '\n') {
            if(mode == MODE_TIME)
                mode = MODE_DISTANCE;
        }

        if(isdigit(*c)) {
            switch(mode) {
                case MODE_TIME:
                    race_time = (race_time*10) + *c - '0';
                    break;
                case MODE_DISTANCE:
                    race_distance = (race_distance*10) + *c - '0';
                    break;
                default:
                    break;
            }
        }
        c++;

    }

    for(uint64_t push_time = race_time - 1; push_time > 0; push_time--) {
        if(race_distance < ((race_time - push_time) * push_time))
            answer++;
    }

    printf("(bruteforce)The answer is %lu\n", answer);

    /*
     * race_distance < (race_time - push_time) * push_time
     * race_time*push_time - push_time^2 - race_distance = 0
     * push_time^2 - race_time*push_time + race_distance
     * race_time+/-sqrt(race_time^2 - 4(1)(race_distance)) / 2 (1)
     */

    double sq = sqrt(race_time*race_time-4*(race_distance - 1)) + 1;
    double upper = (race_time + sq)/2;
    double lower = (race_time - sq)/2;

    printf("(quadratic)The answer is %d\n", abs(upper - lower));


    free(input);
    return 0;
error:
    return 1;
}

