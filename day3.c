#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define IS_NUM(c) (c >= '0' && c <= '9')


void clear_num(char *c) {
    if(!IS_NUM(*c))
        return;

    if(IS_NUM(*(c + 1)))
        clear_num(c + 1);

    *c = '.';
}

int get_number(char *c, char *input) {
    int res = 0;
    if(!IS_NUM(*c))
        return res;

    if(c == input || !IS_NUM(*(c - 1))) {
        res = atoi(c);
        clear_num(c);
    } else
        res = get_number(c - 1, input);

    return res;
}

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
    int pitch = 0;
    int part = 0;

    char *c = input;


    while(pitch < size) {
        if(c[pitch]=='\n')
            break;
        pitch++;
    }

    pitch++;

    while(c && c < input + size) {

        if(*c == '.' || *c == '\n')
            goto next;

        if(!IS_NUM(*c)) {

            char *top_left = c - 1 - pitch;

            for(int y = 0; y < 3; y++) {
                for(int x = 0; x < 3; x++)
                    sum += get_number(top_left + x + y * pitch, input);
            }
        }

next:
        c++;
    }

    printf("Sum of engine parts %d\n", sum);
    free(input);
    return 0;
error:
    return 1;
}
