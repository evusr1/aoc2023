#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>

#define N 0
#define S 1
#define W 2
#define E 3

#define N_TO_S '|'
#define E_TO_W '-'
#define N_TO_E 'L'
#define N_TO_W 'J'
#define S_TO_W '7'
#define S_TO_E 'F'
#define START 'S'

typedef struct corner {
    uint16_t x;
    uint16_t y;
} corner_t;


int count_inside(uint8_t *field, corner_t *corners, int width, int height, int current_corner) {
    int count = 0;
    for(int y = 0; y < height - 1; y++) {
        for(int x = 0; x < width; x++) {
            if(field[y*width + x] < 128 && field[y*width + x] != '\n') {
                int inside = 0;

                for (int i = 0; i < current_corner - 1; i++) {
                    if(((y >= corners[i+1].y && y < corners[i].y)
                        ||(y >= corners[i].y && y < corners[i+1].y))
                        && ((x <= corners[i+1].x)&&(x < corners[i].x))) {
                        inside=!inside;
                    }
                }
                if(inside)
                    count++;
            }
        }
    }
    return count;
}

int step(uint8_t *field, corner_t *corners, int x, int y, int width, int height, int *current_corner) {

    char n = y - 1 >= 0 ? field[(y - 1)*width + x] : '.';
    char s = y + 1 < height ? field[(y + 1)*width + x] : '.';
    char w = x - 1 >= 0 ? field[y*width + x - 1] : '.';
    char e = x + 1 < width ? field[y*width + x + 1] : '.';

    char current  = field[y*width + x];

    field[y*width + x] += 128;

    int steps[4] = { 1, 1, 1, 1 };

    switch(current) {
        case N_TO_S://'|'
            switch(s) {
                case N_TO_E://'L'
                case N_TO_W://'J'
                    corners[*current_corner].x = x;
                    corners[(*current_corner)++].y = y + 1;
                case N_TO_S://'|'
                    steps[S] += step(field, corners, x, y + 1, width, height, current_corner);
                    break;
            }
            switch(n) {
                case S_TO_W://'7'
                case S_TO_E://'F'
                    corners[*current_corner].x = x;
                    corners[(*current_corner)++].y = y - 1;
                case N_TO_S://'|'
                    steps[N] += step(field, corners,  x, y - 1, width, height, current_corner);
                    break;
            }
            break;

        case E_TO_W://'-'
            switch(w) {
                case N_TO_E://'L'
                case S_TO_E://'F'
                    corners[*current_corner].x = x -1;
                    corners[(*current_corner)++].y = y;
                case E_TO_W://'-'
                    steps[W] += step(field, corners, x - 1, y, width, height, current_corner);
                    break;
            }

            switch(e) {
                case N_TO_W://'J'
                case S_TO_W://'7'
                    corners[*current_corner].x = x + 1;
                    corners[(*current_corner)++].y = y;
                case E_TO_W://'-'
                    steps[E] += step(field, corners,  x + 1, y, width, height, current_corner);
                    break;
            }
            break;

        case N_TO_E://'L'
            switch(e) {
                case N_TO_W://'J'
                case S_TO_W://'7'
                    corners[*current_corner].x = x + 1;
                    corners[(*current_corner)++].y = y;
                case E_TO_W://'-'
                    steps[E] += step(field, corners,  x + 1, y, width, height, current_corner);
                    break;
            }
            switch(n) {
                case S_TO_W://'7'
                case S_TO_E://'F'
                    corners[*current_corner].x = x;
                    corners[(*current_corner)++].y = y - 1;
                case N_TO_S://'|'
                    steps[N] += step(field, corners,  x, y - 1, width, height, current_corner);
                    break;
            }
            break;

        case N_TO_W://'J'
            switch(w) {
                case N_TO_E://'L'
                case S_TO_E://'F'
                    corners[*current_corner].x = x - 1;
                    corners[(*current_corner)++].y = y;
                case E_TO_W://'-'
                    steps[W] += step(field, corners, x - 1, y, width, height, current_corner);
                    break;
            }
            switch(n) {
                case S_TO_W://'7'
                case S_TO_E://'F'
                    corners[*current_corner].x = x;
                    corners[(*current_corner)++].y = y - 1;
                case N_TO_S://'|'
                    steps[N] += step(field, corners, x, y - 1, width, height, current_corner);
                    break;
            }
            break;
        case S_TO_W://'7'
            switch(w) {
                case N_TO_E://'L'
                case S_TO_E://'F'
                    corners[*current_corner].x = x - 1;
                    corners[(*current_corner)++].y = y;
                case E_TO_W://'-'
                    steps[W] += step(field, corners,  x - 1, y, width, height, current_corner);
                    break;
            }
            switch(s) {
                case N_TO_W://'J'
                case N_TO_E://'L'
                    corners[*current_corner].x = x;
                    corners[(*current_corner)++].y = y + 1;
                case N_TO_S://'|'
                    steps[S] += step(field, corners,  x, y + 1, width, height, current_corner);
                    break;
            }
            break;
        case S_TO_E://'F'
            switch(e) {
                case N_TO_W://'J'
                case S_TO_W://'7'
                corners[*current_corner].x = x + 1;
                corners[(*current_corner)++].y = y;
                case E_TO_W://'-'
                    steps[E] += step(field, corners,  x + 1, y, width, height, current_corner);
                    break;
            }
            switch(s) {
                case N_TO_E://'L'
                case N_TO_W://'J'
                    corners[*current_corner].x = x;
                    corners[(*current_corner)++].y = y + 1;
                case N_TO_S://'|'
                    steps[S] += step(field, corners, x, y + 1, width, height, current_corner);
                    break;
            }
            break;
        case START:
            switch(n) {
                case S_TO_W://'7'
                case S_TO_E://'F'
                    corners[*current_corner].x = x;
                    corners[(*current_corner)++].y = y - 1;
                case N_TO_S://'|'
                    steps[N] += step(field, corners,  x, y -1, width, height, current_corner);
                    break;
            }
            switch(s) {
                case N_TO_E://'L'
                case N_TO_W://'J'
                    corners[*current_corner].x = x;
                    corners[(*current_corner)++].y = y + 1;
                case N_TO_S://'|'
                    steps[S] += step(field, corners, x, y + 1, width, height, current_corner);
                    break;
            }
            switch(w) {
                case N_TO_E://'L'
                case S_TO_E://'F'
                    corners[*current_corner].x = x - 1;
                    corners[(*current_corner)++].y = y;
                case E_TO_W://'-'
                    steps[W] += step(field, corners, x - 1, y, width, height, current_corner);
                    break;
            }
            switch(e) {
                case S_TO_W://'7'
                case N_TO_W://'J'
                    corners[*current_corner].x = x + 1;
                    corners[(*current_corner)++].y = y;
                case E_TO_W://'-'
                    steps[E] += step(field, corners, x + 1, y, width, height, current_corner);
                    break;
            }

    }
    int highest = steps[0];

    for(int i = 1; i < 4; i++) {
        if(steps[i] > highest)
            highest = steps[i];
    }

    return highest;
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


    uint8_t *input = malloc(size*sizeof(char) + size*sizeof(corner_t));
    if(!input) {
        printf("Could not allocate memory\n");
        goto error;
    }

    fread(input, size, 1, file);

    int width = 0,
        height = 0,
        x = 0,
        y = 0;

    uint8_t *c = input;
    uint8_t *start = NULL;

    while(c < input + size) {
        if(*c == START)
            start = c;
        c++;
        if(!height) {
            width++;
            if(*c == '\n') {
                width++;
                height = size / width;
            }
        }
        if(start && height)
            break;
    }

    y = (start - input)/width;
    x = (start - input)%width;

    corner_t *corners = (corner_t *)(input + size);

    uint32_t current_corner = 0;
    corners[current_corner].x = x;
    corners[current_corner++].y = y;

    uint32_t steps = step(input, corners, x, y, width, height, &current_corner) / 2;

    corners[current_corner].x = x;
    corners[current_corner++].y = y;

    uint32_t intersections = count_inside(input, corners, width, height, current_corner) - 1;

   printf("The answer is %d steps with %d contained\n", steps, intersections);

    free(input);
    return 0;
error:
    return 1;
}

