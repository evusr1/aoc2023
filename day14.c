#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define SHIFT_UP 0
#define SHIFT_DOWN 1
#define SHIFT_LEFT 2
#define SHIFT_RIGHT 3

#define TYPE_ROCK 'O'
#define TYPE_CUBE '#'
#define TYPE_NONE '.'

int perform_shift(char *input, int size, int stride, int x, int y, int count) {
    char *current_char = &input[y * stride + x];

    if(x >= stride - 1
        || y * stride >= size
        ||x < 0
        || y < 0
        || *current_char == TYPE_CUBE)
            return 0;

    if(count > 0) {
        *current_char = TYPE_ROCK;
        return size/stride - y + perform_shift(input, size, stride, x, y + 1, count - 1);
    }

    *current_char = '.';

    return perform_shift(input, size, stride, x, y + 1, 0);

}

int shift(char *input, int size, int stride, int x, int y, int count) {
    char *current_char = &input[y * stride + x];

    if(x >= stride - 1
        || y * stride >= size
        ||x < 0
        || y < 0) {
            return perform_shift(input, size, stride, x , y + 1, count);
    }

    switch(*current_char) {
        case TYPE_ROCK:
            return shift(input, size, stride, x, y - 1, count + 1);
        case TYPE_CUBE:
            return perform_shift(input, size, stride, x, y + 1, count)
                    + shift(input, size, stride, x, y - 1, 0) ;
        default:
            return shift(input, size, stride, x, y - 1, count);
    }
}

int shift_line(char *input, int size, int stride, int x, int y) {
    if(x >= stride - 1
        || y * stride >= size
        ||x < 0
        || y < 0)
            return 0;

    return shift(input, size, stride, x, y, 0)
            + shift_line(input, size, stride, x + 1, y);
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

    int answer = 0;

    int stride = strchr(input, '\n') - input + 1;

    answer = shift_line(input, size, stride, 0, size/stride - 1);

    printf("The answer is %d\n", answer);

    free(input);
    return 0;
error:
    return 1;
}

