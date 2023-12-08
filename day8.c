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
} instruction_t;

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


    int answer = 0,
        instruction_amt = 0;

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
            uint8_t letter = c[i];
            unique_letters[i][letter]++;
            new_key.key[i] = letter;

        }
        c = strchr(c, '(') + 1;

        for(int i = 0; i < KEY_LENGTH; i++) {
            uint8_t letter = c[i];
            new_key.key_l[i] = c[i];
        }
        c = strchr(c, ' ') + 1;

        for(int i = 0; i < KEY_LENGTH; i++) {
            uint8_t letter = c[i];
            new_key.key_r[i] = c[i];
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

    for(int i = instruction_amt - 1; i >= 0 ; i--) {
        memcpy(
            &sort_instructions[
                --unique_letters[2][instructions[i].key[2]]
            ],
            &instructions[i],
            sizeof(instruction_t)
        );
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

    uint8_t last_key[KEY_LENGTH] = { 'Z', 'Z', 'Z' };
    uint8_t first_key[KEY_LENGTH] = { 'A', 'A', 'A' };

    int current_index = search(sort_instructions, instruction_amt, (char*)&first_key);
    int last_index = search(sort_instructions, instruction_amt, (char*)&last_key);

    char *directions = input;

    while(current_index != last_index) {
        answer++;

        if(*directions=='\n')
            directions = input;
        switch(*directions) {
            case DIRECTION_L:
                current_index = search(sort_instructions, instruction_amt, sort_instructions[current_index].key_l);
                break;
            case DIRECTION_R:
                current_index = search(sort_instructions, instruction_amt, sort_instructions[current_index].key_r);
                break;
            default:
                break;
        }

        directions++;
    }
    printf("The answer is %d\n", answer);

    return 0;
error:
    if(input)
        free(input);
    if(sort_instructions)
        free(sort_instructions);
    return 1;
}

