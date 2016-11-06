#Tools & Middleware -- Luke Sanderson

#Bridge --	README


LINK TO YOUTUBE VIDEO:
https://www.youtube.com/watch?v=spdqSMng4Lg&feature=youtu.be

![ScreenShot](https://raw.github.com/LukeSanderson18/octet/tree/SpaceInvadersClone/octet/ToolsAndMiddleware_Pictures/1.png)
![alt tag](https://github.com/LukeSanderson18/octet/tree/SpaceInvadersClone/octet/ToolsAndMiddleware_Pictures/1.png)
![alt tag](https://github.com/LukeSanderson18/octet/tree/SpaceInvadersClone/octet/ToolsAndMiddleware_Pictures/2.png)
![alt tag](https://github.com/LukeSanderson18/octet/tree/SpaceInvadersClone/octet/ToolsAndMiddleware_Pictures/3.png)
![alt tag](https://github.com/LukeSanderson18/octet/tree/SpaceInvadersClone/octet/ToolsAndMiddleware_Pictures/4.png)

This is a simple physics demo of a bridge using spring constraints. 
The ends of the bridge are movable, showing that the bridge's layout can be changed in runtime.

Original code by Andy Thomason, adapted by Luke Sanderson

#To move the ends of the bridge, use the WASD and arrow keys for the left and right ends respectively.
#Press Space to create a new sphere.
#Press Shift to move the bridge ends faster.

To run the game, simply run :
octet/src/examples/example_shapes/example_shapes.h

#Bullet Physics

Bullet2D is a physics engine which contains a number of parameters to emulate various physics effects such as gravity, collisions and spring joints.
The example_shapes file relies heavily on the visual_scene for processes like adding shapes and creating the default camera and lighting setup.
However, I've added a new function that can be called from inside example_shapes.h.
shapeSpring() takes in 4 constraints - the first two are two planks' rigidbodies, and the last two are their respective transforms.
By using a btGeneric6DofSpringConstraint, I created a constraint that could be utilised in constraining all 6 axes of a rigidbody.

While I experimented with constraints such as the spring's stiffness, damping and linear upper & lower limits, I instead chose to only limit the
springs by their angular upper and lower limits.
In my original code, each plank had two spring joints connecting the the next plank, rather than the current one connection. In the end, I decided
that this was a bit overkill, considering the project is meant to be rather simple, so I reduced the number of connections to one and simply set
an angular constraint on each spring. It was more efficient and gave very similar results.

In my example_shapes.h code, I do a simple loop getting every plank but the first and last one, as they're the green ends, and assign a material and
rigidbody to them.
A second function, createSpring(), assigns the rigidbodies and transforms for each plank.

By pressing Space, the player can create a sphere that can roll down the planks. By moving the bridge end, you can create ripples and move the spheres
along the bridge.

**KNOWN BUGS**

Stretching the two ends too far apart can stretch each spring constraint, causing the planks to madly flail.

**THANKS**
Thanks to Robert Doig for exaplaining spring joints and the like.

