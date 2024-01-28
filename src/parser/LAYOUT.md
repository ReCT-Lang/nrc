**NOTE: Design document & thinker. Do not take seriously.**

All right. So. How?

How do we want to do this?

I'd suspect we want to split the code up into parts first, and then design our parser around that, and not the other
way around.

So, how is a program structured?

Well, we have a root "program" or "file" part, which contains everything else.

This part could contain everything for now, since it's up to the next step to actually figure out if it makes sense
to do thing A in place B.

Let's treat a class body, function body & program file as the same thing parser-wide, that means it consists of
**statements**

Now, what can a statement be?
- Class declaration
- Struct declaration
- Function declaration
- Variable declaration
- Flow control statement
- Assign statement
- Function call.

Some of these statements are ended with the TOKEN_STMT_END token(;), and others are not.

Sometimes we support statements within statements, such as within if, for and assigns. These should also not be
ended with the TOKEN_STMT_END token.

So, how do we treat this? Let's look at rgoc.

So, a bit of looking later, and it seems like rgoc just kinda allows you to not do semicolons...

I feel like I'll enforce but do it via a parameter(since flags do be pretty).

So, note for definitions:

Definition functions handle their own damn semicolons. This is because we can assume they're only valid in certain
contexts where it makes sense to enforce a specific declaration.