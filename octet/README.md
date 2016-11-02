Introduction to Programming -- Luke Sanderson

ASTEROIDS --	README

Original code by Andy Thomason, adapted into Atari's 'Asteroids' by Luke Sanderson

**RUNNING THE GAME**


To run the game, simply run :
example_ASTEROIDS/invaderers_app.h.

This is a recreation of the hit arcade game of 1979, Atari's Asteroids. 

**MAKING OF**

The project started as a simple version of Taito's Space Invaders, and has had changed code and art assets in order to become Asteroids.

Much like the original file, the sprites of the asteroids and any objects that have more than one instance are initiliased through loops, for speed and readability.

In order to retain the screen wrapping effect from the original Asteroids, upon colliding with an (offscreen) border object, the player's rotation was stored, it's rotation set to 0, warped to the other side and finally reset to the previous player's rotation. This was done as moving the player by the translate function would have moved it by it's local axis, rather than the world's co-ordinates. While it worked, it was unwieldy and rather ugly to look at, so in the end I managed to move each wrappable object by it's modelToWorld. For example:

		sprites[ship_sprite].modelToWorld[3][0] = -6;

would have set the ship's x co-ordinate to -6;


**GAMEPLAY**

To start the game, simply press Space at the Title Screen.
The controls are as follows:

SPACE -- THRUST
	Press Space to move in the direction you're facing.

LEFT CTRL -- FIRE
	Press CTRL to fire. There can only be four missiles on screen at once.

SHIFT -- HYPERSPACE
	A unique feature from the original Asteroids game. Press Space to temporarily disappear and reappear at a random part of the map.

**TIPS**
Since there can only be 4 missiles on screen at once, try to get as close to an asteroid as possible before firing to ensure you can shoot as many missiles as possible.
Hyperspace can be used as many times as you like, so make sure to use it if you're in a hairy situation. Watch out - you could reappear inside an asteroid an die instantly.
Make use of the screen-wrapping effect to shoot faraway asteroids and escape.
There is a 1 in 20 chance of a UFO appearing once an asteroid is destroyed - listen out for the sound cue once it appears. Shooting it down nets you an extra 10 points. Your ship can't collide with it!

**COPYRIGHT**

Atari 1979, Asteroids, video game, Arcade, Atari, US. 