#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

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


    int answer = 1;
    int race_times[4];
    int race_distances[4];

    int mode = MODE_TIME,
            current_index = 0,
            scanning = 0;

    char *c = input + sizeof("Time:");
    while(c && c < input + size) {
        if(*c == '\n') {
            if(mode == MODE_TIME) {
                mode = MODE_DISTANCE;
                current_index = 0;
            }
        }

        if(*c == ' ')
            scanning = 0;

        if(!scanning && isdigit(*c)) {
            switch(mode) {
                case MODE_TIME:
                    race_times[current_index++] = atoi(c);
                    break;
                case MODE_DISTANCE:
                    race_distances[current_index++] = atoi(c);
                    break;
                default:
                    break;
            }
            scanning = 1;
        }
        c++;

    }

    int race_ways = 0;

    for(int i = 0; i < current_index; i++) {
        for(int push_time = race_times[i] - 1; push_time > 0; push_time--) {
            if(race_distances[i] < ((race_times[i] - push_time) * push_time))
                race_ways++;
        }
        answer *= race_ways;
        race_ways = 0;
    }

    printf("The answer is %d\n", answer);

    free(input);
    return 0;
error:
    return 1;
}

