#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>

#define CYCLES  1000000000
#define SAMPLE_SIZE 256

#define UP 0
#define DOWN 1
#define RIGHT 2
#define LEFT 3

#define TYPE_ROCK 'O'
#define TYPE_CUBE '#'
#define TYPE_NONE '.'


typedef struct cube {
    int y;

    int cycle_history[SAMPLE_SIZE];
    int counts[3];
    int move_index[4];

    int next_index;
} cube_t;

int cycle_detect(cube_t *cube, int start_index) {

    for(int cycle_length = 1; cycle_length < SAMPLE_SIZE; cycle_length++) {

        if(cycle_length * 2 > SAMPLE_SIZE - start_index)
            return 0;

        int found = 1;
        for(int validation = start_index + cycle_length; validation < SAMPLE_SIZE; validation++) {
            if(cube->cycle_history[validation - cycle_length] != cube->cycle_history[validation]) {
                found = 0;
                break;
            }
        }

        if(found)
            return cycle_length;

    }

    return 0;

}

int search_cube(char *input, int stride, int height, int x, int y, int delta_x, int delta_y) {
    int width =  stride - sizeof(char) - 1;

    if(x < 0)
        return stride * y;

    if(x > width)
        return stride * y + width;

    if(y > height - 1)
        return stride * (height - 1) + x;

    if(y < 0)
        return x;

    if(input[stride * y + x] == TYPE_CUBE)
        return stride * y + x;

    return search_cube(input, stride, height, x + delta_x, y + delta_y, delta_x, delta_y);
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

    cube_t *cubes = (cube_t *)malloc(sizeof(cube_t) *  size);

    for(int x = 0; x < stride; x++) {

        int current_cube_index = x;

        for(int y = 0; y < size/stride; y++) {
            int pos = y * stride + x;

            cubes[pos].y = y;

            if(y == 0 || y == size/stride - 1)
                cubes[pos].next_index = pos + 1;
            else if(x == stride  - sizeof(char) - 1)
                cubes[pos].next_index = stride * (y + 1);
            else
                cubes[pos].next_index = search_cube(input, stride, size/stride, x + 1, y, 1, 0);

            char type = input[pos];

            switch(type) {
                case TYPE_CUBE:
                    current_cube_index = pos;
                    break;
                case TYPE_ROCK:
                    cubes[current_cube_index].counts[DOWN]++;
                default:
                    cubes[pos].move_index[UP] = search_cube(input, stride, size/stride, x, y - 1, 0, -1);
                    cubes[pos].move_index[DOWN] = search_cube(input, stride, size/stride, x, y + 1, 0, 1);
                    cubes[pos].move_index[LEFT] = search_cube(input, stride, size/stride, x - 1 , y, -1, 0);
                    cubes[pos].move_index[RIGHT] = search_cube(input, stride, size/stride, x + 1, y, 1, 0);
                    break;

            }

        }
    }


    for(int j = 0; j < SAMPLE_SIZE; j++) {

        for(int pos = 0; pos < size; pos = cubes[pos].next_index) {

            while(cubes[pos].counts[DOWN] > 0) {
                cubes[pos].counts[DOWN]--;

                int left_shift = (input[pos]==TYPE_CUBE);
                int left_pos = cubes[pos + stride * (cubes[pos].counts[DOWN] + left_shift)].move_index[LEFT];

                int down_shift = (input[left_pos]==TYPE_CUBE);
                int down_pos = cubes[left_pos + cubes[left_pos].counts[RIGHT] + down_shift].move_index[DOWN];

                int right_shift = (input[down_pos]==TYPE_CUBE);
                int right_pos = cubes[down_pos - stride * (cubes[down_pos].counts[UP] + right_shift)].move_index[RIGHT];

                cubes[left_pos].counts[RIGHT]++;
                cubes[down_pos].counts[UP]++;

                cubes[right_pos].cycle_history[j]++;
            }

        }

        for(int pos = 0; pos < size; pos = cubes[pos].next_index) {

            for(int i = 0 ; i < cubes[pos].cycle_history[j]; i++) {
                int shift = (input[pos]==TYPE_CUBE);
                int next_pos = cubes[pos - i - shift].move_index[UP];

                cubes[next_pos].counts[DOWN]++;
            }

            cubes[pos].counts[RIGHT] = 0;
            cubes[pos].counts[UP] = 0;

        }

    }


    for(int pos = 0; pos < size; pos = cubes[pos].next_index) {

        int cycle_length = 0,
            start_index = 0;

        for(start_index = 0; start_index < SAMPLE_SIZE; start_index++) {
            cycle_length = cycle_detect(&cubes[pos], start_index);
            if(cycle_length > 0)
                break;
        }

        int count = cubes[pos].cycle_history[start_index  + (CYCLES - start_index - 1) % cycle_length];
        answer += (size/stride - cubes[pos].y) * count;

    }

    printf("The answer is %d\n", answer);
    free(input);
    free(cubes);

    return 0;
error:
    return 1;
}

