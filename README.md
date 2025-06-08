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

Furthermore, you have the choice to use either the Terminal implementation
or the SDL implementation of this engine. To choose between the two, you need to
define one of these macros before including `terdimension.h`.
- `TD_TERMINAL` : Renders inside the terminal using characters simulating pixels.
	- By default, TerDimension will render in **4-Bit Color** in Terminal implementation.
	To enable **Full Color (RGB)**, you will need to define `TD_COLOR_RGB`.
	- Using 4-Bit Color, you can edit `td_color.h` to change the color palette to fit your
	current terminal, or disable bright colors if your terminal doesn't support them.
- `TD_SDL` : Renders inside a SDL window. You need to link SDL to your executable for
this implementation to work. Check out `CMakeLists.txt` and the CMake docs.

For the vertex winding order, you can set `TD_winding` to `TD_CW` for clockwise
winding or `TD_CCW` for counter-clockwise.

# Limitations and specifications
This library supports only triangles, so you will need to triangulate your meshes before
importing them. This 3D engine also uses the Z+ axis as the front axis. The camera is
`TD_camera` and is in reality a `TD_Transform`. This engine also supports different aspect
ratios and scales the output accordingly. Screen dimensions cannot be odd numbers, due to the fact
that each character on screen represents 2 pixels on top of each other.

**Mesh structure** : A mesh must contain vertices (position vertices) and faces (all triangles).
Colors, uvs and normals are optional, but if they are not existant, please set them to NULL.
If you choose to use colors and/or uvs, there must be 1 color/uv coordinate per vertex, with the same index
as the vertex it is paired to. Normals, on the other hand, can be ordered any way, and there can only be 1
normal per face.
