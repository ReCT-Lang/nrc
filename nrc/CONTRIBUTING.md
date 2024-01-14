# How you can contribute
Glad you're willing to contribute to the project! It's people like you that make the OSS world go round.

Now, to keep everything somewhat maintainable there are a couple guidelines.

## Pull Request Etiquette
When opening pull requests, we expect a couple of things from you:
- A clear and descriptive title
- A finished or almost finished PR. Early drafts will be closed(exceptions may be made).
- Descriptive commit messages - This is generally fairly tough to keep up, so we give a lot of leeway.
- Links to any issues this aims to resolve - You can do `#1234`, where `1234` is the issue ID you're resolving.
- Describe what you aim to do - We want to know ahead of time what we're reviewing and merging, since it makes our life
    a lot easier.
- Proper structure - Binaries, object files and such things should never be part of the source tree. Make sure
    you have a proper gitignore and didn't skip anything.

## Code Guidelines
We don't have too strict rules on code formatting. There are a couple of things though.
- Same-line curly brackets - We keep our starting brackets on the same line. `like_this {`
- We use `snake_case` formatting where applicable.
- Struct names are "clean" - No `pParameterName` or `m_myProperty`. Pure old `some_variable`.
- Try to document where needed. What's obvious to you is unreadable to someone else.
- Indentation is done via tabs - Users can change the tab width themselves.
- Clear naming where possible. "Imitation functions" may have different schemes(e.g. `palloc` imitating `malloc`)
- Bad code or temporary solutions should use `// TODO: xxxx` to tell what needs to be redone.
- Comments should always be single-line - No `/* My comment */`. It's always `// My comment`.
- Keep to the horizontal 80-char limit. It's easier to work with.

If unsure of something, just look at the existing code and judge yourself.

## New features
As we stray to provide solid standardization for the project(i.e, ReCT and SSRM lang/VM specs), new features take a
bit of work to implement and thus won't be merged very quickly. If possible, please write a spec for the feature, to
make our life easier.