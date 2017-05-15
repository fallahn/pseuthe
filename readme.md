PSEUTHE
=======

Pseuthe (pronounced 'soothe') is an abstract audio / visual experience,
with casual game play elements.


[![Watch on YouTube](http://djfallen.com/images/pseuthe.gif)](https://youtu.be/EOqYMuoxA50)


Pseuthe started when I was experimenting with simulating Newton's laws of
motion (and, to a lesser extent, Hooke's laws of springs) but has since
evolved - first into something more graphical, closely followed by the
addition of game play. You take on the role of a proterozoic plankton with
the sole aim of survival - absorb and consume the healthy microbes whilst
avoiding those detrimental to your health. If you don't eat you will slowly
starve to death, and colliding with obstacles will also reduce your health.
The game ends once you fade out of existence.

There are four kinds of microbes to look out for. The most common are
either green or red. Green microbes give you health when eaten, red will
take some away. Red microbes are sick and dying, and will fade away slowly.
There are also blue 'viral' microbes which are less common, but will give
you a significant health boost. Very rarely you will also see something 
akin to a jelly fish. These fade rapidly, but eating one gives you full
health, so eat it quick!
    Microbes don't like to group. Striking red plankton to help them die
sooner will promote the arrival of new microbes - but don't accidentally
swallow anything nasty...


#### Controls

The player can be controlled with either the keyboard or a controller.
Pseuthe is tested mainly with an xbox 360 controller, but others should
work fine too. There are two sets of controls available; Arcade - which
is enabled by default, and Classic - which can be enabled via the check
box on the main menu.

In Arcade mode left and right rotates the player's head, while forward 
and backward accelerate and decelerate in the direction the player is 
facing. This is similar to many top down racing games - the main 
difference being that there is no friction applied to the player. Once
you accelerate up to speed you no longer need to keep accelerating. 
Light adjustments using the accelerate and decelerate keys is the 
recommended approach. In Arcade mode the controls are mapped like so:

###### Accelerate:
W or Up Arrow on the keyboard, or Button 0 or z-axis negative on a
controller. This is the equivalent to the A button and right trigger on an
xbox 360 controller.

###### Decelerate:
S or Down Arrow on the keyboard, or Button 1 or z-axis positive on a
controller. This is equivalent to the B button or left trigger on an xbox
360 controller.

###### Turning:
Steering is mapped to the A and D keys or Left Arrow and Right Arrow on the
keyboard. On a controller you can use the analogue stick or the D-Pad to
steer.


Classic mode allows you to control the player in a more realistic, 
physically based sense, but can be much harder to grasp. It is named 
Classic mode because it was the initial control scheme when I started the 
project. Using the WASD or Arrow keys on the keyboard or the analogue 
stick on the controller, you apply a literal force in the direction of the
control. Pressing up will push the player toward the top of the screen, 
down moves towards the bottom, and so on. If you have used RCS systems in 
games such as Kerbal Space Program then the concept should be familiar.
Unfortunately keyboard controls in this mode are limited to eight degrees 
of movement and so classic mode works much better with an analogue controller.
The Classic control scheme has been included for those who are looking for
an increased challenge from the game.


#### Scoring

Points are awarded based on how long you survive. The more body parts you 
have for a longer duration of time, the more points you will score. This
often results in a floating point score, which may appear unusual, but 
works just as well as more traditional scoring systems. Names are generated
on a pseudo-random basis, although they will include a capital A or capital
C depending on whether that score was achieved using Arcade or Classic 
control scheme.


Building
--------

Pseuthe depends on SFML 2.2 or higher, specifically the Window, Graphics,
System and Audio libraries (and sfml-main on Windows.) The repository 
contains a Visual Studio solution and a set of pre-built binaries for 
compiling on Windows. Linux users will have to install SFML manually, but
there is a CMake file included which can be used to create a makefile 
compatible with GCC 4.9+ and the latest versions of clang/llvm. There is
also an xcode project for those looking to build Pseuthe on OS X.

Binaries for Windows / Mac / Linux can be found on [gamejolt](http://gamejolt.com/games/pseuthe/256275)


License
-------

Pseuthe is released under the permissive zlib license, as found in the
source code.
