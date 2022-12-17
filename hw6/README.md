# HW6: Lighting and textures

Julia Troni 
CSCI 4229 Graphics
Hw 6: Lighting and Textures


## Compile and execute
```
$ make
$ ./hw6.exe
Then the window opens up and shows the 3D scene with textured trees and houses, a grassly surroundings. Includes first person navigation, orthagonal, and perspective view, in addition to user controlled lighting
```

## Commands to interact with the program: 
*  0      Reset view angle to 0,0,0 and mode to orthagonal projection (mode 0)
 *  1      Switch to perspective projection (mode 1)
 *  2       Switch to first person view (mode 0)
 *  m changes the mode from orthagonal (0) to perspective (1) to first person (3) projection
 * * In all modes (0,1,2):
 *     +/-   Change field of view for perspective
 *     - x/X looks down the x axis
 *     - y/Y looks down the y axis
 *     - z/Z looks down the z axis
 *     - Left/right arrows rotate about the y axis
 *    -  Up/down arrows rotate about the x axis
 * 
 * In first person (2) mode only:
 *     - f moves forward
 *     - b moves backward

* 'L' or 'l' toggles whether or not light is enabled
* 'T' or 't' toggles the motion of the light
* '<' and '>' allow the user to move the light source if the toggle is off
* '[' decreases light height
* ']' increases light height
* R  : raises the distance of light
* r : lowers the distance of light
* 'a' decreases ambience
* 'A' increases ambience
* 'd' decreases diffuse level
* 'D' increases diffuse level
* 's' decreases specular level
* 'S' increases specular level
* 'e' decreases emission level
* 'E' increases emission level

 * ESC closes the window and terminates the program


- This is an iteration of my hw5
- As usual borrows heavily from in class examples and especially ex16
- This assignment has so far taken me 33 hours 