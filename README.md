# Game lore
**Roaring Dungeon** (RD) - is a roguelike game that takes place deep underground. One day you have suddenly fallen in a hole while on a family forest trip. You were wounded but the old man saved you and taught the basics of magic. With fire wand and green robe you are ready to escape this place. But who knows what dangers await you?
# Tech review
The game is developed in C++ using GLFW window manager, Glad loader, GLM math library and OpenGL Core 3.3. Stb_image implementation is used for reading texture files.

**Project takes use of** object-oriented programming(OOP), pointers and references, C++ vectors, GLSL shaders, reusable functions and more. Thus making it:
* Lightweight: all project files besides .git directory is a little over 2MB.
* Low-level: no game engines nor pre-written game libraries are being used. 
* RAM and CPU efficient. Most of the heavy variables are declared once and accessed by pointers if possible.


**GLM mathematics library** is used for matrix operations, projection matrix creation, all in-game transformations (scaling, moving, rotating).

**flex_shader class** has been created with the purpose of automating GLSL shader compiling routines and setting of GLSL unifroms.

**npc_handle class** has been designed for creating NPCs, player and other game objects and defining interactions between each other. Child function *draw* controlls all NPC behaviours, and also creates *render_object* and pushes it to the render vector (whch is then passed to the *render_handle* function, where it's sorted and eventually rendered).

Only single VAO (Vertex Array Object) is used for all game's graphics.

Game score is recorded and compared with the score in *high_score.txt* file on player's death. If new high score has been set, shows corresponding message in game window and overwrites file's value.
# Gameplay review
Your main goal is to fight with different monsters appearing on your way out. For each defeted monster you gain coins. Also don't forget to check chests! Once you've done fighting the portal opens and you can move to the next level. All monsters follow these rules:
* **Slime** - moves in one direction. Avoids other slimes and bounds. Exchanges it's position with bats and chests. Can be killed by moving right in it's face. Destroys chest if boosted.
* **Bat** - moves diagonally. Unlike slime can share it's position with the chest. Though bats are no friend to you, it won't attack if there are other free positions. If no free cell is present - dies.

**Boost** will move any entity to the cell its facing. Boosted entity has advantage over cell's host. It's direction is changed clockwise when encountered. Avoids facing boundaries.
# How to compile
g++ main.cpp glad.c -o roaring_dungeon -lglfw3 -lGL -lX11 -ldl -lpthread