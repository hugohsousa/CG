# CG

Simple OpenGL based engine and model generator.

## Generator
The generator can be used to generate .3d files with the models information.

Graphic premitives avaiable:
* Plane (a square in the XZ plane, centered at the origin, subdivided in both X and Z directions)
* Box (requires dimension and the number of divisions per edge, centered in the origin)
* Sphere (requires radius, slices and stacks, centered at the origin)
* Cone (requires bottom radius, height, slices and stacks, the bottom of the cone should be
on the XZ plane)
* Bézier patches. (requires the name of a file where the Bézier control points are defined and the required tessellation level)

## Engine
The configuration of the scene can be changed by editing the `engine/config.xml` file.

Place the .3d model files in the `.3d` folder.

Place the textures in the `engine/textures` folder.

### Note: All the features are only available in the last stage of the applications `Entrega4`

  
