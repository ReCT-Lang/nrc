# External Resolver - bind_ext_resolver
Within the binder there exists a struct called `bind_ext_resolver`. The task of this struct is to
provide a common interface for loading package symbols within the binder. Here's how it works:

## Objects
An object can be *anything* that a library exposes. It is identified by a `bind_ext_object` and can have:
- An unspecified amount of child objects - Amount fetched via a `bind_ext_count_fetch`
- A value string, such as the name or type - Fetched via a `bind_ext_value_fetch`
- An object kind, represented with `bind_ext_object_kind` - Fetched via a `bind_ext_kind_fetch`
- An access level, represented with `bind_ext_access_level` - Fetched via a `bind_ext_level_fetch`
Child objects may be fetched using a `bind_ext_object_fetch`.

The global scope object of any package should be fetched via a `bind_ext_get_package`

## Resolver Interface
The resolver interface is nothing complex - It is just a holder struct for a number of function pointers.

## Example Usage
Here's an example of how a binder may use the interface:
```c
bind_ext_resolver resolver; // This would be something you fed in

bind_ext_object sys_package = resolver.get_package("sys"); // We get the sys package

int sys_child_objects = resolver.count_fetch(sys_package); // Get how many child objects there are.
for(int i = 0; i < sys_child_objects) {
    
    // Get child object at index "i"
    bind_ext_object child_object = resolver.object_fetch(sys_package, i);
    
    // Get kind & access level
    bind_ext_object_kind kind = resolver.kind_fetch(child_object);
    bind_ext_access_level access_level = resolver.level_fetch(child_object);
    
    // All public package-wide functions should be static
    if(kind == EXT_OBJECT_KIND_FUNCTION && access_level == EXT_ACCESS_LEVEL_STATIC) {
        int value_length = resolver.value_fetch(child_object, NULL); // We get the string length
        char* value_buffer = malloc(value_length + 1);  // We allocate our string buffer
        resolver.value_fetch(child_object, value_buffer); // We fill our string buffer
        
        printf("Found public static package-wide function: %s\n", value_buffer);
        
        free(value_buffer);
        
    }
    
}

```