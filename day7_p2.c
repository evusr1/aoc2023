#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>

#define TYPES_AMT 7
#define HAND_SIZE 5

const char card_types[] = "J23456789TQKA";

#define TYPE_LONGEST 0
#define TYPE_TYPES 1

const uint8_t types_key[TYPES_AMT][2] = {
    {1, 5},
    {2, 4},
    {2, 3},
    {3, 3},
    {3, 2},
    {4, 2},
    {5, 1}
};

typedef struct card_sort {
    uint8_t labels[5];
    uint32_t bid;
    int less;
    int more;
} card_sort_t;

typedef struct label_sort {
    int value;
    int tail;
}  __attribute__((packed)) label_sort_t;

void card_sort_get_value(card_sort_t *sort_cards, int current_index, int *rank, int *bid) {
    if(current_index == -1)
        return;

    if(sort_cards[current_index].less != -1)
        card_sort_get_value(sort_cards, sort_cards[current_index].less, rank, bid);

    *rank+= 1;
    *bid += *rank * sort_cards[current_index].bid;

    if(sort_cards[current_index].more != -1)
        card_sort_get_value(sort_cards, sort_cards[current_index].more, rank, bid);
}

int card_compare(char *labels1, char *labels2) {
    for(int i = 0; i < HAND_SIZE; i++) {
        int compare = labels1[i] - labels2[i];
        if(compare)
            return compare;
    }
    return 0;
}

void card_sort_add(card_sort_t *sort_cards, int new_index, int higher_index) {

    if(new_index == higher_index)
        return;

    if(card_compare(sort_cards[new_index].labels, sort_cards[higher_index].labels) < 0) {
        if(sort_cards[higher_index].less == -1)
            sort_cards[higher_index].less = new_index;
        else
            card_sort_add(sort_cards, new_index, sort_cards[higher_index].less);
    } else if(sort_cards[higher_index].more == -1)
            sort_cards[higher_index].more = new_index;
        else
            card_sort_add(sort_cards, new_index, sort_cards[higher_index].more);
}

int get_value(char card) {
    return strchr(card_types, card) - (char*)&card_types;
}

void convert_copy_cards(card_sort_t *sort_cards, char *labels_ptr) {
    for(int i = 0; i < HAND_SIZE; i++)
        sort_cards->labels[i] = get_value(labels_ptr[i]);
}

int label_sort_add(label_sort_t *sort_labels, char *cards, uint8_t value, int higher_index, int current_index) {
    if(cards[higher_index] <= value) {
        sort_labels[current_index].tail = higher_index;

        return current_index;
    }

    if(sort_labels[higher_index].tail!=-1)
        if(cards[sort_labels[higher_index].tail] <= value) {
            sort_labels[current_index].tail = sort_labels[higher_index].tail;
            sort_labels[higher_index].tail = current_index;
        } else
            label_sort_add(sort_labels, cards, value, sort_labels[higher_index].tail, current_index);
    else {
        sort_labels[higher_index].tail = current_index;
        sort_labels[current_index].tail = -1;
    }

    return higher_index;
}

int get_type(uint8_t *cards) {
    int types_amt = 1,
        highest_index = 0;

    label_sort_t sort_labels[HAND_SIZE];
    sort_labels[0].value = cards[0];
    sort_labels[0].tail = -1;

    for(int i = 1; i < HAND_SIZE; i++) {
         sort_labels[i].value = cards[i];
         highest_index = label_sort_add(sort_labels, cards, sort_labels[i].value, highest_index, i);
    }

    int current_index = highest_index,
        current_pair = -1,
        jokers = 0,
        types = 0,
        count = 0,
        longest_count = 0;

    while(current_index != -1) {
        if(!sort_labels[current_index].value)
            jokers++;
         else {

            if(sort_labels[current_index].value != current_pair) {
                types++;
                count = 0;
            }

            count++;

            if(count > longest_count)
                longest_count = count;

        }

        current_pair = sort_labels[current_index].value;
        current_index = sort_labels[current_index].tail;
    }

    if(!types)
        types++;

    for(int type = 0; type < TYPES_AMT; type++) {
        if(types_key[type][TYPE_LONGEST] == longest_count + jokers && types_key[type][TYPE_TYPES] == types)
            return type;
    }
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
        card_amt = 0;

    char *c = input;

    while(c && c < input + size) {
        c = strchr(c, '\n') + 1;
        card_amt++;
    }

    card_sort_t *sort_cards = (card_sort_t*)malloc(sizeof(card_sort_t) * card_amt);

    if(!sort_cards) {
        printf("Could not allocate memory\n");
        goto error;
    }

    char *labels_ptr = NULL;
    card_amt = 0;

    int types_highest_index[TYPES_AMT];
    memset((char*)&types_highest_index, -1, sizeof(types_highest_index));

    c = input;

    while(c && c < input + size) {
        if(!labels_ptr) {
            labels_ptr = c;
            c += 5;
        } else if(isdigit(*c)) {
            convert_copy_cards(&sort_cards[card_amt], labels_ptr);

            sort_cards[card_amt].bid = atoi(c);
            sort_cards[card_amt].more = -1;
            sort_cards[card_amt].less = -1;

            int type = get_type(sort_cards[card_amt].labels);

            if(types_highest_index[type] == -1)
                types_highest_index[type] = card_amt;
            else
                card_sort_add(sort_cards, card_amt, types_highest_index[type]);

            card_amt++;
            c = strchr(c, '\n');
        }

        if(*c == '\n')
            labels_ptr = NULL;

        c++;
    }

    free(input);

    int rank = 0;

    for(int i = 0; i < TYPES_AMT; i++) {
        if(types_highest_index[i]!=-1)
            card_sort_get_value(sort_cards, types_highest_index[i], &rank, &answer);
    }

    printf("The answer is %d\n", answer);

    return 0;
error:
    if(input)
        free(input);
    if(sort_cards)
        free(sort_cards);

    return 1;
}

