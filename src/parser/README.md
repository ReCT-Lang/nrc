# The NRC parser
This is the source dir for the parser code!  
The parser should take our tokenized code and turn it into an AST(i guess?)

Now, there are a couple of things that need explaining in here:

## The node struct
Nodes follow an OOP approach to development, which each node type
basically inheriting from the node struct.

At the top of each node definition lies the "type" field, which tells
you what type a node is.

Casting back and forth can be done either using pure casts, or
if you want to be safe, using the `as_[type]` function, which returns null
if the node isn't of the right type.

Creation of nodes is done using `new_[type]`, and it returns a zero-ed pointer of
appropriate size. The `type` and any `node_list` fields will be set.

## nodes.hgen.py & nodes.json
Nodes are defined using a json file, and the header file + code is auto-generated
by nodes.hgen.py. node_impl.c then takes care of implementing all node functions as
defined in the nodes.h file.

You can regenerate nodes.h by either running `python nodes.hgen.py` or `make src/parser/nodes.h`.

There's no dependency on nodes.json, so you'll have to remove nodes.h to regen.

## palloc
`palloc` is the replacement for malloc in parser contexts.
The function allocates memory and tracks all allocations in a
parser context, so it is easy to free on parser destruction.

`malloc` is only allowed for temporary objects, and must be freed.