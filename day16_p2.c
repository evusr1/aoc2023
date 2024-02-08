#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DIR_UP 0x1
#define DIR_RIGHT 0x2
#define DIR_DOWN 0x4
#define DIR_LEFT 0x8

#define MIRROR_BSLASH '\\'
#define MIRROR_FSLASH '/'

#define SPLIT_UP_DOWN '|'
#define SPLIT_L_R '-'

int beam(char *input, int stride, int height, int x, int y, int dir) {
    char *mask = input + stride * height;

    int delta_x = 0,
        delta_y = 0,
        count = 0;

    int pos = y * stride + x;

    if(x >= stride - 1)
        return count;
    if(x < 0)
        return count;
    if(y >= height)
        return count;
    if(y < 0)
        return count;

    switch(dir) {
        case DIR_UP:
            delta_y = -1;
            break;
        case DIR_DOWN:
            delta_y = 1;
            break;
        case DIR_RIGHT:
            delta_x = 1;
            break;
        case DIR_LEFT:
            delta_x = -1;
            break;
    }

    if(dir & mask[pos])
        return count;
     else {
        if(!mask[pos])
            count++;
        mask[pos] |= dir;
    }

    int new_dir = dir;

    switch(input[pos]) {
        case SPLIT_UP_DOWN:
            if(delta_x) {
                count += beam(input, stride, height, x, y + 1, DIR_DOWN);
                count += beam(input, stride, height, x, y - 1, DIR_UP);
                return count;
            }
            break;

        case SPLIT_L_R:
            if(delta_y) {
                count += beam(input, stride, height, x - 1, y, DIR_LEFT);
                count += beam(input, stride, height, x + 1, y, DIR_RIGHT);
                return count;
            }
            break;

        case MIRROR_BSLASH:
            if(delta_x) {
                if(delta_x < 0)
                    new_dir = DIR_UP;

                if(delta_x > 0)
                    new_dir = DIR_DOWN;

                delta_y = delta_x;
                delta_x = 0;
            } else if(delta_y) {
                if(delta_y < 0)
                    new_dir = DIR_LEFT;

                if(delta_y > 0)
                    new_dir = DIR_RIGHT;

                delta_x = delta_y;
                delta_y = 0;
            }
            break;

        case MIRROR_FSLASH:
            if(delta_x) {
                if(delta_x < 0) {
                    new_dir = DIR_DOWN;

                }
                if(delta_x > 0) {
                    new_dir = DIR_UP;
                }

                delta_y = -delta_x;
                delta_x = 0;

            } else if(delta_y) {
                if(delta_y < 0) {
                    new_dir = DIR_RIGHT;
                }
                if(delta_y > 0) {
                    new_dir = DIR_LEFT;
                }

                delta_x = -delta_y;
                delta_y = 0;
            }
            break;

    }

    return count + beam(input, stride, height, x + delta_x, y + delta_y, new_dir);
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

    char *input = calloc(size * 2, sizeof(char));
    if(!input) {
        printf("Could not allocate memory\n");
        goto error;
    }

    fread(input, size, 1, file);

    int answer = 0;

    char *c = input;
    int stride = strchr(input, '\n') - input + 1;
    int height = size/stride;

    for(int y = 0; y < height; y++) {
        int energized;

        energized = beam(input, stride, height, 0, y, DIR_RIGHT);

        if(answer < energized)
            answer = energized;

        memset(input + size, 0, size);

        energized = beam(input, stride, height, stride - 2, y, DIR_LEFT);

        if(answer < energized)
            answer = energized;

        memset(input + size, 0, size);
    }

    for(int x = 0; x < stride - 1; x++) {
        int energized;

        energized = beam(input, stride, height, x, 0, DIR_DOWN);

        if(answer < energized)
            answer = energized;

        memset(input + size, 0, size);

        energized = beam(input, stride, height, x, height - 1, DIR_UP);

        if(answer < energized)
            answer = energized;

        memset(input + size, 0, size);
    }

    printf("The answer is %d\n", answer);

    free(input);
    return 0;
error:
    return 1;
}

