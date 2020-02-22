# Level Streaming Project

<img align="right" src="https://i.imgur.com/6Spws9u.png" width="30%">

This project is a demo implementation of a grid-based level streaming system, complete with a custom archive format for models and textures, as well as a level editor and content management tool.

The system is implemented in DirectX11 and is fully self contained. Import some models (they must be triangulated, have DDS textures, and have material groups) and create some levels!

Levels are automatically segmented into a grid (of user-specified size). These levels are loaded at different levels of detail, depending on the player camera's position.

## Editing levels

To create a level, launch `AssetManager`. Here you can instance a new level and add models to it. Adding models will pull associated textures also, and package everything into a custom binary format - styled after Creative Assembly's Cathode BIN/PAK format.

Once you've created a level and added assets to it, you can launch `LevelStreamingEditor` to build the level and add NPCS, triggers, and set the spawn location.

Following that, `LevelStreamingPlayer` can be used to walk through the level with streaming enabled.

## Ideas for expansion

The project was created in just a few weeks, so a number of elements have been implemented but not expanded upon.

For example, it would be nice to have...

* Logic tied to the trigger volumes (can easily be added!)
* Variable levels of recursion into neighbours for loading high levels of detail (e.g. the current tile and its neighbour is high LOD, the rest are low)
* More complex NPC pathing logic, or perhaps jobs

... the list goes on!