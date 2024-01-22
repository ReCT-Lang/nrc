# Access modifiers
ReCT specifies a number of access modifiers, and we found
that we should try and give each one a purpose.

| Name    | Effect                                   |
|---------|------------------------------------------|
| private | private                                  |
| set     | public                                   |
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

| Item      | Valid modifiers                  |
|-----------|----------------------------------|
| variables | var, set, static, extern         |
| functions | var, set, static, unsafe, extern |
| classes   | var, set, static, unsafe         |
| structs   | var, set, unsafe                 |