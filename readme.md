PSEUTHE
=======

Pseuthe (pronounced 'soothe') is an abstract audio / visual experience,
with casual gameplay elements.

Pseuthe started when I was experimenting with simulating Newton's laws of
motion (and, to a lesser extent, Hooke's laws of springs) but has since
evolved - first into something more graphical, closely followed by the
addition of gameplay. You take on the role of a proterozoic plankton with
the sole aim of survival - absorb and consume the healthy microbes whilst
avoiding those detrimental to your health.

A note on controls:
By default controls are set to 'arcade' mode - that is, the player is
steered much like a car in a 2D racing game. If you prefer the challenge
of using slightly more accurate physics, select 'Classic' mode on the main
menu.

Pseuthe is best played with a controller.


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