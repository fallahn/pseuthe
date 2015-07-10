PSEUTHE
=======

Pseuthe (pronounced 'soothe') is an abstract audio / visual experience,
with casual game play elements.

Pseuthe started when I was experimenting with simulating Newton's laws of
motion (and, to a lesser extent, Hooke's laws of springs) but has since
evolved - first into something more graphical, closely followed by the
addition of game play. You take on the role of a proterozoic plankton with
the sole aim of survival - absorb and consume the healthy microbes whilst
avoiding those detrimental to your health.

There are four kinds of microbes to look out for. The most common are
either green or red. Green microbes give you health when eaten, red will
take some away. Red microbes are sick and dying, and will fade away quite
quickly. There are also blue 'viral' microbes which are less common, but
will give you a significant health boost. Very rarely you will also see
something akin to a jelly fish. These fade rapidly, but eating one gives
you a full set of health, so eat it quick!


####Controls

The player can be controlled with either the keyboard or a controller.
Pseuthe is tested mainly with an xbox 360 controller, but others should
work fine too. There are two sets of controls available; Arcade - which
are enabled by default, and Classic - which can be enabled via the check
box on the main menu.

In Arcade mode left and right rotates the player's head left and right,
while forward and backward accelerate and decelerate. This is similar to
many top down racing games - the main difference being that there is no
friction applied to the player. Once you accelerate up to speed you no
longer need to keep accelerating. Light adjustments using the accelerate
and decelerate keys is the recommended approach. In Arcade mode the 
controls are mapped like so:

######Accelerate:
W or Up Arrow on the keyboard, or Button 0 or z-axis negative on a
controller. This is the equivalent to the A button and right trigger on an
xbox 360 controller.

######Decelerate:
S or Down Arrow on the keyboard, or Button 1 or z-axis positive on a
controller. This is equivalent to the B button or left trigger on an xbox
360 controller.

######Turning:
Steering is mapped to the A and D keys or Left Arrow and Right Arrow on the
keyboard. On a controller you can use the analogue stick or the D-Pad to
steer.


Classic mode allows you to control the player in a more realistic, 
physically based sense, but can be much harder to grasp. It is named 
Classic mode because it was the initial control scheme when I started the 
project. Using the WASD or Arrow keys on the keyboard, or the analogue 
stick on the controller you apply a literal force in the direction of the
control. Pressing up will push the player toward the top of the screen, 
down moves towards the bottom, and so on. If you have used RCS systems in 
games such as Kerbal Space Program then the concept should be more familiar.
Unfortunately keyboard controls in this mode are limited to eight degrees 
of movement and so classic mode works much better on an analogue controller.
The Classic control scheme has been included purely for reference, or for 
those who are looking for an increased challenge in the game.


####Scoring

Points are awarded based on how long you survive. The more body parts you 
have for a longer duration of time, the more points you will score. This
often results in a floating point score, which may appear unusual, but 
works just as well as more traditional scoring systems. Names are generated
on a pseudo-random basis, although they will include a capital A or capital
C depending on whether that score was achieved using Arcade or Classic 
control scheme.


Building
--------

Pseuthe depends on SFML, specifically the Window, Graphics, System and 
Audio libraries (and sfml-main on Windows.) The repository contains a 
Visual Studio solution and a set of pre-built binaries for compiling on 
Windows. Linux users will have to install SFML manually, but there is a 
CMake file included which can be used to create a makefile compatible with
GCC 4.9+ and the latest versions of clang/llvm. Pseuthe should also build 
on OSX in theory, but is untested (pull requests for OSX support are 
welcome!)


License
-------

Pseuthe is released under the permissive zlib license.