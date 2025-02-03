
# Requirements: Render colored spheres via ray casting

# Bonus: render axis aligned cylindars or parrellel projection

## Input file description:

### view origin:

		**eye** <x> <y> <z>
		The point where the camera will be located in the scene
### view direction:

		**viewdir** <x> <y> <z>
		The tip of a vector rooted at the view origin indicating where the camera is pointing

### up direction:

		**updir** <x> <y> <z>
		The tip of a vector rooted at the view origin indicating the vertical angle of the camera

### vertical field of view:

		**vfov** <angle>
		The size of the vertical field of view in degrees

### image size:

		**imsize** <width> <height>
		The size of the image in pixels

### background color:

		**bkgcolor** <r> <g> <b>
		The rgb color of the background, normalized from 0-1

### material color:

		**mtlcolor** <r> <g> <b>
		The rgb color of all following objects until overriden by a new material color, normalized from 0-1

### sphere object:

		**sphere** <x> <y> <z> <r>
		Definition of a sphere object where the first 3 arguments are the center point and the final is the radius

## optional

### parellel projection:

		**parellel** <height>
		Creates image as a parellel projection with a frustrum height of <height>

### cylinder object:

		**cylinder** <x1> <y1> <z1> <x2> <y2> <z2> <r> <l>
		Definition of a cylinder object with center at (x1,y2,z3), a direction vector of {x2, y2, z2}, a radius of r and a length of l.
