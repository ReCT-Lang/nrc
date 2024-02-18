#pragma once
#include "ext.h"
#include <parser/nodes.h>
#include <memstack.h>

typedef struct binder_context {
    bind_ext_resolver resolver;
    memstack* alloc_stack;

    node_root** program_nodes;
    int program_node_count;
} binder_context;

binder_context* binder_create(bind_ext_resolver resolver);
void binder_mount(binder_context* binder, node_root* program_node);
void binder_validate(binder_context* binder);
void binder_destroy(binder_context* binder);