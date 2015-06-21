# Buildable

## Introduction

For convenient standard interface for everything that may require building
such interface is introduced. Buildable specification defines
only [Configuration file](#configuration-file) format and general concepts.

## Definition
**TODO**

## Specifications

### Configuration file

Configuration file has [INI format](ini.md).
`[build]` section is used for [Buildable](buildable.md) options specifications.
User may introduce arbitrary sections except `[build]` for additional
configuration (not used by [Buildable](buildable.md)).

`[build]` section contains `builder` option
that specifies one of supported implementation supported builders.
Implementation may provide default `builder` value.

#### Examples

##### Specifies cmake builder

```ini
[build]
builder = cmake
```

##### Specifies single-source builder

```ini
[build]
builder = single
source = main.cpp
```

##### Specifies cmake builder with non-standard config location

```ini
[build]
builder = cmake
source = src/CMakeLists.txt
```
