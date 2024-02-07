#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define MAX_SLOT 256

#define CMD_RM '-'
#define CMD_SET '='
#define CMD_END '\n'
#define CMD_SEQ_END ','

typedef struct lens_data_t {
    char *label;

    int power;
    int box;
    int slot;

    int enabled;
} lens_data_t;

typedef struct node_t {
    struct node_t *left;
    struct node_t *right;

    void *data;
    int data_size;
} node_t;

typedef struct boxes_t {
    int slot_index;
    node_t *head;
} boxes_t;

node_t *create_node(void *data, int data_size) {
    node_t *new_node = (node_t *)malloc(data_size + sizeof(node_t));

    new_node->data = (void*)(new_node + 1);
    new_node->data_size = data_size;

    memcpy(new_node->data, data, data_size);

    return new_node;
}


void lens_update(void *data1, void *data2) {
    lens_data_t *lens_out = (lens_data_t *)data1;
    lens_data_t *lens_in = (lens_data_t *)data2;

    if(!lens_in->enabled) {
        lens_out->enabled = 0;
    } else {
        if(!lens_out->enabled)
            lens_out->slot = lens_in->slot;

        lens_out->power = lens_in->power;
        lens_out->enabled = 1;
    }
}

int lens_compare(void *data1, void *data2) {
    lens_data_t *lens_out = (lens_data_t *)data1;
    lens_data_t *lens_in = (lens_data_t *)data2;

    return strcmp(lens_out->label, lens_in->label);
}

int lens_compare_slot(void *data1, void *data2) {
    lens_data_t *lens_out = (lens_data_t *)data1;
    lens_data_t *lens_in = (lens_data_t *)data2;

    return lens_out->slot - lens_in->slot;
}

node_t *add_node(node_t *head,
                int (*compare_func)(void *data1, void *data2),
                void (*update_func)(void *data1, void *data2),
                void *data,
                int data_size) {

    if(!head)
        return create_node(data, data_size);

    int compare = compare_func(data, head->data);

    if(update_func && !compare)
        update_func(head->data, data);

    if(compare > 0) {
        if(head->right)
            return add_node(head->right, compare_func, update_func, data, data_size);

        head->right = add_node(head->right, compare_func, update_func, data, data_size);
    }

    if(compare < 0) {
        if(head->left)
            return add_node(head->left, compare_func, update_func, data, data_size);

        head->left = add_node(head->left, compare_func, update_func, data, data_size);
    }

    return NULL;
}


void sort_box(boxes_t *boxes, node_t *head) {
    if(!head)
        return;

    lens_data_t *data_in = head->data;

    if(data_in->enabled) {
        node_t *new_head = add_node(boxes[data_in->box].head, lens_compare_slot, NULL, head->data, head->data_size);

        if(!boxes[data_in->box].head)
            boxes[data_in->box].head = new_head;
    }

    sort_box(boxes, head->left);
    sort_box(boxes, head->right);
}


int compute_lens(node_t *head, int *slot) {
    if(!head)
        return 0;

    int focus_power = 0;

    if(head->left)
        focus_power += compute_lens(head->left, slot);

    lens_data_t *data = (lens_data_t *)head->data;

    focus_power += (data->box + 1) * (*slot) * data->power;

    (*slot)++;

    if(head->right)
        focus_power += compute_lens(head->right, slot);

    free(head);

    return focus_power;
}

int calc_box(boxes_t *boxes) {
    int answer = 0;

    return answer;
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
        cmd = 0;

    char *c = input;

    node_t *head = NULL;
    node_t *new_head = NULL;

    boxes_t boxes[MAX_SLOT];
    memset(&boxes, 0, sizeof(boxes));


    lens_data_t temp_lens;

    temp_lens.label = c;

    while(c && c < input + size) {
        switch(*c) {
            case CMD_SET:
            case CMD_RM:
                cmd = *c;
                *c = 0;
                break;
            case CMD_SEQ_END:
                memset(&temp_lens, 0, sizeof(temp_lens));
                temp_lens.label = c + 1;

            case CMD_END:
                break;
            default:
                temp_lens.box += *c;
                temp_lens.box *= 17;
                temp_lens.box %= 256;
                break;
        }

        switch(cmd) {
            case CMD_SET:
                c++;
                temp_lens.power = atoi(c);
                temp_lens.slot = boxes[temp_lens.box].slot_index++;
                temp_lens.enabled = 1;
            case CMD_RM:
                new_head = add_node(head, lens_compare, lens_update, (void*)&temp_lens, sizeof(lens_data_t));
                break;
        }

        if(new_head) {
            head = new_head;
            new_head = NULL;
        }

        cmd = 0;

        c++;
    }

    sort_box(&boxes, head);

    for(int i = 0; i < MAX_SLOT; i++) {
        int slot = 1;

        answer += compute_lens(boxes[i].head, &slot);
    }

    printf("The answer is %d\n", answer);

    free(input);
    return 0;
error:
    return 1;
}

