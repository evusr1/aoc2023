#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>

int validate(uint8_t *springs, uint8_t *numbers, uint8_t *validation, int springs_length, int number_length) {
    int current_number = 0,
        pounds = 0,
        x = 0;
    while(x < springs_length) {

        if(validation[x]=='#') {
            if(current_number > number_length)
                return 0;


            for(int v = x; v < x + numbers[current_number]; v++) {
                if((springs[v] != '?' && springs[v] != '#')
                    || validation[v] != '#')
                    return 0;
            }

            x += numbers[current_number];

            if(x < springs_length && validation[x] == '#')
                return 0;

            current_number++;
        }
        x++;
    }

    return 1;
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

    int answer = 0,
        number_length = 0,
        springs_length = 0,
        scanning = 1;

    char *c = input;
    char *springs = input;
    uint8_t *numbers = NULL;
    while(c && c < input + size) {
        if(isdigit(*c)) {
            int num = atoi(c);
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
            uint8_t *validation = (uint8_t*)malloc(springs_length * sizeof(uint8_t));
            if(!validation) {
                printf("Could not allocate memory\n");
                goto error;
            }


            uint8_t pos_stack[256] = { 0 };
            int stack_index = 0;

            for(int s = 0; s < springs_length; s++) {
                int x = 0,
                    stack_index = 0;

                pos_stack[stack_index] = s;
                x = pos_stack[stack_index] + numbers[stack_index++];

                while(stack_index > 0) {

                    if(x + numbers[stack_index] > springs_length)
                        x = pos_stack[--stack_index];
                    else  {
                        pos_stack[stack_index] = x;

                        if(stack_index == number_length - 1) {
                            memcpy(validation, springs, springs_length);

                            for(int i = 0; i <= stack_index; i++)
                                memset(validation + pos_stack[i], '#', numbers[i]);

                            if(validate(springs, numbers, validation, springs_length, number_length))
                                answer++;

                        } else
                            x += numbers[stack_index++];
                    }

                    x++;
                }
            }

            if(validation)
                free(validation);

            number_length = 0;
            springs_length = 0;
            scanning = 1;
            springs = c + 1;
        }

        c++;
    }

    printf("The answer is %d\n", answer);

    free(input);
    return 0;
error:
    return 1;
}

