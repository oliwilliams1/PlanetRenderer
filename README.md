# PlanetRenderer
This repo is research for my upcoming scholarship project, intending get further understanding and experience in C++ and OpenGL.

The terrain for a planet is generated via dynamic mesh tesselated on the GPU via tesselation shaders, with a heightmap for each vertex to sample its height. Gpu tesselation is a solid choice for my upcoming project as it is extremely performant, but with the cost of a lot of bugs along the way.
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
[Perlin Noise](https://github.com/Reputeless/PerlinNoise) Simple library that allows me to generate noise without any hassle.<br>
All are correctly linked via cmake for both windows with vcpkg and linux with custom package managers.

**Plans**<br>
I plan for the future to have better tesslation for each use case, i.e. tesselation for approaching the planet is different to the tesselation when on the planet. The plan is to maximise how many fragments each face takes up whilst keeping maximum detail. I also need to find a solution for removing seams across patches in tesselation, as right now I find it too challenging.

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