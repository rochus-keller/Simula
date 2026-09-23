
`test.sim` is the single test program of the GNU cim distribution (cim 5.1,
`test/test.sim`, RCS id `test.sim,v 1.2 1994/07/17`), copied here unchanged.

    Copyright (C) 1994 Sverre Hvammen Johansen, Stein Krogdahl and Terje Mjoes
    Department of Informatics, University of Oslo.
    GNU General Public License, version 2 (see the cim distribution for the full text of the license).

cim uses it as its installation test:
the program is a compiler and interpreter for the small language Minila (lexical analysis, symbol table,
code generation into an integer/real store,
and an interpreter of the generated code),
and it runs a Minila program that prints the primes below 40.
The program checks the result itself, so the whole test collapses into one line of output.

## Files

- `test.sim` — the original cim source, unmodified.
- `test-simc.sim` — adapted copy for simc, see below.

## Expected output

    Installation: No errors found

## Changes

The original program ends with cim's non-standard `return(n)` extension, which sets the exit code of the program:

    ... Outtext("Installation: No errors found"); outimage; return(0);

`return` is not part of the Simula standard and is intentionally not implemented by simc, so `test.sim` is rejected with

    validator: "test.sim" 278 89 "declaration for 'return' not found"
    validator: "test.sim" 280 58 "declaration for 'return' not found"

`test-simc.sim` is identical to `test.sim` apart from the removal of these two calls plus a comment;
it is therefore not authoritative cim source.

