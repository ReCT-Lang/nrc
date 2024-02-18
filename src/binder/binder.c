#include "binder.h"
#include <stdlib.h>
#include <string.h>
#include "scope.h"
#include <errors/error.h>

typedef enum binder_build_allowed {
    ALLOW_PACKAGES  = 0b0000001,
    ALLOW_CLASSES   = 0b0000010,
    ALLOW_STRUCTS   = 0b0000100,
    ALLOW_FUNCTIONS = 0b0001000,
    ALLOW_VARIABLES = 0b0010000,

    ALLOW_NONE      = 0b0000000,
    ALLOW_ALL       = 0b0011111
} binder_build_allowed;

scope_object* binder_build_object(binder_context* binder, node* node, binder_build_allowed allowed, scope_object* parent);

static string copy_string(binder_context* context, string src) {
    string str = (string)msalloc(context->alloc_stack, (int)strlen(src) + 1);
    strcpy(str, src);
    return str;
}

static string read_value(binder_context* context, bind_ext_resolver resolver, bind_ext_object object) {
    int length = resolver.value_fetch(object, NULL);
    string s = (string)msalloc(context->alloc_stack, length + 1);
    resolver.value_fetch(object, s);
    return s;
}

binder_context* binder_create(bind_ext_resolver resolver) {
    binder_context* binder = (binder_context*)malloc(sizeof(binder_context));

    binder->resolver = resolver;
    binder->alloc_stack = msnew();
    binder->program_nodes = NULL;
    binder->program_node_count = 0;

    return binder;
}

void binder_mount(binder_context* binder, node_root* program_node) {
    // For now, we'll not bother about performant lists, since we won't do *that* many adds.
    // Like, one thousand isn't too damn bad

    node_root** new_array = msalloc(binder->alloc_stack, binder->program_node_count + 1);
    for (int i = 0; i < binder->program_node_count; ++i) {
        new_array[i] = binder->program_nodes[i];
    }
    msdealloc(binder->alloc_stack, binder->program_nodes);
    binder->program_nodes = new_array;
    binder->program_nodes[binder->program_node_count] = program_node;
    binder->program_node_count++;
}

scope_object* binder_build_ext(binder_context* binder, bind_ext_resolver resolver, bind_ext_object object, scope_object* parent) {
    bind_ext_object_kind kind = resolver.kind_fetch(object);

    if(kind == EXT_OBJECT_KIND_FUNCTION) {
        // Return a function
        scope_object* function = new_scope_object(binder, SCOPE_OBJECT_FUNCTION);
        function->name = read_value(binder, resolver, object);
        function->private = resolver.level_fetch(object) == EXT_ACCESS_LEVEL_PRIVATE;
        function->parent = parent;

        int values = resolver.count_fetch(object);

        for (int i = 0; i < values; ++i) {
            bind_ext_object value = resolver.object_fetch(object, i);
            bind_ext_object_kind value_kind = resolver.kind_fetch(value);

            // Parameters
            if(value_kind == EXT_OBJECT_KIND_PARAMETER) {
                scope_object* param = new_scope_object(binder, SCOPE_OBJECT_PARAMETER);
                param->name = read_value(binder, resolver, value);
                function->private = 0;
                function->parent = function;

                // We assume a child(or whetever else you want me to say)
                bind_ext_object p = resolver.object_fetch(value, 0);
                scope_object* param_t = new_scope_object(binder, SCOPE_OBJECT_TYPE);
                param->name = read_value(binder, resolver, p);
                function->private = 0;
                function->parent = param;

                object_list_push(binder, function->children, param);
            }
        }

        return function;
    }

    return NULL;
}

scope_object* binder_build_package(binder_context* binder, node_package_def* node) {
    scope_object* object = new_scope_object(binder, SCOPE_OBJECT_PACKAGE);

    object->name = copy_string(binder, node->package_name);

    bind_ext_resolver resolver = binder->resolver;

    bind_ext_object package_obj = resolver.get_package(node->package_name);
    for (int i = 0; i < resolver.count_fetch(package_obj); ++i) {
        scope_object* obj = binder_build_ext(binder, resolver, resolver.object_fetch(package_obj, i), object);
        if(obj != NULL)
            object_list_push(binder, object->children, obj);
    }

    return object;
}

scope_object* binder_build_class(binder_context* binder, node_class_def* node, scope_object* parent) {
    scope_object* object = new_scope_object(binder, SCOPE_OBJECT_CLASS);
    object->parent = parent;
    object->name = copy_string(binder, node->name);
    object->private = (node->flags & PERMS_PRIVATE) || !(node->flags & PERMS_PUBLIC);

    for (int i = 0; i < node->body->children->length; ++i) {
        scope_object* o =
                binder_build_object(binder, node->body->children->data[i],
                                    ALLOW_CLASSES | ALLOW_FUNCTIONS | ALLOW_VARIABLES | ALLOW_STRUCTS, object);
        if(o != NULL)
            object_list_push(binder, object->children, o);
    }

    return object;
}

scope_object* binder_build_struct(binder_context* binder, node_struct_def* node, scope_object* parent) {
    scope_object* object = new_scope_object(binder, SCOPE_OBJECT_STRUCT);
    object->parent = parent;
    object->name = copy_string(binder, node->name);
    object->private = (node->flags & PERMS_PRIVATE) || !(node->flags & PERMS_PUBLIC);

    for (int i = 0; i < node->body->children->length; ++i) {
        scope_object* o =
                binder_build_object(binder, node->body->children->data[i],
                                    ALLOW_FUNCTIONS | ALLOW_VARIABLES, object);
        if(o != NULL)
            object_list_push(binder, object->children, o);
    }

    return object;
}

scope_object* binder_build_function(binder_context* binder, node_function_def* node, scope_object* parent) {

    scope_object* object = new_scope_object(binder, SCOPE_OBJECT_FUNCTION);
    object->parent = parent;
    object->name = copy_string(binder, node->name);
    object->private = (node->flags & PERMS_PRIVATE) || !(node->flags & PERMS_PUBLIC);

    if(node->body == NULL)
        return object;

    for (int i = 0; i < node->body->children->length; ++i) {
        scope_object* o =
                binder_build_object(binder, node->body->children->data[i],
                                    ALLOW_FUNCTIONS | ALLOW_VARIABLES, object);
        if(o != NULL)
            object_list_push(binder, object->children, o);
    }

    return object;
}

scope_object* binder_build_variable(binder_context* binder, node_variable_def* node, scope_object* parent) {

    scope_object* object = new_scope_object(binder, SCOPE_OBJECT_VARIABLE);
    object->parent = parent;
    object->name = copy_string(binder, node->name);
    object->private = (node->flags & PERMS_PRIVATE) || !(node->flags & PERMS_PUBLIC);

    return object;
}

scope_object* binder_build_object(binder_context* binder, node* node, binder_build_allowed allowed, scope_object* parent) {

    // TODO: Throw errors
    if(node->type == NODE_PACKAGE_DEF && (allowed & ALLOW_PACKAGES))
        return binder_build_package(binder, as_node_package_def(node));
    if(node->type == NODE_CLASS_DEF && (allowed & ALLOW_CLASSES))
        return binder_build_class(binder, as_node_class_def(node), parent);
    if(node->type == NODE_STRUCT_DEF && (allowed & ALLOW_STRUCTS))
        return binder_build_struct(binder, as_node_struct_def(node), parent);
    if(node->type == NODE_FUNCTION_DEF && (allowed & ALLOW_FUNCTIONS))
        return binder_build_function(binder, as_node_function_def(node), parent);
    if(node->type == NODE_VARIABLE_DEF && (allowed & ALLOW_VARIABLES))
        return binder_build_variable(binder, as_node_variable_def(node), parent);

    return NULL;
}

scope_object* binder_build_global(binder_context* binder, node_root* root) {
    scope_object* global_scope = new_scope_object(binder, SCOPE_OBJECT_GLOBAL);

    for (int i = 0; i < root->children->length; ++i) {
        scope_object* o = binder_build_object(binder, root->children->data[i], ALLOW_ALL, global_scope);
        if(o != NULL)
            object_list_push(binder, global_scope->children, o);
    }

    return global_scope;
}

void binder_validate(binder_context* binder) {
    // First we build our program
    for (int i = 0; i < binder->program_node_count; ++i) {
        scope_object* object = binder_build_global(binder, binder->program_nodes[i]);
        print_scope_object(object);
    }
}

void binder_destroy(binder_context* binder) {
    msfree(binder->alloc_stack);
    free(binder);
}