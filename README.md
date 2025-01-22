# PlanetRenderer
This repo is research for my upcoming scholarship project, intending get further understanding and experience in C++ and OpenGL.

Time poured into this project

[![wakatime](https://wakatime.com/badge/user/c2914490-a1ff-469e-9751-c6f7f416b31c/project/9affd918-ef81-4f90-a4c2-6aed6d2c2766.svg)](https://wakatime.com/badge/user/c2914490-a1ff-469e-9751-c6f7f416b31c/project/9affd918-ef81-4f90-a4c2-6aed6d2c2766)

## Features & screenshots
![atmosphere](images/atmosphere.png)
^ Above is an overview of the features I have, a planet with ~50k trees rendered in real time, volumetric atmospheric scattering (colours to be added), deffered rendering pipline for a solution to expensive overdraw, realtime planet generation (planet generation is procedural and happens via compute shaders for all components). Many settings making the planet dynamic. An asset manager for ease of adding meshes to the project.
<br><br><br>
Below shows the dyamic lod of the planet, tesselated based on distance on the gpu makes this extremely performant and low cost (frame time lost to planet generation is negligible)
<div style="display: flex; justify-content: space-between;">
    <div style="margin-right: 10px;">
        <img src="images/low-lod.png" alt="Low LOD" style="max-width: 100%; height: auto;">
    </div>
    <div>
        <img src="images/high-lod.png" alt="High LOD" style="max-width: 100%; height: auto;">
    </div>
</div>

<br><br>

<div style="display: flex; justify-content: space-between;">
    <div style="width: 40%; margin-right: 10px;">
        Rendering trees are a challenging task as they have lots of vertices & alpha textures, but my implementation allows me to render ~100k trees before noticing any dip in performance. This is because all trees are just an image, which is switched out based on your viewing angle to the billboard. The only name I've found for this is Simplygon's flipbook billboard imposters, and getting them to work was hell.
    </div>
    <div style="width: 60%;">
        <img src="images/imposters.png" alt="atmosphere" style="max-width: 100%; height: auto;">
    </div>
</div>

<br><br>

<div style="display: flex; justify-content: space-between;">
    <div style="width: 60%; margin-right: 10px;">
        <img src="images/tree-lod.png" alt="atmosphere" style="max-width: 100%; height: auto;">
    </div>
    <div style="width: 40%;">
        Trees when close enough, swtich out with their full mesh version, and is indirectly drawn and managed all of the gpu via compute shaders and atomic counters. (Found a way to not use atomic counters but havn't implemented yet).
    </div>
</div>

<br><br>

<div style="display: flex; justify-content: space-between;">
    <div style="width: 40%; margin-right: 10px;">
        For flipbook imposters, I've found it hard to do in blender as there has not been many implementations on it, so I made my own interface to create these "flipbook imposters" textures.
    </div>
    <div style="width: 60%;">
        <img src="images/billboard-gen.png" alt="atmosphere" style="max-width: 100%; height: auto;">
    </div>
</div>





## Libraries
GLFW, GLEW, glm, ImGui, Assimp, and stb_image