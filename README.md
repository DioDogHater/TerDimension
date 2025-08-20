# TerDimension
A in-terminal 3D software rendering library.\
The name is "Third Dimension", but with "Ter" for "Terminal".

# Requirements
- Linux (Windows terminals suck)
- CMake
- SDL (optional)\
**That's it.**

# How to build
```
git clone https://github.com/DioDogHater/TerDimension
cd TerDimension
mkdir ./build
cmake -S . -B ./build
cmake --build ./build
```
Compiled program is `./build/main`

# Initializing the TerDimension library
First, you will need to include the `TD/terdimension.h` header to
use the functions and global variables the library offers. The first
thing to do is to initialize the TerDimension library to render stuff.
`TD_init(...)` is a function that takes 2 arguments, the width and height
of the "screen". The vertical resolution is actually halved since a character
takes a 1x2 "pixels". It shall return false if the library fails to initialize.
Don't forget to quit the library using `TD_quit()` when the time comes.

```C
#include "TD/terdimension.h"

int main(int argc, char* argv[]){
  // Create a 200 x 120 pixel screen
  if(! TD_init(200, 120) )
    return 1;

  TD_quit();
  return 0;
}
```

# Hard-coding / importing a 3D model
There is two ways to get a valid mesh: \
initializing it by hand like in `example_meshes.h`,
or importing them using `TD/td_obj.h`.\
For hard coded meshes, we only need to import the textures needed for the meshes.
You can also use colors which is very useful for simple meshes.
`TD_load_texture(...)` takes the path to the texture and the pointer to the TD_Texture we will use
as arguments, returns false when something goes wrong. `TD_free_texture(...)` will free that texture.\
`TD_load_obj(...)` takes two arguments, the first one being the path to the
model we want to import, the second being the pointer to the TD_Mesh we want to use. It returns
false if something goes wrong. We will of course eventually need to free the mesh using `TD_free_obj(...)`.

```C
TD_Mesh triangle = {
  // vertex positions
  (TD_Vec3[]){ 
    {0,0.5,0},
    {-0.5,-0.5,0},
    {0.5,-0.5,0}
  },
  // vertex colors
  (TD_Color[]){ TD_RED, TD_GREEN, TD_BLUE },
  // vertex normals (none)
  NULL,
  // no texture
  TD_TextureEMPTY,
  // faces
  // {positions, uvs / colors, normals}
  (TD_Face[]){ {1,2,3, 0,0,0, 0,0,0} },
  // number of faces
  1,
  // TD_Transform (position, rotation, scale)
  (TD_Transform){
    (TD_Vec3){2,0,3},
    TD_Vec3ZERO,
    TD_Vec3IDENTITY
  }
};

TD_Mesh monke = {.transform = (TD_Transform){
  (TD_Vec3){0,0,5},
  TD_Vec3ZERO,
  TD_Vec3IDENTITY
}};

int main(...){
  ...
  if(! TD_load_obj("assets/monke_model/monke.obj", &monke))

  ...

  TD_free_obj(&monke);
  ...
}
```

# Rendering 3D meshes
Firstly, we need a fragment shader. This is two basic shaders that display the
color / texture of the mesh using the TD_ShaderInfo struct.
```C
TD_Color color_shader(TD_ShaderInfo* si){ return si->color; }
TD_Color texture_shader(TD_ShaderInfo* si){ return TD_sample_texture(si->uv.x, si->uv.y, si->texture); }
```
Now, as you can see, the TD_ShaderInfo struct already contains the interpolated
vertex attributes, such as the positions, colors and uvs. By using `TD_sample_texture()`
we get the color of the pixel in the texture with UV coordinates we already calculated.
Now, we need to render the meshes on screen using these shaders.
```C
...

int main(...){
  ...
  // Makes sure the terminal is cleared
  TD_clear_screen();

  // Setup the buffers
  TD_clear_buffers();

  TD_use_shader(color_shader);
  TD_render_mesh(&triangle);

  TD_use_shader(texture_shader);
  TD_render_mesh(&monke);

  // Show the monkey and the triangle
  TD_update_screen();
  ...
}
```
There should be the monkey and the triangle on your screen, in full color!
To change other rendering modes (`TD_RENDER_RGB`, `TD_RENDER_COLOR`, `TD_RENDER_NO_COLOR` combined optionally with `TD_RENDER_UNICODE`),
use the `TD_set_render_flags(...)` macro.

# How to export your meshes correctly
Before import models into your application / game, you will need to:
- Triangulate all faces of your model
- Calculate all normals correctly
- Bake all textures / materials into one diffuse texture
- Use a material that will use that texture as the model's color
- Export your model in the Wavefront (.obj) format, using the Z+ and Y+ axis as the forward and up axis.
- Make sure the exported .mtl (Material library) contains a line similar to this: `map_Kd texture.png`
- If your mesh seems inverted / weirdly shaded, try changing the triangle winding by setting `TD_winding` to either `TD_CW` or `TD_CCW`
