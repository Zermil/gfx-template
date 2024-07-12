# gfx-template

This is a simple template to get started with rendering on Windows OS. This project **IS NOT** a complete engine or application, but just a lot of boilerplate code to **START** programming an actual application or an engine. Mostly for personal use, but gatekeeping knowledge and learning materials is cringe.

Most, if not all, of the code follows the same principles described in [This youtube series](https://www.youtube.com/playlist?list=PLT6InxK-XQvNKhnBT_nYydBfR9xpfV0XY). Huge thanks to [Allen Webster](https://twitter.com/AllenWebster4th) and [Ryan Fleury](https://twitter.com/ryanjfleury) for showing people that programming can be (and is) very enjoyable.

## Code explanation

`base` - Helpers and useful 'standard library' functions.  
`gfx` - Deals with opening a window on a specific OS.  
`os` - Deals with OS specific stuff; win32, linux etc.  
`render` - Deals with backend API specific stuff; d3d11, opengl etc.  
`font` - Different font providers, font rasterization and rendering.  

If someone is taken a little aback by the use of linked list, there's [this great article](https://www.rfleury.com/p/in-defense-of-linked-lists) by [Ryan Fleury](https://twitter.com/ryanjfleury). Short version is that they work very well with arenas and as free-list for quick allocations.

## build

Make sure to change `MSVC_PATH` in the build script.

```console
> build
```
