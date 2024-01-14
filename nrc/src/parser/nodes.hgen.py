# Wacky code that generates all node types & functions.
# This is pretty much required.

import json

print("Generating nodes")
output_f = open("nodes.h", "w")
output = '''// AUTO GENERATED!
#ifndef NODES_H
#define NODES_H

#include "parser.h"

typedef char* string;
typedef struct parser_context parser_context;

typedef enum {
\tNODE_NULL,
\t'''

defs = open("nodes.json")
definitions = json.load(defs)
defs.close()

output += ',\n\t'.join([k.upper() for k in definitions.keys()])
output += "\n} node_type;\n\n"

output += '''
typedef struct node {
    node_type type;
} node;

typedef struct {
    int length;
    int allocated;
    node** data;
} node_list;

void list_push(parser_context* context, node_list* list, node* data)

'''

for t in definitions.keys():
    data = definitions[t]
    output += "typedef struct " + t + "{\n"
    output += "\tnode_type type;\n"
    for fieldName in data.keys():
        dataType = data[fieldName]
        if dataType == "node_list":
            dataType += "*"
        if dataType == "node":
            dataType += "*"
        if dataType in definitions:
            dataType += "*"
        output += "\t" + dataType + " " + fieldName + ";\n"
    output += "} " + t + ";\n\n"
    output += t + "* new_" + t + "(parser_context* parser);\n\n"
    output += t + "* as_" + t + "(node* n);\n\n"

output += "void print_node(node* node, int indent);\n\n"
output += "#ifdef NODES_PRINT_IMPL\n\n"

output += '''
node_list* new_node_list(parser_context* context) {
    node_list* list = (node_list*)palloc(context, sizeof(node_list));
    list->length = 0;
    list->allocated = 0;
    list->data = NULL;
    return list;
};

static void print_node_list(const char* name, node_list* list, int indent) {
    printf("%*s %s:\\n", indent, "", name);
    for (int i = 0; i < list->length; ++i) {
        print_node(list->data[i], indent + 1);
    }
}

static void print_string(const char* name, string s, int indent) {
    printf("%*s %s: \\"%s\\"\\n", indent, "", name, s);
}\n\n'''

for t in definitions.keys():
    # Generate node type functions such as print & new
    output += t + "* new_" + t + "(parser_context* parser) {\n"
    output += "\t" + t + "* data = (" + t + "*)palloc(parser, sizeof(" + t + "));\n"
    output += "\t" + "memset(data, 0, sizeof(" + t + "));\n"
    output += "\t" + "data->type = " + t.upper() + ";\n"

    for f in definitions[t].keys():
        if definitions[t][f] == "node_list":
            output += "\tdata->" + f + " = new_node_list(parser);\n"

    output += "\t" + "return data;\n"
    output += "};\n\n"

    output += t + "* as_" + t + "(node* n) {\n"
    output += "\treturn n->type == " + t.upper() + " ? (" + t + "*)n : NULL;\n}\n\n"

    output += "void print_" + t + "(" + t + "* node, int indent) {\n"
    output += '\tprintf("%*s ' + t.upper() + ':\\n", indent, "");\n'

    fields = definitions[t]

    for name in fields.keys():
        if fields[name] == "node_list":
            output += "\tprint_node_list(\"" + name + "\", node->" + name + ", indent + 1);\n"
        elif fields[name] == "string":
            output += "\tprint_string(\"" + name + "\", node->" + name + ", indent + 1);\n"
        elif fields[name] == "node":
            output += "\tprintf(\"" + name + ":\" );\n\tprint_node(node->" + name + ", indent + 1);\n"
        elif fields[name] in definitions:
            output += "\tprintf(\"" + name + ":\" );\n\tprint_" + fields[name] + "(node->" + name + ", indent + 1);\n"

    output += "}\n\n"

output += '''void print_node(node* node, int indent)
{
    if(node == NULL) {
        printf("%*s NULL\\n", indent, "");
        return;
    }
    
    switch (node->type) {
        case NODE_NULL:
            break;
'''

for t in definitions.keys():
    output += "\t\tcase " + t.upper() + ":\n"
    output += "\t\t\tprint_" + t + "((" + t + "*)node, indent + 1);\n"
    output += "\t\t\tbreak;\n"

output += '''   }
}

'''

output += "#endif\n\n#endif\n"

output_f.write(output)
output_f.close()
