# PA199 - project (game engine and breakout game)

An implementation of a small game engine and
a breakout game using the engine.

## NOTES FOR DEVELOPERS

This file is located in the root folder under which
you are supposed to implement your entire project.
The content of the folder (and sub-folders) is completely
under your control. But in CMakeLists.txt files there are
few lines which should not be modified. They are clearly
identified by comments.

The initial project implementation (once you unzip the
project's template) provides you a quick tutorial for
adding more files and libraries to the project. The
tutorial consists of the file:

    ./some_game_file.cpp 
    
and a library:

    some_my_library
    
in the sub-folder ./some_my_library_dir. In the
./CMakeLists.txt file you can see, how a new module
can be added to the project.

We also provide tutorial for making unit tests of
library functions. They are in a tutorial file:

    ./test/some_test.cpp

We also included a tutorial for loading shaders
and textures from the disk. It is in file:

    ./application.cpp

All tutorial files and libraries are completelly
artificial. So feel free to remove them any time
you want.

Once you project in ready for submition to IS, then
build the targets:

    PA199_project.exe (Install)
    PA199_project_tests.exe (Install)

There will be created a "dist" folder right under
the project's root folder containing all data and
binaries to run the project.

## GAME CONTROLS

| Key         | Description |
| :---        | :----       |
| Left Arrow  | Rotate paddles in CW direction. |
| Right Arrow | Rotate paddles in CCW direction. |
| Space       | Launch the ball at the game begin. |
| P           | Toggle game pause. |
| 1           | Switch to perspective camera (look from side). |
| 2           | Switch to orthographics camera (look from top down). |
