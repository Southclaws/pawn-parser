# Pawn Lexer/Parser

Derived from the Golang scanner/token packages and modified for Pawn code.

The first commit to this repository includes the original, official Go 1.8.1 `scanner` and `token` packages so you can compare every diversion from the original commit-by-commit!

## Roadmap

The first stage is getting a substantial set of sample files of *valid* and *invalid* Pawn source code. This will be collected in the repository and tests will be written for all the samples. Then the codebase will be moulded to fit these tests and hopefully I won't forget about anything!

I'm pretty confident Scavenge and Survive has a substantial example of every Pawn language construct so I will initially use that as a test bed since it's also modular and includes a decent amount of good and bad code practices. In the future, if more people start to use this I'll have a better understanding of what I've missed and what this project should aim for.

You'll notice that if you run `go run main.go` the sample code looks pretty good except for:

```
00 - ILLEGAL    "#"
00 - IDENT      "include"
00 - <  ""
00 - IDENT      "YSI"
00 - ILLEGAL    "\\"
00 - IDENT      "y_hooks"
00 - >  ""
```

That's because Go does not have any concept of preprocessor directives - the things in C/++/Pawn that start with a `#` character. This shouldn't be too hard and I plan to also process include trees and dependencies (woo!) so you should be able to know the exact location of a function from it's call points and build a graph of dependencies for a set of modular scripts.

Once that's done, I will build an AST package that will transform the tokenised form into an AST for walking and performing static analysis.

Hopefully this will help with such things as:

- did you forget to declare a variable?
- these function arguments are wrong
- these tags don't match
- this loop has the potential to run forever
- this code is unreachable
- this variable could be declared in a more optimal location
- etc...

The end goal of this project is to eventually build some form of static analysis tool (similar to the Go toolchain) and eventually, a cool VS Code plugin for Pawn projects!

I've never worked with code lexing/parsing or ASTs before so this will be a learning experience! Part of the reason I'm doing it on Pawn is because it's not a very well known language so it will be challenging. There are resources for lexing/parsing C/++ code but Pawn has some unique features that I'd like to cover.

## Macros

I don't initially plan on supporting macros - it sounds complicated and I don't like complicated things (even though I'm writing a code parser :neutral_face:)

Maybe in the future but right now I'm focusing on scope, functions and variables.

## YSI

Yes YSI gets its own section in the readme because it's so damn complex! (Thanks @Y-Less!) I actually plan to write YSI syntax directly into the parser because that would be much easier than processing the macro spaghetti on which YSI is built on! This means keywords such as `hook`, `task`/`ptask`, `foreach`, etc will be processed as individual tokens.
