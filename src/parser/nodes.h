// AUTO GENERATED!
#ifndef NODES_H
#define NODES_H

#include "parser.h"

typedef char* string;
typedef struct parser_context parser_context;

typedef enum permissions {
    PERMS_PUBLIC = 1,
    PERMS_PRIVATE = 2,
    PERMS_STATIC = 4,
    PERMS_EXTERN = 8,
    PERMS_UNSAFE = 16,
    PERMS_NONE = 0
} permissions;

typedef enum {
	NODE_NULL,
	NODE_ROOT,
	NODE_PACKAGE_DEF,
	NODE_CLASS_DEF,
	NODE_STRUCT_DEF,
	NODE_FUNCTION_DEF,
	NODE_ENUM_DEF,
	NODE_IDENTIFIER,
	NODE_FUNCTION_BODY,
	NODE_VARIABLE_DEF
} node_type;


typedef struct node {
    node_type type;
} node;

typedef struct {
    int length;
    int allocated;
    node** data;
} node_list;

void list_push(parser_context* context, node_list* list, node* data);

typedef struct node_root node_root;
typedef struct node_package_def node_package_def;
typedef struct node_class_def node_class_def;
typedef struct node_struct_def node_struct_def;
typedef struct node_function_def node_function_def;
typedef struct node_enum_def node_enum_def;
typedef struct node_identifier node_identifier;
typedef struct node_function_body node_function_body;
typedef struct node_variable_def node_variable_def;



typedef struct node_root{
	node_type type;
	node_list* children;
} node_root;

node_root* new_node_root(parser_context* parser);

node_root* as_node_root(node* n);

typedef struct node_package_def{
	node_type type;
	string package_name;
} node_package_def;

node_package_def* new_node_package_def(parser_context* parser);

node_package_def* as_node_package_def(node* n);

typedef struct node_class_def{
	node_type type;
	string name;
	permissions flags;
	node_list* children;
} node_class_def;

node_class_def* new_node_class_def(parser_context* parser);

node_class_def* as_node_class_def(node* n);

typedef struct node_struct_def{
	node_type type;
	string name;
	permissions flags;
} node_struct_def;

node_struct_def* new_node_struct_def(parser_context* parser);

node_struct_def* as_node_struct_def(node* n);

typedef struct node_function_def{
	node_type type;
	string name;
	permissions flags;
	node_identifier* return_type;
	node_function_body* body;
} node_function_def;

node_function_def* new_node_function_def(parser_context* parser);

node_function_def* as_node_function_def(node* n);

typedef struct node_enum_def{
	node_type type;
	string name;
	permissions flags;
} node_enum_def;

node_enum_def* new_node_enum_def(parser_context* parser);

node_enum_def* as_node_enum_def(node* n);

typedef struct node_identifier{
	node_type type;
	string name;
	int package;
	struct node_identifier* child;
	node_list* generic_values;
} node_identifier;

node_identifier* new_node_identifier(parser_context* parser);

node_identifier* as_node_identifier(node* n);

typedef struct node_function_body{
	node_type type;
	node_list* children;
} node_function_body;

node_function_body* new_node_function_body(parser_context* parser);

node_function_body* as_node_function_body(node* n);

typedef struct node_variable_def{
	node_type type;
	string name;
	permissions flags;
	node_identifier* value_type;
	node* default_value;
} node_variable_def;

node_variable_def* new_node_variable_def(parser_context* parser);

node_variable_def* as_node_variable_def(node* n);

void print_node(node* node, const char* name, int indent);

#ifdef NODES_PRINT_IMPL


node_list* new_node_list(parser_context* context) {
    node_list* list = (node_list*)palloc(context, sizeof(node_list));
    list->length = 0;
    list->allocated = 0;
    list->data = NULL;
    return list;
};

static void print_node_list(const char* name, node_list* list, int indent) {
    printf("%*s%s:\n", indent * 4, "", name);
    for (int i = 0; i < list->length; ++i) {
        print_node(list->data[i], NULL, indent + 1);
    }
}

static void print_string(const char* name, string s, int indent) {
    printf("%*s%s: \"%s\"\n", indent * 4, "", name, s);
}

static void print_int(const char* name, int v, int indent) {
    printf("%*s%s: \"%i\"\n", indent * 4, "", name, v);
}

void print_node_root(node_root* node, const char* name, int indent);
void print_node_package_def(node_package_def* node, const char* name, int indent);
void print_node_class_def(node_class_def* node, const char* name, int indent);
void print_node_struct_def(node_struct_def* node, const char* name, int indent);
void print_node_function_def(node_function_def* node, const char* name, int indent);
void print_node_enum_def(node_enum_def* node, const char* name, int indent);
void print_node_identifier(node_identifier* node, const char* name, int indent);
void print_node_function_body(node_function_body* node, const char* name, int indent);
void print_node_variable_def(node_variable_def* node, const char* name, int indent);


node_root* new_node_root(parser_context* parser) {
	node_root* data = (node_root*)palloc(parser, sizeof(node_root));
	memset(data, 0, sizeof(node_root));
	data->type = NODE_ROOT;
	data->children = new_node_list(parser);
	return data;
};

node_root* as_node_root(node* n) {
	return n->type == NODE_ROOT ? (node_root*)n : NULL;
}

void print_node_root(node_root* n, const char* name, int indent) {
	if(n == NULL)
		return print_node((node*)n, name, indent);
	if(name == NULL)
		printf("%*sNODE_ROOT\n", indent * 4, "");
	else
		printf("%*s%s: NODE_ROOT\n", indent * 4, "", name);
	print_node_list("children", n->children, indent + 1);
}

node_package_def* new_node_package_def(parser_context* parser) {
	node_package_def* data = (node_package_def*)palloc(parser, sizeof(node_package_def));
	memset(data, 0, sizeof(node_package_def));
	data->type = NODE_PACKAGE_DEF;
	return data;
};

node_package_def* as_node_package_def(node* n) {
	return n->type == NODE_PACKAGE_DEF ? (node_package_def*)n : NULL;
}

void print_node_package_def(node_package_def* n, const char* name, int indent) {
	if(n == NULL)
		return print_node((node*)n, name, indent);
	if(name == NULL)
		printf("%*sNODE_PACKAGE_DEF\n", indent * 4, "");
	else
		printf("%*s%s: NODE_PACKAGE_DEF\n", indent * 4, "", name);
	print_string("package_name", n->package_name, indent + 1);
}

node_class_def* new_node_class_def(parser_context* parser) {
	node_class_def* data = (node_class_def*)palloc(parser, sizeof(node_class_def));
	memset(data, 0, sizeof(node_class_def));
	data->type = NODE_CLASS_DEF;
	data->children = new_node_list(parser);
	return data;
};

node_class_def* as_node_class_def(node* n) {
	return n->type == NODE_CLASS_DEF ? (node_class_def*)n : NULL;
}

void print_node_class_def(node_class_def* n, const char* name, int indent) {
	if(n == NULL)
		return print_node((node*)n, name, indent);
	if(name == NULL)
		printf("%*sNODE_CLASS_DEF\n", indent * 4, "");
	else
		printf("%*s%s: NODE_CLASS_DEF\n", indent * 4, "", name);
	print_string("name", n->name, indent + 1);
	print_int("flags", n->flags, indent + 1);
	print_node_list("children", n->children, indent + 1);
}

node_struct_def* new_node_struct_def(parser_context* parser) {
	node_struct_def* data = (node_struct_def*)palloc(parser, sizeof(node_struct_def));
	memset(data, 0, sizeof(node_struct_def));
	data->type = NODE_STRUCT_DEF;
	return data;
};

node_struct_def* as_node_struct_def(node* n) {
	return n->type == NODE_STRUCT_DEF ? (node_struct_def*)n : NULL;
}

void print_node_struct_def(node_struct_def* n, const char* name, int indent) {
	if(n == NULL)
		return print_node((node*)n, name, indent);
	if(name == NULL)
		printf("%*sNODE_STRUCT_DEF\n", indent * 4, "");
	else
		printf("%*s%s: NODE_STRUCT_DEF\n", indent * 4, "", name);
	print_string("name", n->name, indent + 1);
	print_int("flags", n->flags, indent + 1);
}

node_function_def* new_node_function_def(parser_context* parser) {
	node_function_def* data = (node_function_def*)palloc(parser, sizeof(node_function_def));
	memset(data, 0, sizeof(node_function_def));
	data->type = NODE_FUNCTION_DEF;
	return data;
};

node_function_def* as_node_function_def(node* n) {
	return n->type == NODE_FUNCTION_DEF ? (node_function_def*)n : NULL;
}

void print_node_function_def(node_function_def* n, const char* name, int indent) {
	if(n == NULL)
		return print_node((node*)n, name, indent);
	if(name == NULL)
		printf("%*sNODE_FUNCTION_DEF\n", indent * 4, "");
	else
		printf("%*s%s: NODE_FUNCTION_DEF\n", indent * 4, "", name);
	print_string("name", n->name, indent + 1);
	print_int("flags", n->flags, indent + 1);
	print_node_identifier(n->return_type, "return_type", indent + 1);
	print_node_function_body(n->body, "body", indent + 1);
}

node_enum_def* new_node_enum_def(parser_context* parser) {
	node_enum_def* data = (node_enum_def*)palloc(parser, sizeof(node_enum_def));
	memset(data, 0, sizeof(node_enum_def));
	data->type = NODE_ENUM_DEF;
	return data;
};

node_enum_def* as_node_enum_def(node* n) {
	return n->type == NODE_ENUM_DEF ? (node_enum_def*)n : NULL;
}

void print_node_enum_def(node_enum_def* n, const char* name, int indent) {
	if(n == NULL)
		return print_node((node*)n, name, indent);
	if(name == NULL)
		printf("%*sNODE_ENUM_DEF\n", indent * 4, "");
	else
		printf("%*s%s: NODE_ENUM_DEF\n", indent * 4, "", name);
	print_string("name", n->name, indent + 1);
	print_int("flags", n->flags, indent + 1);
}

node_identifier* new_node_identifier(parser_context* parser) {
	node_identifier* data = (node_identifier*)palloc(parser, sizeof(node_identifier));
	memset(data, 0, sizeof(node_identifier));
	data->type = NODE_IDENTIFIER;
	data->generic_values = new_node_list(parser);
	return data;
};

node_identifier* as_node_identifier(node* n) {
	return n->type == NODE_IDENTIFIER ? (node_identifier*)n : NULL;
}

void print_node_identifier(node_identifier* n, const char* name, int indent) {
	if(n == NULL)
		return print_node((node*)n, name, indent);
	if(name == NULL)
		printf("%*sNODE_IDENTIFIER\n", indent * 4, "");
	else
		printf("%*s%s: NODE_IDENTIFIER\n", indent * 4, "", name);
	print_string("name", n->name, indent + 1);
	print_int("package", n->package, indent + 1);
	print_node_identifier(n->child, "child", indent + 1);
	print_node_list("generic_values", n->generic_values, indent + 1);
}

node_function_body* new_node_function_body(parser_context* parser) {
	node_function_body* data = (node_function_body*)palloc(parser, sizeof(node_function_body));
	memset(data, 0, sizeof(node_function_body));
	data->type = NODE_FUNCTION_BODY;
	data->children = new_node_list(parser);
	return data;
};

node_function_body* as_node_function_body(node* n) {
	return n->type == NODE_FUNCTION_BODY ? (node_function_body*)n : NULL;
}

void print_node_function_body(node_function_body* n, const char* name, int indent) {
	if(n == NULL)
		return print_node((node*)n, name, indent);
	if(name == NULL)
		printf("%*sNODE_FUNCTION_BODY\n", indent * 4, "");
	else
		printf("%*s%s: NODE_FUNCTION_BODY\n", indent * 4, "", name);
	print_node_list("children", n->children, indent + 1);
}

node_variable_def* new_node_variable_def(parser_context* parser) {
	node_variable_def* data = (node_variable_def*)palloc(parser, sizeof(node_variable_def));
	memset(data, 0, sizeof(node_variable_def));
	data->type = NODE_VARIABLE_DEF;
	return data;
};

node_variable_def* as_node_variable_def(node* n) {
	return n->type == NODE_VARIABLE_DEF ? (node_variable_def*)n : NULL;
}

void print_node_variable_def(node_variable_def* n, const char* name, int indent) {
	if(n == NULL)
		return print_node((node*)n, name, indent);
	if(name == NULL)
		printf("%*sNODE_VARIABLE_DEF\n", indent * 4, "");
	else
		printf("%*s%s: NODE_VARIABLE_DEF\n", indent * 4, "", name);
	print_string("name", n->name, indent + 1);
	print_int("flags", n->flags, indent + 1);
	print_node_identifier(n->value_type, "value_type", indent + 1);
	print_node(n->default_value, "", indent + 1);
}

void print_node(node* node, const char* name, int indent)
{
    if(node == NULL) {
        if(name != NULL)
            printf("%*s%s: NULL\n", indent * 4, "", name);
        else
            printf("%*sNULL\n", indent * 4, "");
        return;
    }
    
    switch (node->type) {
        case NODE_NULL:
            break;
		case NODE_ROOT:
			print_node_root((node_root*)node, name, indent);
			break;
		case NODE_PACKAGE_DEF:
			print_node_package_def((node_package_def*)node, name, indent);
			break;
		case NODE_CLASS_DEF:
			print_node_class_def((node_class_def*)node, name, indent);
			break;
		case NODE_STRUCT_DEF:
			print_node_struct_def((node_struct_def*)node, name, indent);
			break;
		case NODE_FUNCTION_DEF:
			print_node_function_def((node_function_def*)node, name, indent);
			break;
		case NODE_ENUM_DEF:
			print_node_enum_def((node_enum_def*)node, name, indent);
			break;
		case NODE_IDENTIFIER:
			print_node_identifier((node_identifier*)node, name, indent);
			break;
		case NODE_FUNCTION_BODY:
			print_node_function_body((node_function_body*)node, name, indent);
			break;
		case NODE_VARIABLE_DEF:
			print_node_variable_def((node_variable_def*)node, name, indent);
			break;
   }
}

#endif

#endif
