# Utility

## Introduction

Utilities are the entities which performs various service operations.

Utility implementation are placed in [bacs::problem::utilities](../src/utilities) namespace.

## Buildable

Each utility is \ref buildable_page.
If utility does not need to be build (for example it is a script)
we can treat copy operation as build.

Configuration file is named *config.ini*.

### Target

Utility is a executable file.
`[build]` section may define `target` option that will be interpreted
as binary name. If no such option is present it will fallback to directory name
(where *config.ini* is located).

## Calling conventions

Implementation may use different wrapper to implement different calling conventions.
For instance binary executable may be wrapped for functional call.

`[utility]` section has `call` option that will be used to determine calling convention.

## Utilities list

*TODO*
