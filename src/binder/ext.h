#pragma once

typedef unsigned long bind_ext_object;

typedef enum bind_ext_object_kind {
    EXT_OBJECT_KIND_GLOBAL_SCOPE,
    EXT_OBJECT_KIND_VARIABLE,
    EXT_OBJECT_KIND_FUNCTION,
    EXT_OBJECT_KIND_CLASS,
    EXT_OBJECT_KIND_STRUCT,

    EXT_OBJECT_KIND_PARAMETER,
    EXT_OBJECT_KIND_RETURN_VALUE,
    EXT_OBJECT_KIND_GENERIC_PARAMETER,
    EXT_OBJECT_KIND_TYPE
} bind_ext_object_kind;

extern const char* OBJECT_KIND_STR[];

typedef enum bind_ext_access_level {
    EXT_ACCESS_LEVEL_PRIVATE, // private or static private
    EXT_ACCESS_LEVEL_PUBLIC,  // public
    EXT_ACCESS_LEVEL_STATIC   // static public
} bind_ext_access_level;

// Get how many of something there is within an external object
// Returns the number of child IDs
typedef int (*bind_ext_count_fetch)(bind_ext_object object);
// Get a child object from a parent object.
typedef bind_ext_object (*bind_ext_object_fetch)(bind_ext_object object, int index);
// Get the name of thing "index" within "object", stored in buffer - Returns
typedef int (*bind_ext_value_fetch)(bind_ext_object object, char* buffer);
// Get the object kind from the object ID.
typedef bind_ext_object_kind (*bind_ext_kind_fetch)(bind_ext_object object);
// Get the access level from object ID
typedef bind_ext_access_level (*bind_ext_level_fetch)(bind_ext_object object);
// Get the global scope of a package name
typedef bind_ext_object (*bind_ext_get_package)(char* name);

// Interface for loading external libraries & symbols
typedef struct bind_ext_resolver {
    bind_ext_count_fetch count_fetch;
    bind_ext_object_fetch object_fetch;
    bind_ext_value_fetch value_fetch;
    bind_ext_kind_fetch kind_fetch;
    bind_ext_level_fetch level_fetch;

    bind_ext_get_package get_package;
} bind_ext_resolver;