#include "node.h"
#include <stdio.h>

static void print_node_list(const char* name, node_list* list, int indent) {
    printf("%*s %s:\n", indent, "", name);
    for (int i = 0; i < list->length; ++i) {
        print_node(list->data[i], indent + 1);
    }
}

static void print_root(node* node, int indent) {
    printf("%*s NODE_ROOT:\n", indent, "");
    print_node_list("children", ((node_root*)node)->children, indent + 1);
}

static void print_package_def(node* node, int indent) {

}

void print_node(node* node, int indent)
{
    if(node == NULL) {
        printf("%*s NULL\n", indent, "");
        return;
    }

    switch (node->type) {
        case NODE_NULL:
            break;
        case NODE_ROOT:
            print_root(node, indent);
            break;
        case NODE_PACKAGE_DEF:
            print_package_def(node, indent);
            break;
    }
}