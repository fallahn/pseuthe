PSEUTHE
=======

Pseuthe (pronounced 'soothe') is an abstract audio / visual experience.
Sounds and patterns are created in a pseudo-random manner with the intent
to aid relaxation, study or to create an ambient atmosphere.

Pseuthe started when I was experimenting with simulating Newton's laws of
motion (and, to a lesser extent, Hooke's laws of springs) but has since
evolved. Other than the ambient experience Psuethe serves no real purpose.

Building
--------

Pseuthe depends on SFML, specifically the Window, Graphics, System and Audio
libraries (and sfml-main on Windows.) The repository contains a Visual Studio
solution and a set of pre-built binaries for compiling on Windows. Linux
users will have to install SFML manually, but there is a CMake file included
which can be used to create a makefile compatible with GCC 4.9+ and the latest
versions of clang/llvm. Pseuthe should also build on OSX in theory, but is
untested (pull requests for OSX support are welcome!)


License
-------

Pseuthe is released under the permissive zlib license.