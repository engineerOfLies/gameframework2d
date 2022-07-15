# gameframework2d (GF2D) Master Branch
This branch holds all of the most up-to-date, but non game specific code.
This is a collection of utlitity functions designed to facilitate creating 2D games with SDL2
While the main branch project is specifically intended to function as an educational tool for my students taking 2D Game Programming, this one is specifically more robust and complicated.

Currently the project is in a WIP state, however it is very functional

# Features

## Graphics
Initialization of SDL and setup of a hardware accelerated 2D rendering context

### Draw
Support for basic geometric shape drawing

### Sprite
A resource manager and support for loading and displaying 2D images as well as simple sprite sheet based animations

### Actor
Building on Sprite, this keeps track of named actions and timing of animations.  These configurations can be loaded from json files

### Camera
Rudimentary 2D camera functions

### particles
lightweight single pixels, shapes, or sprites for special effects.

### mouse
Basic management of the mouse.  Uses an actor to draw it (file specified when initialized)

### lighting
A lighting / shadow mask.  Can have lights rendered to it to mask out where light and shadow is in a game world.

## Armature
Uses 2D bones and forward kinematics to animate a skeleton.  Uses Actor code to keep track of named animations, but these animations  work with poses of the armature instead of frames of a sprite sheet

### Figure
Working with Armatures, these provide links between specific bones and sprites to create animated 2D figures.  Figure Instances are loaded from common figures to allow for ad-hoc changes to the figures (such as loading different weapons into the hands of a figure)

## Font
Building on SDL_tff this library allows for the loading of fonts and ease of rendering single lines or even word wrapped blocks of text

## Collisions
A simple 2D collision system.  Good for fast testing of a space for collisions

### space
Manages the space for collision and can be configured for more or less accuracy
Has a 2D spacial hash to speed up collision tests

### bodies
Keep track of space objects.  Each body can be configured to be represented by different shapes.  Currently only supporting circles, rects, and edges.

### dynamic bodies
Used internally by spaces when running an update.

## window system
A faily robust window system driven by json config is provided.  Sample window configs can be found in menus/
Code integration can be seen in action with the gf2d_message_buffer and gf2d_windows_common.

### Elements
Generic container class for window components (Widgets) including:
 - Labels - text rendered with fonts
 - Actors - support simple flat images, sprites, and figures
 - Button - uses labels and or actors to represent the button.  Supports hotkey, focus, and mouse input
 - entry - basic text entry component.  Still needs some work to better support numbers and copy/paste
 - list - container type that can support lists laid out horizontally or vertically.


# Build Process

Before you can build the example code we are providing for you, you will need to obtain the libraries required
by the source code
 - SDL2
 - SDL2_image
 - SDL2_mixer
 - SDL2_ttf
There are additional sub modules that are needed for this project to work as well, but they can be pulled right from within the project.
Performable from the following steps from the root of the cloned git repository within a terminal. 

Make sure you fetch submodules: `git submodule update --init --recursive`
Go into each submodule's src directory and type:
`make`
`make static`

Once each submodule has been made you can go into the base project src folder anre simply type:
`make`

You should now have a `gf2d` binary within the root of your git repository. Executing this will start your game.
