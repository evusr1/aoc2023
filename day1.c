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

    int sum = 0;
    int i1 = 0;
    int i2 = 0;

    char *number_words[9] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};

    for(char *c = input; c < input + size; c++) {

        if(*c == '\n' || *c == 0) {

            sum += i1 * 10 + i2;
            i1 = 0;
            i2 = 0;

            continue;

        }

        for(int j = 0; j < 9; j++) {
            if(strncmp(number_words[j], c, strlen(number_words[j])) == 0) {
                *c = j + 1;

                goto set_integer;

            }
        }


        if(*c < '1' || *c > '9')
            continue;

        *c -= '0';

set_integer:

        if(!i1)
            i1 = *c;

        i2 = *c;

    }

    printf("Sum of calibration data: %i\n", sum);

    free(input);
    return 0;
error:
    return 1;
}

