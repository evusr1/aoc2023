#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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


    int sum = 0,
        game = 1,
        possible = 1,
        current_number = 0;

    char *c = strchr(input, ':');

    while(c && c < input + size) {
        if(*c == '\n') {
            if(possible == 1 && game)
                sum += game;

            game++;

            c = strchr(c, ':');
            possible = 1;

            continue;
        }

        if(*c == ';'
            || *c == ','
            || *c == ' '
            || *c == ':') {

            c++;
            continue;
        }
        current_number = atoi(c);

        if(current_number) {
            c = strchr(c, ' ') + 1;

            if(strncmp(c, "blue", sizeof("blue") -1)==0) {
                if(current_number > 14)
                    possible = 0;

                c += sizeof("blue") - 1;

            } else if(strncmp(c, "green", sizeof("green") -1)==0) {
                if(current_number > 13)
                    possible = 0;

                c += sizeof("green") - 1;

            } else if(strncmp(c, "red", sizeof("red") -1)==0) {
                if(current_number > 12)
                    possible = 0;

                c += sizeof("red") - 1;

            }
            if(!possible)
                c = strchr(c, '\n');
        }
    }

    printf("Sum of games id: %i\n", sum);

    free(input);
    return 0;
error:
    return 1;
}

