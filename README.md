# Game lore
**Roaring Dungeon** (RD) - is a roguelike game that takes place deep underground. One day you have suddenly fallen in a hole while on a family forest trip. You were wounded but the old man saved you and taught the basics of magic. With fire wand and green robe you are ready to escape this place. But who knows what dangers await you?
# Tech review
The game developed in C++ using GLFW window manager, Glad loader, GLM math library and OpenGL Core 3.3. Stb_image implementation is used for reading texture files.

**Project takes use of** object-oriented programming(OOP), pointers and references, C++ vectors, GLSL shaders, reusable functions and more. Thus making it:
* Lightweight: all project files besides .git directory is a little over 2MB.
* Low-level: no game engines nor pre-written game libraries. 
* RAM and CPU efficient. Most of the heavy variables are declared once and accessed by pointers if possible.


**GLM mathematics library** is used for matrix operations, projection matrix creation, all in-game transforms (scale, moving, rotating).

**flex_shader class** has been created for GLSL shader compiling and setting GLSL unifroms. Error checks automated, setting uniforms has became less complicated. Simple and solid.

**npc_handle class** has been designed for creating NPCs, player and other game objects and defining interactions between each other.
# How to compile
g++ main.cpp glad.c -o roaring_dungeon -lglfw3 -lGL -lX11 -ldl -lpthread
