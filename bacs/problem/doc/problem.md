# Problem

## Problem

BACS support different problem formats.
Each problem is a directory tree.

### Problem format
In the root directory "format" file is provided.
It's trimmed content is a problem format.

*It is not recommended to use problem formats with whitespaces.*

### Problem type
The first part of problem format, separated by '#' if present, whole format string otherwise,
is problem type. [bacs::problem::importer](../include/bacs/problem/importer.hpp)
implements particular problem type conversion.
