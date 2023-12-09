#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define MAX_NUM 256

void calculate_diff(int64_t *arr, int length, int pos) {
    int index = pos + 1;

    if(index == length)
        return;

    arr[pos] =  arr[index] - arr[pos];

    calculate_diff(arr, length, index);
}

int64_t oasis(int64_t *arr, int length) {
    int index = length - 1;

    if(!index)
        return arr[index];

    calculate_diff(arr, length, 0);

    return oasis(arr, index) + arr[index];

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

    int64_t answer = 0;
    int scanning = 0,
        current_length = 0,
        negative = 1;

    char *c = input;
    int64_t numbers[MAX_NUM];

    while(c && c < input + size) {
        if(*c == '-')
            negative = -1;

        if(!scanning && isdigit(*c)) {
            numbers[current_length++]  = atoi(c) * negative;
            scanning = 1;
            negative = 1;
        }

        if(*c == ' ')
            scanning = 0;

        if(*c == '\n') {
            int64_t ans = oasis((int64_t*)&numbers, current_length);
            answer += ans;
            current_length = 0;
            scanning = 0;
        }

        c++;
    }

    printf("The answer is %ld\n", answer);

    free(input);
    return 0;
error:
    return 1;
}
