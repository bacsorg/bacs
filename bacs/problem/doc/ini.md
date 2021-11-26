# INI format

## Introduction

This format is useful for simple configuration files,
albeit it is deprecated.

There are a lot of different implementations.
Different parsers introduce inconsistent features,
so it is not possible to determine what INI format is
only by it's name.

To make things convenient specifications are provided.

## Definition

INI file is *plain text* file encoded in *utf8*.

INI file is divided into sections.
Each section has unique name.
Sections order is not preserved.

Each section contains several options.
Each option has unique name in the section scope.
Options order is not preserved.

```ebnf
INI file = {section} ;
section = section header, section contents ;
section header = {space symbol}, "[", section header name, "]", {space symbol}, eoln ;
space symbol = ? space symbol excluding line separators ? ;
eoln = "\n" | "\r" | "\r\n" ;
section header name = identifier ;
section contents = {section line} ;
section line = blank line | comment line | option line ;
blank line = {space symbol}, eoln ;
comment line = (";" | "#"), {-eoln}, eoln;
option line = option name, {space symbol}, "=", {space symbol}, option value, {space symbol}, eoln ;
option name = identifier ;
identifier = id symbol, {id symbol};
id symbol = "_" | "a" | ... | "z" | "A" | ... | "Z" | "0" | ... | "9" ;
```

## Examples

### Example 1

```ini
[section1]
; comment
option1 = value1
```

### Example 2

```ini
[section2]
x = 3
y = some string
```
