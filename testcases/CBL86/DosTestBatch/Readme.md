The Simula TestBatch used by Simula a.s. to verify the PC-Simula in the 80s.
`simtst00.sim` .. `simtst99.sim` are the 100 test programs; most of them check themselves and print `NO ERRORS FOUND` or `*** ERROR` / `*** NOTE`.

Originally downloaded from https://github.com/portablesimula/TestBatch

Eight of the tests need something besides their own source. These files were missing here and are taken from the Portable Simula S-PORT repository,
whose `SimulaTestBatch/src` holds the same 100 sources (byte identical) plus the data they read:

https://github.com/portablesimula/S-PORT/tree/master/SimulaTestBatch

| test | needs | origin |
|---|---|---|
| `simtst40` | external procedures `pa`, `pb` | `p40a.sim`, `p40b.sim`, `p40c.sim` |
| `simtst41` | external procedure `p41` | `p41.sim` |
| `simtst59` | external class `c59` | `c59.sim` |
| `simtst77` | external file `any77` | nonempty file, contents irrelevant |
| `simtst86` | SYSIN | `sysin86` |
| `simtst88` | SYSIN | `sysin88` |
| `simtst89` | SYSIN, external file `any8189` | `sysin89`, nonempty file |
| `simtst96` | external file `data96` | `data96` |
| `simtst80`, `simtst81` | external files `any8089`, `any8189` | nonempty files |

The `bat0.xnx` .. `bat9.xnx` scripts are the original run procedure of the batch (also from S-PORT)
and document how each test is compiled and run, which files are created before a test and which input is redirected, e.g.

```
simtst86 <sysin86
```

The files whose contents are irrelevant are created by those scripts as copies of the script itself and are therefore not kept here:

```
cp bat7.xnx any77 ; cp bat8.xnx any8089 ; cp bat8.xnx any8189
```

`simtst40` tests separate compilation of two procedures which call each other,
where `pb` is first compiled as a stub (`p40b.sim`) and then recompiled from `p40c.sim`.
Since the present compiler imports Simula source modules by their name, the two modules are provided 
as `pa.sim` (= `p40a.sim`) and `pb.sim` (= `p40c.sim`, the recompiled version).


`out100` is not an input, but the report file written by `simtst00`, which lists the long real values
whose `put` / `get` on a directfile might not reproduce exactly. The copy kept here is the one produced by the original implementation.


