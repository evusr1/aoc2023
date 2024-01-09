#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>

#define FOLD_SIZE 5

#define VISITED_TYPE_DISABLED -1
#define VISITED_TYPE_NO 0
#define VISITED_TYPE_YES 1

typedef struct allowed_position {
    int8_t visited;
    uint64_t num;
} allowed_position_t;



int validate(uint8_t *springs, uint8_t *numbers, uint8_t *validation, int springs_length, int number_length, int row_end) {
    int current_number = 0,
        x = 0;

    while(x < springs_length) {

        if(validation[x]=='#') {
            if(current_number >= row_end)
                return 0;

            for(int v = x; v < x + numbers[current_number%number_length]; v++) {
                if((springs[v] != '?' && springs[v] != '#')
                    || validation[v] != '#')
                    return 0;
            }

            x += numbers[current_number%number_length];
            if(x < springs_length && validation[x] == '#')
                return 0;

            current_number++;
        }
        x++;
    }

    return 1;
}

uint64_t validate_block(uint8_t *springs,
                  uint8_t* numbers,
                  uint8_t *validation,
                  uint8_t *pos_stack,
                  allowed_position_t *allowed_positions,
                  int springs_length,
                  int number_length,
                  int springs_length_ex,
                  int row_end) {

    uint64_t configurations = 0;

    int x = 0,
        stack_index = 0;

    memcpy(validation, springs, springs_length);

    while(stack_index > -1) {
        int delta = 0;

        if(springs[x] != '.') {
            if(x + numbers[stack_index%number_length] > springs_length) {
                stack_index--;

                if(stack_index < 0)
                    return configurations;

                x = pos_stack[stack_index];
                continue;

            } else {
                pos_stack[stack_index] = x;
                memset(validation + x, '#', numbers[stack_index%number_length]);

                if(allowed_positions[stack_index * springs_length_ex + x].visited) {

                    if(allowed_positions[stack_index * springs_length_ex + x].visited > 0) {
                        if(stack_index)
                            allowed_positions[(stack_index - 1) * springs_length_ex + pos_stack[(stack_index - 1)]].num += allowed_positions[stack_index * springs_length_ex + x].num;
                        else
                            configurations += allowed_positions[stack_index * springs_length_ex + x].num;
                    }

                    delta++;

                } else {
                    int is_last_row = stack_index == row_end - 1 ? 1 : 0;

                    if(validate(springs, numbers, validation, is_last_row ? springs_length : x + numbers[stack_index%number_length] + 1, number_length, stack_index + 1)) {
                        allowed_positions[stack_index * springs_length_ex + x].visited = VISITED_TYPE_YES;

                        if(!is_last_row) {
                            x += numbers[stack_index%number_length] + 1;

                            stack_index++;
                            continue;

                        } else
                            allowed_positions[stack_index * springs_length_ex + x].num += 1;

                    } else {
                        if(!stack_index)
                            allowed_positions[stack_index * springs_length_ex + x].visited = VISITED_TYPE_DISABLED;

                        delta++;
                    }
                }

                memcpy(validation + x, springs + x, springs_length - x);
            }

        } else
            delta++;

        x += delta;
    }

    return configurations;

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

    uint64_t answer = 0;

    int misses_total = 0,
        number_length = 0,
        springs_length = 0,
        scanning = 1;

    char *c = input;
    char *springs = input;
    uint8_t *numbers = NULL;

    while(c && c < input + size) {
        if(isdigit(*c)) {
            uint8_t num = atoi(c);
            numbers[number_length++] = num;
            c+= num / 10;
        }

        if(*c == ' ') {
            *c = 0;
            numbers = (uint8_t*)c;
            scanning = 0;
        } else if(scanning)
            springs_length++;

        if(*c == '\n') {
            uint32_t springs_length_ex = (springs_length + 1) * FOLD_SIZE;
            uint32_t number_length_ex = number_length  * FOLD_SIZE;

            uint32_t allowed_positions_sz = number_length_ex * springs_length_ex * sizeof(allowed_position_t);

            uint32_t memblock_sz = allowed_positions_sz + (springs_length_ex * 2
                                    + number_length_ex) * sizeof(uint8_t);

            allowed_position_t *allowed_positions = (allowed_position_t*)malloc(memblock_sz);
            if(!allowed_positions) {
                printf("Could not allocate memory\n");
                goto error;
            }

            memset(allowed_positions, 0, memblock_sz);

            uint8_t *validation = (uint8_t*)allowed_positions + allowed_positions_sz;
            uint8_t *springs_ex = validation + springs_length_ex;
            uint8_t *pos_stack  = springs_ex + springs_length_ex;

            for(int s = 0; s < springs_length_ex; s += springs_length + 1) {
                memcpy(springs_ex + s, springs, springs_length);
                springs_ex[s+ springs_length] = '?';
            }

            springs_length++;

            answer += validate_block(springs_ex,
                  numbers,
                  validation,
                  pos_stack,
                  allowed_positions,
                  springs_length_ex - 1,
                  number_length,
                  springs_length_ex,
                  number_length_ex);

            if(allowed_positions)
                free(allowed_positions);


            number_length = 0;
            springs_length = 0;
            scanning = 1;
            springs = c + 1;
        }

        c++;
    }

    printf("The answer is %ld\n", answer);

    free(input);
    return 0;
error:
    return 1;
}
