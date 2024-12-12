# PlanetRenderer
This repo is research for my upcoming scholarship project, intending get further understanding and experience in C++ and OpenGL.

The terrain for a planet is generated via dynamic mesh tesselated on the GPU via tesselation shaders, with a heightmap generated at runtime with compute shaders for each vertex of the planet to sample its height.
### Low lod, near planet yet not close enough for little details to matter
![no subdivision](images/no%20subdivision.png)
### Smooth transition to high lod via tesselation shaders
![medium lod](images/med%20lod.png)
### Max lod, enough verts to remain performant while keep all the detail
![on surface](images/on%20surface.png)

**Features**<br>
As of now, the program features the most simple terrain generation, manipulation of position, rotation, and scale of the planet, and my personal favourite: hot reload for shaders, allowing you to edit shaders and see the result in the cick of a button without running the program from scratch again.

**Libraries**<br>
[GLFW](https://github.com/glfw/glfw) Used for window management across platforms, and linking an OpenGL context to it.<br>
[GLEW](https://github.com/nigels-com/glew) A binding library used to access opengl functions.<br>
[glm](https://github.com/icaven/glm) OpenGl mathematics library, used for complex vector and matrix mathematics and easy-to-implement with OpenGL.<br>
OpenGL is the base framework that allows me to do basically everything I do on the GPU.<br>
[ImGui](https://github.com/ocornut/imgui) An immediate-mode gui which is for immediate debugging.<br>
All are correctly linked via cmake for both windows with vcpkg and linux with custom package managers.
## Building and running
### Windows
On windows, my CMake file has been tested to work with MSVC and [VCPKG](https://github.com/microsoft/vcpkg) (a package manager for windows), installation should be as easy as cloning the repo via MSVC, installing vcpkg to path, and running directly in the MSVC ide, CMake should handle the harder things.

### Linux
On linux, you need to install the required libraries via your distros package manager. Here is an example: 
```bash 
sudo pacman -S glfw glew glm imgui
```
After installing packages, you can clone the repo via
```bash
git clone https://github.com/oliwilliams1/PlanetRenderer
```
Cd into the cloned folder, make a build directory and run cmake
```bash
cd PlanetRenderer/
mkdir build
cd build/
cmake ..
```
Build the project and run the exectuable
```bash
make && ./PlanetRenderer
```