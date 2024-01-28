# Access modifiers
ReCT specifies a number of access modifiers, and we found
that we should try and give each one a purpose.

| Name    | Effect                                   |
|---------|------------------------------------------|
| private | May only be accessed within scope        |
| set     | May be accessed outside scope            |
| static  | class-wide(not on instances)             |
| unsafe  | unmanaged code allowed(pointers)         |
| extern  | External(may not define function body)   |
| mine    | (NRC_JOKES) Compiles to TOKEN_KW_PRIVATE |

**private** and **set** cannot be used simultaneously.

**static** functions may not access instance stuff.

**unsafe** functions may use pointers.

**extern** may not define a function body, and should be
resolved from external binaries.

## Valid access modifiers on different "items"

| Item      | Valid modifiers                      |
|-----------|--------------------------------------|
| variables | private, set, static, extern         |
| functions | private, set, static, unsafe, extern |
| classes   | private, set, static, unsafe         |
| structs   | private, set, unsafe                 |