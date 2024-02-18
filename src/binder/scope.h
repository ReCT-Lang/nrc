#pragma once

#include "binder.h"
#include "ext.h"

// There are a number of different scope properties:

typedef enum scope_object_type {
    SCOPE_OBJECT_GLOBAL,
    SCOPE_OBJECT_PACKAGE,
    SCOPE_OBJECT_FUNCTION,
    SCOPE_OBJECT_VARIABLE,
    SCOPE_OBJECT_CLASS,
    SCOPE_OBJECT_STRUCT,
    SCOPE_OBJECT_PARAMETER,
    SCOPE_OBJECT_TYPE
} scope_object_type;

typedef struct scope_object_list scope_object_list;
typedef struct scope_object scope_object;

typedef struct scope_object {
    scope_object_type object_type;
    scope_object_list* children;
    char* name;
    scope_object* parent;
    int private;
} scope_object;

scope_object* new_scope_object(binder_context* binder, scope_object_type type);

typedef struct scope_object_list {
    scope_object** objects;
    int length;
    int allocated;
} scope_object_list;
void object_list_push(binder_context* binder, scope_object_list* list, scope_object* object);
scope_object_list* new_scope_object_list(binder_context* binder);

void print_scope_object(scope_object* object);