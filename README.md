# B-Minor-Source-to-Source-Compiler
This repository contains the source code for a B-Minor compiler, as described by Professor Douglas Thain. 
The main change is that the compiler translates the source file into C++ code instead of assembly language code.
#Instructions
To make compiler: make bminor
To scan: bminor -scan source.bminor
To parse: bminor -parse source.bminor
To resolve declaration: bminor -resolve source.bminor
To typecheck: bminor -typecheck source.bminor
To generate CPP file: bminor -cpp source.bminor
