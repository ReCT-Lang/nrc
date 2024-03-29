#define NODES_PRINT_IMPL

#include <string.h>
#include "nodes.h"

void list_push(parser_context* context, node_list* list, node* data) {
    // If we can't fit more data into the list, we need to allocate more memory.
    if(list->allocated < list->length + 1) {
        if(list->data == NULL) {
            // A new list won't have any data
            list->data = palloc(context, 1);
            list->allocated = 1;
        } else {
            node** new_data = msalloc(context->allocation_stack, (int)sizeof(node*) * list->allocated * 2);
            for (int i = 0; i < list->length; ++i) {
                new_data[i] = list->data[i];
            }
            msdealloc(context->allocation_stack, list->data);
            list->allocated = list->allocated * 2;
            list->data = new_data;
        }
    }
    list->data[list->length] = data;
    list->length++;
}