This is a copy of `testcases/CBL70/DEMOS` with a few changes which make it a Simula 1986 source, plus a main program per example.

Changes with respect to the CBL70 originals:

- `inf.close(notext)` / `outf.close(notext)` became `inf.close` / `outf.close` (4 occurrences in each of `demos.sim` and `demos2.sim`). 
  `close` of the 1986 standard environment (10.1.2) has no parameter; the text argument is the pre 1986 file interface.
- `demosex.sim`, which contains the 21 examples of the DEMOS manual as one source, is split into one program per example, `demoex00.sim` .. `demoex20.sim`.
  Each of them is `begin external class demos; ... end;` with the example unchanged, so every example is a program which can be compiled and run on its own.

Nothing else was changed; in particular the DEMOS class itself is the original source apart from the four `close` calls.

`demos2.sim` is the other DEMOS variant of the CBL70 directory (a different sequencing set implementation,
the median technique instead of the linked list with a `bl` field); it is not another language version. 
