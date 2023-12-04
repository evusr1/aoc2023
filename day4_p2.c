#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
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

    char *win_numbers_ptr= strchr(input, ':');
    char *my_numbers_ptr = strchr(win_numbers_ptr, '|') ;

    int length = strchr(my_numbers_ptr, '\n') - input + 1;

    int winning_nums = 0,
        scanning_toggle = 0,
        matches = 0;

    char *c = win_numbers_ptr;

    uint16_t *winning_nums_converted_ptr = (uint16_t *)win_numbers_ptr;
    uint32_t *card_stats = (uint32_t *)input;

    while(card_stats && (char*)card_stats< input + size) {
        *card_stats = 1;
        card_stats = (uint32_t*)((char*)card_stats + length);
    }

    card_stats = (uint32_t *)input;

    while(c && c < input + size) {
        if(*c==' ')
            scanning_toggle = 0;

        if(*c=='\n') {
            uint32_t *current_card_ptr = card_stats;

            for(; matches > 0; matches--) {
                current_card_ptr = (uint32_t*)((char*)current_card_ptr + length);
                *current_card_ptr += *card_stats;
            }

            winning_nums = 0;
            my_numbers_ptr += length;
            win_numbers_ptr += length;
            card_stats = (uint32_t*)((char*)card_stats + length);

            winning_nums_converted_ptr = (uint16_t *)win_numbers_ptr;

            c = win_numbers_ptr;
        }

        if(*c == '|')
            c = my_numbers_ptr;

        if(!scanning_toggle && isdigit(*c)) {

            if(c <= my_numbers_ptr) {
                winning_nums_converted_ptr[winning_nums++] = atoi(c);
                c = strchr(c, ' ');
            } else {
                for(int i = 0; i < winning_nums; i++) {
                    if(winning_nums_converted_ptr[i] == atoi(c))
                        matches++;
                }
                scanning_toggle=1;
            }
        }

        c++;
    }

    card_stats = (uint32_t *)input;

    while(card_stats && (char*)card_stats< input + size) {
        answer += *card_stats;
        card_stats = (uint32_t*)((char*)card_stats + length);
    }

    printf("The answer is %d\n", answer);

    free(input);
    return 0;
error:
    return 1;
}
