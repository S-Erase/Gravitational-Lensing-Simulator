# Interactive Gravitational Lensing Simulator

## Table of Contents
* [Introduction](introduction)
* [Technologies](technologies)
* [Settings](settings)
* [Controls](controls)

## Introduction
There are many images and videos on the web that simulate the bending of light caused by a black hole.
This program was built to make that experience interactive. Allowing you to move freely around a
singularity as well as upload skybox nets to decide the environment for yourself.

## Technologies
This application uses wxWidgets 3.1.4 to handle the user interface, and OpenGL to render the environment.

## Settings

### Skybox
<img src="https://i.imgur.com/NDYE6cK.png" alt="drawing" width="450"/><img src="https://i.imgur.com/x3AwCG4.png" alt="drawing" width="450"/>

Upon opening the program, you will be met with the inside of a cube whose each face is distinctly coloured and labeled. Each of these six faces 
can replaced by a .jpg or .png image of the user's choosing by pressing the respective button under the 'Skybox' section. The entire skybox can 
also be replaced at once by loading a custom cube net.

### Gravity
<img src="https://i.imgur.com/iwsijgQ.png" alt="drawing" width="300"/><img src="https://i.imgur.com/dSboaqn.png" alt="drawing" width="300"/><img src="https://i.imgur.com/NDYE6cK.png" alt="drawing" width="300"/>

#### No Gravity
The "No Gravity" mode is the default setting and does not bend light in any way, so a reflective sphere of schwarzschild radius is rendered so 
that the camera's bearings are known.
The screen goes black when the camera goes inside this sphere.

#### Newtonian Gravity
The "Newtonian Gravity" mode bends light as if it were an object subject to the classical model of gravity.
The radius of a black hole's event horizon is the same under Newtonian Gravity as under General Relativity. The difference is that light rays 
outside the event horizon will always escape to infinity regardless of direction, unlike in General Relativity. This means that the event horizon 
is not directly visible and you must observe the distortion of light to infer the singularity's location.

#### General Relativity
The "General Relativity" mode treats light rays as null geodesics in Reissner–Nordström spacetime. The spacetime which models a single black hole inside 
otherwise flat space. It bends light according to the laws of General Relativity.

#### Reissner–Nordström spacetime
Reissner–Nordström spacetime is a generalised version of Schwarzchild spacetime which accounts for the electric charge of a black hole. Unlike in classical models of gravity, the electric charge and angular momentum of mass influences gravitational forces. Increasing the electric charge enough will cause the event horizon to vanish, causing a "naked singularity", currently thought to be impossible in nature.

## Controls

Drag the mouse across the screen to rotate the camera.
Use WASD to move the camera around.
