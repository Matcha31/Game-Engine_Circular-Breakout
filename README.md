# Circular Breakout — PA199 Project

This project is a circular variant of the classic *Breakout* game, developed as part of the **PA199 Computer Graphics** course.

The game takes place in a circular arena with curved paddles and a vertical wall of bricks.
The goal is to destroy all bricks by bouncing the ball using the paddles, without letting the ball escape the outer boundary.

---

## Gameplay Overview

* A circular wall of bricks stands at the center of the arena.
* Bricks are stacked vertically in columns.
* A column only opens once all bricks in that column are destroyed.
* The game ends when:

  * **All bricks are destroyed** → Win
  * **The ball reaches the outer boundary** → Game Over

The physics simulation handles curved collisions for both paddles and bricks.

---

## Controls

### Game control

* **Space** — Launch the ball
* **R** — Reset the game
* **P** — Pause / unpause the game
* **Left Shift** — Slow motion (hold or toggle, depending on configuration)

### Paddle control

* **Left Arrow** — Rotate paddles counterclockwise
* **Right Arrow** — Rotate paddles clockwise

### Camera control

* **1** — Perspective camera
* **2** — Top-down camera

---

## Visual & Gameplay Features

* Curved paddle collision with angle-dependent bounce
* Curved brick collision using annular sector geometry
* Brick collapse with falling animation
* Camera shake on brick impact (visual feedback)
* Optional slow-motion mode for debugging or gameplay control
* Multiple paddles placed evenly around the arena

---

## Build & Run

The project uses the provided PA199 framework and OpenGL setup.

Typical workflow (on Linux in terminal):

```bash
mkdir build
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
cd build/
ninja
./courses/PA199/project/PA199_project
```

---
