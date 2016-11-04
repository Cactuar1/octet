LINK TO YOUTUBE VIDEO:
https://www.youtube.com/watch?v=NaGSVIhJRpQ&feature=youtu.be




Introduction to Programming -- Luke Sanderson

ASTEROIDS --	README
				


Original code by Andy Thomason, adapted into Atari's 'Asteroids' by Luke Sanderson

**RUNNING THE GAME**

To run the game, simply run :
octet/src/examples/example_ASTEROIDS/invaderers_app.h.

This is a recreation of the hit arcade game of 1979, Atari's Asteroids. 

**MAKING OF**

The project started as a simple version of Taito's Space Invaders, and has had changed code and art assets in order to become Asteroids. The player must control their ship through an Asteroid belt whilst evading and destroying them. The game uses screen-wrapping on both axes, so every object can pass through the sides and appear on the other. 

Much like the original file, the sprites of the asteroids and any objects that have more than one instance are initiliased through loops, for speed and readability. However, the randomizer function is used when initialising asteroids to ASDFASDFASDF.

In order to retain the screen wrapping effect from the original Asteroids, upon colliding with an (offscreen) border object, the player's rotation was stored, it's rotation set to 0, warped to the other side and finally reset to the previous player's rotation. This was done as moving the player by the translate function would have moved it by it's local axis, rather than the world's co-ordinates. While it worked, it was unwieldy and rather ugly to look at, so in the end I managed to move each wrappable object by it's modelToWorld. For example:

		sprites[ship_sprite].modelToWorld[3][0] = -6;

will set the ship's x co-ordinate to -6; This method's much faster as we aren't storing variables for every sprite.

The large asteroids were originally going to be created from values found in a csv file, however I opted instead to create them using a simple sin and cos function:

		float pointNum = (k*0.5f) / num_big_asteroids;
		float angle = pointNum*3.14159265;
		float x = sin(angle * 4) * 2.5f;
		float y = cos(angle * 4) * 2.5f;



**GAMEPLAY**

To start the game, simply press Space at the Title Screen.
The controls are as follows:

SPACE -- THRUST

	Press Space to move in the direction you're facing.

LEFT & RIGHT ARROWS -- TURN

	Turn the ship.

LEFT CTRL -- FIRE

	Press CTRL to fire. There can only be four missiles on screen at once.

SHIFT -- HYPERSPACE

	A unique feature from the original Asteroids game. Press Shift to temporarily disappear and reappear at a random part of the map.

**GAMEPLAY TIPS**

Since there can only be 4 missiles on screen at once, try to get as close to an asteroid as possible before firing to ensure you can shoot as many missiles as possible.	


Hyperspace can be used as many times as you like, so make sure to use it if you're in a hairy situation. Watch out - you could reappear inside an asteroid an die instantly.


Make use of the screen-wrapping effect to shoot faraway asteroids and escape.


There is a 1 in 20 chance of a UFO appearing once an asteroid is destroyed - listen out for the sound cue once it appears. Shooting it down nets you an extra 10 points. Your ship can't collide with it!

Shoot the big asteroids first, don't let too many small asteroids on screen at one time!

**THANKS**

Thanks to Matthew Duddington for the help with the texture_shader file.

**COPYRIGHT**

Atari 1979, Asteroids, video game, Arcade, Atari, US. 
