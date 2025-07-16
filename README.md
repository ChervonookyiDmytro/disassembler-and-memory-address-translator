# Virtual Memory Address Translator & Disassembler (C++)

This project implements a disassembler and virtual-to-physical address translation tool based on page table data.

## Overview

The system reads binary instructions and a virtual memory page table, and produces:
- Disassembled assembly-like instructions from bytecode.
- Address translation: replaces virtual addresses with physical ones.
- Error diagnostics when pages are missing or not present in memory.
- Automated testing with input/output validation.

## Features

- Page table loaded from CSV file.
- Conversion of virtual addresses to physical (if page is present).
- Disassembler supports a custom instruction set with:
  - MOV, CMP, SUB, MUL
  - Register-to-register and register-to-address formats
- Two modes: run from hardcoded hex strings or from file input.
- Testing module that compares actual vs. expected output.
