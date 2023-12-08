#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>

#define DIRECTION_L 'L'
#define DIRECTION_R 'R'

#define KEY_LENGTH 3
#define ASCII_MAX 256

typedef struct instruction {
    uint8_t key[KEY_LENGTH];
    uint8_t key_l[KEY_LENGTH];
    uint8_t key_r[KEY_LENGTH];
} __attribute__((packed)) instruction_t;

typedef struct instruction_ghost {
    instruction_t keys;
    int current_index;
    uint64_t steps;
    int current_direction;
}__attribute__((packed)) instruction_ghost_t;

int compare(char *labels1, char *labels2, int len) {
    for(int i = 0; i < len; i++) {

        int compare = labels1[i] - labels2[i];
        if(compare)
            return compare;
    }
    return 0;
}

int search(instruction_t *instructions, int max, char *key) {
    int left = 0;
    int right = max - 1;

    int index = 0;

    while(left <= right) {
        index = (left + right)/2;
        int compare_res = compare(instructions[index].key, key, KEY_LENGTH);

         if(!compare_res)
            return index;

        if(compare_res > 0)
            right = index - 1;
        else if (compare_res < 0)
            left = index + 1;

    }
    return -1;
}

uint64_t gcd(uint64_t a, uint64_t b) {

    if(b==0)
        return a;

    return gcd(b, a % b);
}

int main(int argc, char *argv[]) {
    instruction_t *sort_instructions = NULL;
    char *input = NULL;

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


    input = malloc(size * sizeof(char));
    if(!input) {
        printf("Could not allocate memory\n");
        goto error;
    }

    fread(input, size, 1, file);


    int instruction_amt = 0;

    char *c = input;

    c = strchr(c, '\n') + 1;
    instruction_t *instructions = (instruction_t *)c;

    int unique_letters[KEY_LENGTH][ASCII_MAX] = { 0 };

    while(c && c < input + size) {
        if(*c=='\n') {
            c++;
            continue;
        }
        instruction_t new_key;
        for(int i = 0; i < KEY_LENGTH; i++) {
            uint8_t letter = c[i] ;
            unique_letters[i][letter]++;
            new_key.key[i] = letter;
        }
        c = strchr(c, '(') + 1;

        for(int i = 0; i < KEY_LENGTH; i++) {
            uint8_t letter = c[i] ;
            new_key.key_l[i] = c[i] ;
        }
        c = strchr(c, ' ') + 1;

        for(int i = 0; i < KEY_LENGTH; i++) {
            uint8_t letter = c[i] ;
            new_key.key_r[i] = c[i] ;
        }

        c = strchr(c, '\n') + 1;

        memcpy(&instructions[instruction_amt++], &new_key, sizeof(new_key));

    }

    for(int i = 0; i < KEY_LENGTH; i++) {
        for(int j = 1; j < ASCII_MAX; j++)
            unique_letters[i][j] += unique_letters[i][j - 1];
    }

    sort_instructions = (instruction_t*)malloc(sizeof(instruction_t) * instruction_amt);
    if(!sort_instructions) {
        printf("Could not allocate memory\n");
        goto error;
    }

    int lowest_a_index = instruction_amt,
        a_amt = unique_letters[2]['A'];


    for(int i = instruction_amt - 1; i >= 0 ; i--) {
        unique_letters[2][instructions[i].key[2]] -= 1;
        int new_index = unique_letters[2][instructions[i].key[2]];

        if(instructions[i].key[2] == 'A' && new_index - 1 < lowest_a_index)
            lowest_a_index = new_index;

        memcpy(
            &sort_instructions[new_index],
            &instructions[i],
            sizeof(instruction_t)
        );
    }


    instruction_ghost_t *ghost_starts = (instruction_ghost_t *)&instructions[instruction_amt];


    for(int i = 0; i < a_amt; i++) {
        memset(&ghost_starts[i], 0, sizeof(instruction_ghost_t));
        memcpy(&ghost_starts[i].keys,  &sort_instructions[lowest_a_index + i], sizeof(instruction_t));
    }


    for(int i = instruction_amt - 1; i >= 0 ; i--) {
        memcpy(
            &instructions[
                --unique_letters[1][sort_instructions[i].key[1]]
            ],
            &sort_instructions[i],
            sizeof(instruction_t)
        );
    }

    for(int i = instruction_amt - 1; i >= 0 ; i--) {
        memcpy(
            &sort_instructions[
                --unique_letters[0][instructions[i].key[0]]
            ],
            &instructions[i],
            sizeof(instruction_t)
        );
    }

    for(int i = 0; i < a_amt; i++)
        ghost_starts[i].current_index = search(sort_instructions, instruction_amt, ghost_starts[i].keys.key);

    char *directions = input;
    int all_indexes_z = 0;
    uint64_t total_steps = 0;

    while(!all_indexes_z) {
        all_indexes_z = 1;

        if(*directions=='\n')
            directions = input;

        for(int i = 0; i < a_amt; i++) {
            if(!ghost_starts[i].steps) {
                if(sort_instructions[ghost_starts[i].current_index].key[2] == 'Z' ) {
                    ghost_starts[i].current_direction = directions - input;
                    ghost_starts[i].steps = total_steps;

                } else
                    all_indexes_z = 0;
            }

        }

        total_steps++;

        switch(*directions) {
            case DIRECTION_L:
                for(int i = 0; i < a_amt; i++) {
                     if(!ghost_starts[i].steps)
                        ghost_starts[i].current_index = search(sort_instructions, instruction_amt, sort_instructions[ghost_starts[i].current_index].key_l);
                }
                break;
            case DIRECTION_R:
                for(int i = 0; i < a_amt; i++) {
                     if(!ghost_starts[i].steps)
                        ghost_starts[i].current_index = search(sort_instructions, instruction_amt, sort_instructions[ghost_starts[i].current_index].key_r);
                }
                break;
            default:
                break;
        }

        directions++;
    }

    uint64_t answer = ghost_starts[0].steps;
    for(int i = 1; i < a_amt; i++)
        answer = (answer * ghost_starts[i].steps) / gcd(answer, ghost_starts[i].steps);

    printf("The answer is %lu\n", answer);

    return 0;
error:
    if(input)
        free(input);
    if(sort_instructions)
        free(sort_instructions);
    return 1;
}
