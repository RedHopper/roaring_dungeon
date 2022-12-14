# Game lore
**Roaring Dungeon** (RD) - is a roguelike game that takes place deep underground. One day you have suddenly fallen into a hole while on a family trip to forest. You were wounded but the old man saved you and taught the basics of magic. With fire wand and green robe you are ready to escape this place. But who knows what monsters will get in your way?
# Tech review
The game is developed in C++ using GLFW window manager, Glad loader, GLM math library and OpenGL Core 3.3. Stb_image implementation is used for reading texture files.

**Project takes use of** object-oriented programming(OOP), pointers and references, C++ vectors, GLSL shaders, reusable functions and more. Thus making it:
* Lightweight: all project files besides .git directory are a little over 2MB.
* Low-level: no game engines nor pre-written game libraries are being used. 
* RAM and CPU efficient. Most of the heavy variables are declared once and accessed by pointers if possible.


**GLM mathematics library** is used for matrix operations, projection matrix creation, all in-game transformations (scaling, moving, rotating).

**flex_shader class** has been created with the purpose of automating GLSL shader compilation routines and accessing GLSL unifroms.

**npc_handle class** has been designed for creating NPCs, player and other game objects and defining interactions between each other. Child function *draw* controlls all NPCs behaviour, and also creates *render_object* and pushes it to the render vector (whch is then passed to the *render_handle* function, where it's sorted and eventually rendered).

Only single VAO (Vertex Array Object) is used for all game's graphics.

Game score is recorded and compared with the score in *high_score.txt* file on player's death. If new high score has been set, it shows corresponding message in game window and overwrites file's value.
# Gameplay review
Game's main goal is to fight different monsters appearing on your way out. For each defeted monster you gain coins. Also don't forget to loot chests! Once you are done fighting the portal opens and you can move on to the next level. All monsters follow these rules:
* **Slime** - moves in one direction. Avoids other slimes and bounds. Exchanges it's position with bats and chests. Can be killed by moving in front of it. Destroys chest if boosted.
* **Bat** - moves diagonally. Unlike slime can share it's position with the chest. Though bats are no friend to you, it won't attack if there are other free positions. If no free cell is present it dies.

* **Boost** will move any creature to the cell its facing. Boosted creature has advantage over cell's host. It's direction is changed clockwise when encountered. Avoids facing boundaries.
# How to compile
g++ main.cpp other_libs/glad.c -o roaring_dungeon -lglfw3 -lGL -lX11 -ldl -lpthread
