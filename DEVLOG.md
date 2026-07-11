# Devlog — Circular Breakout Project (PA199)

Development ran roughly from late September to mid-December, with additional polishing and optional features implemented in early January.
This devlog was reconstructed from development notes taken during the project and refined afterward into a coherent timeline.

This devlog was reconstructed and edited at the end of the project from development notes taken during the semester. An AI-based writing assistant was used to help reorganize and clarify these notes into a coherent narrative, without adding features or technical work that were not implemented in the project.

---

## Week 1 (Sep 20–25) — Project setup & math groundwork

I began by setting up the provided framework and exploring the starter `Application` code. The first goal was to understand how rendering, shaders, and event callbacks were structured.

I then implemented the math library required by the assignment:

* `Vec4` (vector arithmetic, dot product, cross product, normalization)
* `Mat4` (identity, multiplication, transpose, vector transform)

I wrote unit tests for both classes using GoogleTest. These tests caught several early mistakes, especially related to column-major indexing and normalization edge cases. Having a reliable math layer proved essential later when implementing transformations, physics, and camera logic.

At this stage, the project was mostly non-visual, but it established a solid foundation.

---

## Week 2 (Sep 27–Oct 2) — Matrix transforms & camera

I extended the math library with transformation utilities (translation, rotation, scaling) and started building a simple camera abstraction.

Initially, I struggled with column-major vs row-major conventions, which caused inverted rotations and incorrect object placement. After re-reading the seminar slides and aligning everything with OpenGL conventions, I fixed the issue by enforcing consistency across all math operations.

I added small tests for transformation composition to validate correctness.

---

## Week 3 (Oct 4–9) — Rendering primitives & axes

I refactored `Application.cpp` to reduce its size and moved rendering-related code into a `graphics/` folder.

I implemented:

* a mesh abstraction
* procedural generation of rings and ring segments
* axis rendering for debugging (X/Y/Z colored axes)

At this point, I could render a ground disk, axes, and simple ring shapes. This helped visualize the coordinate system and catch spatial mistakes early.

---

## Week 4 (Oct 11–16) — Bricks & paddles (first attempt)

I implemented the circular brick wall and paddles using ring segments.

---

## Week 5 (Oct 18–23) — Scene scaling & structure cleanup

I introduced a `SceneScale` structure to centralize all gameplay dimensions (brick radii, paddle radii, ball radius, arena size, etc.).

This was critical because early values were inconsistent:

* the ball was too large
* the ground was too small
* the wall spacing felt wrong

After tuning these values, the scene visually matched the reference images from the assignment much more closely.

---

## Week 6 (Oct 25–30) — Game state & input

I created a `GameState` structure to separate gameplay logic from rendering.

It stores:

* paddle angle and angular velocity
* ball position and speed
* game mode (waiting, playing, win, game over)

Initially, I updated the ball in polar coordinates (`r`, `θ`), which resulted in spiral or elliptical motion. This was a design mistake that became increasingly problematic.

---

## Week 7 (Nov 1–6) — Physics refactor (major redesign)

I realized that physics must be handled in **Cartesian space**, not polar.

I refactored ball movement to use:

* `(x, y)` position
* `(vx, vy)` velocity
* constant speed normalization

Polar coordinates were kept only for rendering and broad-phase checks.
This completely fixed the “weird ellipse” motion and made collision response far more robust.

---

## Week 8 (Nov 8–13) — Paddle collisions (curved geometry)

I implemented paddle collision using an annular sector model:

* inner arc
* outer arc
* two radial edges

Early versions had major issues:

* only one paddle reacted
* the ball sometimes passed through
* paddle curvature barely affected bounce direction

Fixes included:

* true closest-point computation on the annular sector
* checking all paddles (3 paddles equally spaced)
* sub-stepping to prevent tunneling
* enforcing a minimum separating velocity after collision

This was one of the hardest parts of the project.

---

## Week 9 (Nov 15–20) — Brick collisions & gameplay rules

Brick collisions were implemented using the same curved annular sector approach.

---

## Week 10 (Nov 22–27) — Brick collapse & animation

I implemented proper brick collapse:

* each column behaves as a vertical stack
* removing one brick shifts all above bricks downward

At first, entire columns disappeared due to repeated collision resolution in a single frame. This was fixed by:

* limiting brick removal to once per physics step
* adding a small hit cooldown

For visual feedback, I added a simple falling animation using:

* `row_visual`
* `row_target`
* linear interpolation per frame

This made the wall behavior match the assignment description.

---

## Week 11 (Nov 29–Dec 4) — Gameplay tuning

I focused on playability:

* added aiming influence based on where the ball hits the paddle
* introduced a small tangential bias so the ball doesn’t stay in the same column
* prevented the ball from gliding or sticking to paddles

These changes made the game much more controllable and enjoyable.

---

## Week 12 (Dec 6–11) — Win/lose states & cleanup

I finalized:

* game over when the ball reaches the outer limit
* win condition when no bricks remain
* reset and relaunch logic
* code cleanup and file organization

At this point, all **mandatory features** from the assignment were implemented.

---

## Week 13 (Dec 13–15) — Final testing & fixes

I tested edge cases:

* shallow paddle hits
* high ball speeds
* brick edge collisions

I fixed remaining bugs related to collision cooldowns and penetration resolution.

---

## January polish & optional features (Jan 2–6)

During final polish, I implemented two optional gameplay enhancements:

### Camera shake on brick impact

A small camera shake effect is triggered when a brick is destroyed.
This required no changes to gameplay logic and provided immediate visual feedback, making impacts feel more satisfying.

### Gameplay slow-down toggle

I added a simple slow-motion toggle by scaling the simulation time step.
This feature is useful both for debugging and for demonstrating control over the physics update loop.

I also performed final cleanup:

* removed dead code
* simplified comments
* re-verified behavior against the assignment requirements
