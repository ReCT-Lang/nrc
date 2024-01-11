#pragma once

// Fun fact of the day: C is not object oriented(but I wish it was).
// This means it doesn't have inheritance, and thus doesn't work well for these kinds of node trees.
// So, how can we solve this? Well, there are two ways!

// 1. We store a common denominator in each node, where the first field always is a type definition
//  like they do in Vulkan and the sType field. We then keep each node stored by-reference instead of
//  by-value. This "works" I suppose, but it means a lot of memory management and allocations left and right.

// 2. We have a gigantic struct with each possible value in it. This looks ugly, but it'll "work".

// So basically, we need to pick our poison.

// That aside I chose to use #1 but using a library called memstack(partially authored by yours truly).
// memstack will handle free-ing all the memory when we're done, so we won't have to worry!

typedef enum {
    NODE_NULL,
    NODE_ROOT,
    NODE_PACKAGE_DEF
} node_type;

typedef struct {
    node_type type;
} node;

typedef struct {
    node_type type;
} node_invalid;

typedef struct {
    int length;
    int allocated;
    node** data;
} node_list;

typedef struct {
    node_type type;
    node_list* children;
} node_root;

typedef struct {
    node_type type;
    char* package_name;
} package_def_node;

void print_node(node* node, int indent);