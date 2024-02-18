#include "scope.h"

void object_list_push(binder_context* binder, scope_object_list* list, scope_object* object) {
    // We're out of space! Reallocate
    if(list->allocated <= list->length + 1) {
        list->allocated = list->allocated * 2 + 1; // The new size is twice as large + 1 to avoid any 0-lengths.
        scope_object** new_list = (scope_object**)msalloc(binder->alloc_stack, sizeof(scope_object*) * list->allocated);

        if(list->objects != NULL) // NULL check to be safe.
        {
            for (int i = 0; i < list->length; ++i) {
                new_list[i] = list->objects[i]; // Copy over data :)
            }
            msdealloc(binder->alloc_stack, list->objects); // Free the old one
        }

        list->objects = new_list;
    }
    list->objects[list->length] = object;
    list->length++;
}

scope_object_list* new_scope_object_list(binder_context* binder) {
    scope_object_list* list = (scope_object_list*)msalloc(binder->alloc_stack, sizeof(scope_object_list));

    list->allocated = 0;
    list->length = 0;
    list->objects = NULL;

    return list;
}

scope_object* new_scope_object(binder_context* binder, scope_object_type type) {
    scope_object* object = (scope_object*)msalloc(binder->alloc_stack, sizeof(scope_object));

    object->object_type = type;
    object->children = new_scope_object_list(binder);
    object->private = 0;
    object->parent = NULL;

    return object;
}

static void print_scope_object_i(scope_object* object, int indent) {

    string type_names[] = {
            "SCOPE_OBJECT_GLOBAL",
            "SCOPE_OBJECT_PACKAGE",
            "SCOPE_OBJECT_FUNCTION",
            "SCOPE_OBJECT_VARIABLE",
            "SCOPE_OBJECT_CLASS",
            "SCOPE_OBJECT_STRUCT",
            "SCOPE_OBJECT_PARAMETER",
            "SCOPE_OBJECT_TYPE"
    };

    if(object == NULL) {
        printf("%*sNULL\n", indent * 4, "");
        return;
    }

    printf("%*s%s: %s - private: %i\n", indent * 4, "", type_names[object->object_type],
           object->name, object->private);

    for (int i = 0; i < object->children->length; ++i) {
        print_scope_object_i(object->children->objects[i], indent + 1);
    }
}

void print_scope_object(scope_object* object) {
    print_scope_object_i(object, 0);
}