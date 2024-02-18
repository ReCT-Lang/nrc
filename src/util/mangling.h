#pragma once
#include <parser/nodes.h>
#include <binder/scope.h>

typedef char* mangle;

mangle mangle_identifier(memstack* alloc_stack, node_identifier* identifier, scope_object_type object_type);