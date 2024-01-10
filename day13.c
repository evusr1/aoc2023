#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

int check_horizontal_line(char *string, int line_length, int x, int compare) {
    if(x < 0 || compare >= line_length)
        return 1;

    if(string[x] == string[compare]) {
        return check_horizontal_line(string, line_length, x - 1, compare + 1);
    } else
        return 0;
}

int check_horizontal(char *block, int line_length, int height, int x, int compare, int y) {
    if(y >= height)
        return 1;

    if(!check_horizontal_line(block + y * (line_length + 1), line_length, x, compare))
        return 0;

    return check_horizontal(block, line_length, height, x, compare, y + 1);
}


int check_vertical_line(char *block, int height, int line_length, int x, int y, int compare) {
    if(y < 0 || compare >= height)
        return 1;

    if(block[(line_length + 1) * y + x] == block[(line_length + 1) * compare + x])
        return check_vertical_line(block, height, line_length, x, y - 1, compare + 1);
    else
        return 0;
}

int check_vertical(char *block, int line_length, int height, int y, int compare, int x) {
    if(x >= line_length)
        return 1;

    if(!check_vertical_line(block, height, line_length, x, y, compare))
        return 0;

    return check_vertical(block, line_length, height, y, compare, x + 1);
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
        line_length = 0,
        height = 0,
        block_size = 0;

    char *block_start = input;

    while(block_start < input + size) {
        line_length = strchr(block_start, '\n') - block_start;

        while(block_start[(line_length + 1) * height]!='\n' && &block_start[(line_length + 1) * height] < input + size)
            height++;

        int x,
            y;

        for(x = 0; x < line_length; x++) {
            if(block_start[x]==block_start[x + 1]
                && check_horizontal(block_start, line_length, height, x, x + 1, 0))
                break;
        }

        for(y = 0; y < height; y++) {
            if(block_start[(line_length + 1) * y] == block_start[(line_length + 1) * (y + 1)]
                && check_vertical(block_start, line_length, height, y, y + 1, 0))
                break;
        }

        if(x!=line_length)
            answer+=x + 1;

        if(y!=height)
            answer += (y + 1) * 100;

        block_start+= (line_length + 1) * height;

        if(block_start[0]=='\n')
            block_start++;

        height = 0;
        line_length = 0;
    }

    printf("The answer is %d\n", answer);

    free(input);
    return 0;
error:
    return 1;
}
