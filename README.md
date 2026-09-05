# Quantum Snake 2.0 (Raylib + C++)

## Overview
A complete Snake game built in C++ with the [raylib](https://www.raylib.com/) graphics library. It started from a raylib C++ starter template for VS Code and was built out into a full small game: a start menu with player-name entry, three selectable difficulty levels, sound effects, background music, a mute toggle, scoring with different food types, a persistent high score for the session, and a game-over screen with a restart option.

## Features
- Start menu with player name entry and an animated background
- Three difficulty levels (Easy / Medium / Insane) that change the snake's speed
- Three food types worth different point values, each with its own sprite
- Sound effects for eating food, losing, and beating the high score, plus background music
- A mute/unmute button
- Snake rendered with directional "eyes" that face the direction of movement
- Collision detection against the walls and the snake's own tail
- Game-over screen showing the player's name, final score, and high score, with a restart option

## Technologies Used
- **C++** (structs/classes, `std::deque` for the snake body)
- **[raylib](https://www.raylib.com/)** for windowing, 2D rendering, input, and audio (`raymath.h` for vector helpers)
- **Make** (GNU Makefile build, VS Code tasks/launch configuration included)

## How It Works
The game is organized around three small classes in `src/main.cpp`:
- `Button` — a clickable image-based UI button used for the menu, level selection, and the mute toggle.
- `Food` — picks a random on-grid position (avoiding the snake's body) and a random sprite/point value each time it spawns.
- `Snake` — stores its body as a `deque<Vector2>`, growing from the front and shrinking from the back to animate movement.

A `Game` class composes a `Snake` and `Food`, and owns the sound effects. The `main()` function drives a simple state machine (`gamestate`: menu → level select → playing → game over) inside the raylib game loop (`InitWindow` / `BeginDrawing` / `EndDrawing`).

## How to Run
This project depends on **raylib** being installed/available to the compiler and linker.

**Using the included Makefile (MinGW/GCC on Windows, or Linux/macOS with raylib installed):**
```bash
make
./quantum_snake   # or the binary name produced by the Makefile
```

**Using VS Code:** open the folder, make sure the raylib include/lib paths are configured for your machine, and press `F5` (this project was originally built from the "Raylib C++ Starter Template for VS Code").

Controls: `W A S D` to move, mouse to click menu buttons, `R` to restart after game over, `Esc` to exit.

## Project Structure
```
quantum-snake-raylib/
├── src/
│   ├── main.cpp          # all game logic (Button, Food, Snake, Game classes + main loop)
│   ├── *.png              # menu, background, food, and UI sprites
│   └── *.mp3               # sound effects and music
├── Makefile                # build configuration
└── preview.jpg              # template preview image
```

## What I Learned
- Structuring a small game with classes instead of one long procedural `main()`
- Using raylib for 2D rendering, texture loading, input handling, and audio playback
- Implementing a simple game-state machine (menu / levels / playing / game over)
- Using `std::deque` to efficiently grow and shrink a moving body of segments
- Collision detection (point-in-rectangle for UI buttons, edge and self-collision for gameplay)

## Known Limitations
- The high score is kept in memory for the current run only — it is not saved to disk, so it resets each time the game is closed and reopened.
- Requires raylib to be installed/available to the compiler; it is not bundled with this repository.

## Attribution
The initial project scaffolding (folder layout, `main.code-workspace`, base window setup) comes from a public raylib + VS Code starter template. All gameplay code — the menu system, levels, scoring, sounds, and the snake/food/game logic — was written on top of that template.
