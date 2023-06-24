# Developer's Guide to Broma
Thank you for your interest in contributing to Broma! This guide aims to introduce you to the codebase quickly; use the codebase
documentation in the sidebar if you'd like to learn more about specific classes or functions.

## Requirements
Broma requires:
- **A C++17 compliant compiler.** Any recent version of Clang, GCC, or Microsoft Visual C++ should work.
- **CMake.** Broma uses CMake as its build system.

## Building
Make sure you have the required dependencies, then run the following commands:
```sh
# Configure in `build/` directory
cmake -B build
# Build Broma
cmake --build build
```

You should now have `Broma-test` and the Broma static library in your build directory.

## Code layout
Below is the rough hierarchy of files in Broma:
- `broma.cpp`: Entrypoint and root grammar
    - `state.hpp`: AST (abstract syntax tree) state to be used during parsing and errors encountered
    - `basic_components.hpp`: Assorted basic grammar rules (keywords, whitespace, comments, etc.)
    - `class.hpp`: Top-level grammar for classes
        - `attribute.hpp`: C++-like attributes (ex. `[[docs(...)]]`)
        - `member.hpp`: Member variables and padding
            - `type.hpp`: Utility grammars for parsing C++ types
            - `bind.hpp`: Grammar for specifying padding amount (ex. `PAD = 0x4`)
        - `function.hpp`: Member functions
            - `type.hpp`: Utility grammars for parsing function prototypes
            - `bind.hpp`: Grammar for specifying offsets of functions (ex. `void func() = win 0x100`)
    - `function.hpp`: Top-level grammar for free functions
    - `post_process.hpp`: Post-processing of parsed AST (currently just sorting parsed classes by dependencies)

You'll notice that some headers appear multiple times in this layout; this is because some grammars share the
same description and therefore are used in different places.

## PEGTL
### A quick introduction
Broma uses the [PEGTL](https://github.com/taocpp/PEGTL/) parser generator to describe its grammar.
PEGTL is a very powerful template library, but it has very verbose documentation, so we've provided a short introduction in the context of Broma.

You can think of PEGTL in terms of [BNF](https://en.wikipedia.org/wiki/Backus%E2%80%93Naur_form) (Backus-Naur form) or a more advanced form of regex. After all, that's basically what it is in formal logic!
For example, given the following PEGTL grammar:
```cpp
struct my_grammar : seq<'a', 'b', star<any>> {};
```
This roughly corresponds to `ab.*` in regex, assuming `.` matches *any* character.

We can take a look at the `function_proto` grammar in `function.hpp` and try to translate it to [EBNF](https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form):
```cpp
struct function_proto :
        seq<rule_begin<function_proto>,
            opt<attribute>,
            sep,
            tagged_rule<function_proto, type>,
            sep,
            tagged_rule<function_proto, identifier>,
            arg_list>
        {};
```
This would roughly correspond to:
```ebnf
function_proto ::= [ <attribute> ] <sep> <type> <sep> <identifier> <arg_list>
```

Here's another example with `sor` from `field` in `class.hpp`:
```cpp
struct field : sor<inline_expr, pad_expr, member_expr, bind_expr, ool_expr> {};
```
This would simply correspond to:
```enbf
field ::= <inline_expr> | <pad_expr> | <member_expr> | <bind_expr> | <ool_expr>
```

You can find the rule reference for PEGTL [here](https://github.com/taocpp/PEGTL/blob/main/doc/Rule-Reference.md), and the (very verbose) user guide [here](https://github.com/taocpp/PEGTL/blob/main/doc/README.md).

### How Broma uses PEGTL
Broma uses a couple custom utility structs and conventions, derived from PEGTL.

#### `run_action`
These struct overloads allow Broma to modify AST state when the parser has reached a certain point in the grammar.

#### `tagged_rule`
While this grammar initially appears to be a simple alias to `seq<...>`, it serves a very important purpose:
it allows portions of grammar to be captured so that `run_action` overloads can be easily assigned to them.
This makes it much easier to write actions to mutate the AST based on where the parser has reached.

#### `named_rule`
This serves almost an identical purpose to `tagged_rule`, except that it makes the code a bit more readable,
by providing a description to the section of the grammar it is applied to.

#### `rule_begin`
This grammar is also purely for readability, since it simply evaluates to `success`.
Make sure the template parameter in this grammar is the same as the one begin currently defined:
```cpp
struct my_rule : seq<rule_begin<my_rule>, sor<subrule1, subrule2>>

struct run_action<rule_begin<my_rule>> {
	template <typename T>
	static void apply(/* ... */) {
		// Reset the WIP structure for this AST type, etc...
	}
}
```
Otherwise, this may lead to confusion to other contributors.
