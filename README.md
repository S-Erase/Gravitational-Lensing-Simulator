# Gravitational-Lensing-Simulator

## Table of Contents
* [Introduction](introduction)
* [Technologies](technologies)
* [Settings](settings)
* [Controls](controls)

## Introduction
This is an interactive application which allows you to 
move freely around a gravitational singularity in an 
environment of your choosing and experience the gravitation 
lensing caused by it.

## Technologies
This application uses wxWidgets to handle the user interface, and OpenGL to render the environment.

## Settings

### Skybox
<img src="https://i.imgur.com/NDYE6cK.png" alt="drawing" width="450"/><img src="https://i.imgur.com/x3AwCG4.png" alt="drawing" width="450"/>

Each of the six faces can replaced by a .jpg/.png image of the user's choosing by pressing the respective button under the 'Skybox' section. The entire skybox can also be replaced at once by loading a custom cube net.

### Gravity
<img src="https://i.imgur.com/iwsijgQ.png" alt="drawing" width="300"/><img src="https://i.imgur.com/dSboaqn.png" alt="drawing" width="300"/><img src="https://i.imgur.com/NDYE6cK.png" alt="drawing" width="300"/>

#### No Gravity
The "No Gravity" mode does not bend light in any way, so a reflective sphere of schwarzschild radius is rendered so that the camera's bearings are known.
The screen goes black when the camera goes inside the sphere.

#### Newtonian Gravity
The "Newtonian Gravity" mode bends light as if it were an object subject to the classical model of gravity.
The radius of a black hole's event horizon is the same in the Newtonian model as in General Relativity. The difference is that light rays outside the event horizon will always escape to infinity, unlike in General Relativity. This means that the event horizon is not directly visible and you must look for the centre of the distortion of light to find the singularity.

#### General Relativity
The "General Relativity" mode bends light according to the schwarzschild spacetime. The spacetime which models a single black hole inside otherwise flat space.

## Controls

Drag the mouse across the screen to rotate the camera.
Use WASD to move the camera around.
