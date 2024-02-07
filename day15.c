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

    int answer = 0;

    char *c = input;
    int current_value = 0;
    while(c && c < input + size) {

        if(*c == ',' || *c == '\n') {
            answer += current_value;
            current_value = 0;
        } else {
            current_value += *c;
            current_value *= 17;
            current_value %= 256;
        }

        c++;
    }

    printf("The answer is %d\n", answer);

    free(input);
    return 0;
error:
    return 1;
}

