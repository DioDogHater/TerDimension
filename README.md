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

# Options and tweaks
You can change some things about this library as you please.
You can overwrite these macros before including `terdimension.h` to change how this library manages
its memory management and logging.
- `TD_MALLOC(sz)` : Allocates `(sz)` bytes in memory. Default is `malloc` from `stdlib.h`.
- `TD_FREE(ptr)` : Frees memory pointed to by `(ptr)`. Default is `free` from `stdlib.h`.
- `TD_MEMSET(ptr,c,sz)` : Sets `(sz)` bytes to char `(c)` in memory pointed to by `(ptr`.
Default is `memset` from `string.h`
- `TD_LOG(fmt,...)` : Logs error / information, formatted. Default is `printf` from `stdio.h`.

Furthermore, you can change the way the scene is rendered by setting "Render flags".
These flags determine what techniques the renderer will use. You can combine these
flags to customize fully how the scene is rendered. Look at `td_definitions.h` for
more information.

For the vertex winding order, you can set `TD_winding` to `TD_CW` for clockwise
winding or `TD_CCW` for counter-clockwise.

# Limitations and specifications
This library supports only triangles, so you will need to triangulate your meshes before
importing them. This 3D engine also uses the Z+ axis as the front axis. The camera is a global variable
called `TD_camera` in `terdimension.h`. This engine also supports different aspect
ratios and scales the output accordingly. Screen dimensions cannot be odd numbers, due to the fact
that each character on screen represents 2 pixels on top of each other.

**Mesh structure** : A mesh must contain vertices (position vertices) and faces (all triangles).
Colors, uvs and normals are optional, but if they are not existant, please set them to NULL.
If you choose to use colors and/or uvs, there must be 1 color/uv coordinate per vertex, with the same index
as the vertex it is paired to. Normals, on the other hand, can be ordered any way, and there can only be 1
normal per face.
