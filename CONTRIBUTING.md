# How to contribute

I'm really glad you're reading this, because we need volunteer developers to help this project stay alive.

You can conribute to Ipponboard in lots of different ways:

- Report bugs and discuss issues
- Test the program
- Contribute code
- Specify requiremens (e.g. new or different rule sets)
- Submit club logos and information 

I want you to be able contribute your ideas and input that you're exited about. So, don't hesitate and get in contact.

## Building Ipponboard

Please refer to the [HOW_TO_BUILD](HOW_TO_BUILD.md) document to get you started.

## Submitting changes

- Please use [pull requests](http://help.github.com/pull-requests/) with a clear list of what you've done. 
- Please follow our coding conventions (below)
- Make sure all of your commits are atomic (one feature per commit).
- Always write a clear log message for your commits. One-line messages are fine for small changes, but bigger changes should look like this:

      $ git commit -m "A brief summary of the commit
      > 
      > A paragraph describing what changed and its impact."

## Coding conventions

Start reading our code and you'll get the hang of it. We optimize for readability:

> _This is open source software. Consider the people who will read your code, and make it look nice for them. It's sort of like driving a car: Perhaps you love doing donuts when you're alone, but with passengers the goal is to make the ride as smooth as possible._

### Naming guidelines

The code base leans on Qt's conventions with a few project-specific tweaks:

- **Types** (`class`, `struct`, `enum class`, `namespace`): PascalCase (`TournamentModel`, `ScaledImage`).
- **Public interfaces** (member/static/free functions visible to other translation units): PascalCase (`StartTimer()`, `CreateModel()`).
- **Private/Protected helpers**: lowerCamelCase (`startTimerInternal()`).
- **Member data**: prefix private members with `m_` (`m_scoreBoard`). Public or struct-like members should stay rare; when needed use lowerCamelCase and document mutability.
- **Constants**: prefer `constexpr`/`const` named in PascalCase (`DefaultFightTime`). `enum class` values stay PascalCase.
- **Macros/defines**: SCREAMING_SNAKE_CASE and kept as a last resort.

Document new patterns when they emerge so contributors stay aligned.

### Formatting

- Run `clang-format` (LLVM 14+) with the repository's `.clang-format` on every C++ header/implementation you touch before committing.
- Scope formatting to the lines you change to avoid massive churn in legacy files (`clang-format -i file.cpp` works fine after staging).
- The config derives from Qt-style expectations (tabs for indentation, braces on their own line) and keeps includes unsorted by default to respect existing grouping.
- For bulk reformatting discuss first; prefer incremental cleanup when altering code paths.

Thanks! ❤️ ❤️ ❤️<br>
Florian Mücke
