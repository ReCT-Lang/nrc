// AUTO GENERATED!
#ifndef NODES_H
#define NODES_H

#include "parser.h"

typedef char* string;
struct parser_context;

typedef enum {
	NODE_NULL,
	NODE_ROOT,
	NODE_PACKAGE_DEF,
	NODE_FUNCTION
} node_type;


typedef struct node {
    node_type type;
} node;

typedef struct {
    int length;
    int allocated;
    node** data;
} node_list;

typedef struct node_root{
	node_type type;
	node_list* children;
} node_root;

node_root* new_node_root(struct parser_context* parser);

typedef struct node_package_def{
	node_type type;
	string package_name;
} node_package_def;

node_package_def* new_node_package_def(struct parser_context* parser);

typedef struct node_function{
	node_type type;
	node* thing;
	node_root* thing2;
} node_function;

node_function* new_node_function(struct parser_context* parser);

void print_node(node* node, int indent);

#ifdef NODES_PRINT_IMPL


node_list* new_node_list(struct parser_context* context) {
    node_list* list = (node_list*)palloc(context, sizeof(node_list));
    list->length = 0;
    list->allocated = 0;
    list->data = NULL;
    return list;
};

static void print_node_list(const char* name, node_list* list, int indent) {
    printf("%*s %s:\n", indent, "", name);
    for (int i = 0; i < list->length; ++i) {
        print_node(list->data[i], indent + 1);
    }
}

static void print_string(const char* name, string s, int indent) {
    printf("%*s %s: \"%s\"\n", indent, "", name, s);
}

node_root* new_node_root(struct parser_context* parser) {
	node_root* data = (node_root*)palloc(parser, sizeof(node_root));
	memset(data, 0, sizeof(node_root));
	data->type = NODE_ROOT;
	data->children = new_node_list(parser);
	return data;
};

void print_node_root(node_root* node, int indent) {
	printf("%*s NODE_ROOT:\n", indent, "");
	print_node_list("children", node->children, indent + 1);
}

node_package_def* new_node_package_def(struct parser_context* parser) {
	node_package_def* data = (node_package_def*)palloc(parser, sizeof(node_package_def));
	memset(data, 0, sizeof(node_package_def));
	data->type = NODE_PACKAGE_DEF;
	return data;
};

void print_node_package_def(node_package_def* node, int indent) {
	printf("%*s NODE_PACKAGE_DEF:\n", indent, "");
	print_string("package_name", node->package_name, indent + 1);
}

node_function* new_node_function(struct parser_context* parser) {
	node_function* data = (node_function*)palloc(parser, sizeof(node_function));
	memset(data, 0, sizeof(node_function));
	data->type = NODE_FUNCTION;
	return data;
};

void print_node_function(node_function* node, int indent) {
	printf("%*s NODE_FUNCTION:\n", indent, "");
	printf("thing:" );
	print_node(node->thing, indent + 1);
	printf("thing2:" );
	print_node_root(node->thing2, indent + 1);
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
			print_node_root((node_root*)node, indent + 1);
			break;
		case NODE_PACKAGE_DEF:
			print_node_package_def((node_package_def*)node, indent + 1);
			break;
		case NODE_FUNCTION:
			print_node_function((node_function*)node, indent + 1);
			break;
   }
}

#endif

#endif
