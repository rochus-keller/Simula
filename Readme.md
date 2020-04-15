## About this project

The goal of this project is to build a Simula 67 parser, code model, LuaJIT bytecode compiler and an IDE with integrated source-level debugger (similar to https://github.com/rochus-keller/Oberon).

Simula 67 was the first object-oriented programming language and the pioneer for many concepts that were adopted years later in Smalltalk, C++ and Java - just to name a few. So to say it is the mother of all object-oriented programming languages existing today. 
The original SIMULA 67 COMMON BASE DEFINITION appeared in June 1967. See here for more information: http://www.simula67.info/

Just as C++ is based on C, Simula is an extension of Algol 60. Accordingly I have extended my Algol 60 parser (see https://github.com/rochus-keller/Algol60) with the syntax elements from the Common Base Language report (1970 edition, see http://simula67.at.ifi.uio.no/Standard-70/Simula-1970-NR-22.pdf). 

The lexer/parser works and successfully parses e.g. the DEMOS package (see http://staffwww.dcs.shef.ac.uk/people/G.Birtwistle/simulation.html).

This project is work in progress.


### Binary versions

Not yet available.

### Build Steps

Follow these steps if you want to build the application yourself:

1. Make sure a Qt 5.x (libraries and headers) version compatible with your C++ compiler is installed on your system.
1. Download the source code from https://github.com/rochus-keller/Simula/archive/master.zip and unpack it.
1. Goto the unpacked directory and execute `QTDIR/bin/qmake SimLjEditor.pro` (see the Qt documentation concerning QTDIR).
1. Run make; after a couple of seconds you will find the executable in the build directory.

Alternatively you can open SimLjEditor.pro using QtCreator and build it there.

## Support
If you need support or would like to post issues or feature requests please use the Github issue list at https://github.com/rochus-keller/Simula/issues or send an email to the author.



 
