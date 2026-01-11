# Devlog — Circular Breakout Project (PA199)

Development ran roughly from late September to mid-December, with additional polishing in early January.

---

## Week 1 (Sep 20–25) — Project setup & math groundwork

I started by setting up the provided framework and exploring the starter `Application` code. At first, I focused on understanding how rendering, shaders, and the event callbacks were structured.

I then implemented the math library required by the assignment:

* `Vec4` (vectors, dot, cross, normalization)
* `Mat4` (identity, multiplication, transpose, vector transform)

I wrote unit tests for both classes using GoogleTest. This helped me catch multiple early mistakes (column-major indexing in matrices, normalization edge cases). The tests were useful later when extending the math library for transformations.

At this stage, the project was mostly non-visual, but it helped me build confidence in the math layer.

---

## Week 2 (Sep 27–Oct 2) — Matrix transforms & camera

I added basic transformation utilities (translation, rotation, scaling) on top of `Mat4` and began working on a simple camera abstraction.

Initially, I struggled with column-major vs row-major conventions, which caused inverted rotations and incorrect object placement. After re-reading the seminar slides and checking OpenGL conventions, I fixed this by being consistent everywhere.

I added simple tests for transformation composition to ensure correctness.

---

## Week 3 (Oct 4–9) — Rendering primitives & axes

I refactored `Application.cpp` to reduce its size and moved rendering utilities into a `graphics/` folder.

I implemented:

* a mesh abstraction
* procedural generation of rings and ring segments
* axis rendering for debugging (X/Y/Z colored axes)

At this stage I could render a ground disk, axes, and simple ring shapes. This helped me visualize the coordinate system and detect mistakes early.

---

## Week 4 (Oct 11–16) — Bricks & paddles (first attempt)

I implemented the circular brick wall and paddles as ring segments.

Mistake:

* I initially stacked brick “rows” radially (multiple rings inside each other), instead of vertically.
* This made the structure incorrect compared to the assignment description.

After reviewing the assignment again, I realized the wall should be **vertical**, not radial. I reworked the brick structure so all rows share the same radii and are stacked in Z.

This refactor simplified rendering and later collision detection.

---

## Week 5 (Oct 18–23) — Scene scaling & structure cleanup

I introduced a `SceneScale` struct to centralize all gameplay dimensions (brick radii, paddle radii, ball radius, etc.).

This was extremely useful because:

* initial scale values were inconsistent
* the ball was too big
* the ground was too small

After tuning these values, the scene visually matched the reference images from the assignment much better.

---

## Week 6 (Oct 25–30) — Game state & input

I created a `GameState` structure to separate gameplay logic from rendering.

This included:

* paddle angle and angular velocity
* ball position and speed
* game mode (waiting, playing, win, game over)

At first, I updated the ball in polar coordinates (`r`, `θ`), which resulted in strange spiral or elliptical motion. This was a mistake.

---

## Week 7 (Nov 1–6) — Physics refactor (major redesign)

I realized the physics must be handled in **Cartesian space**, not polar.

I refactored the entire ball movement system to use:

* `(x, y)` position
* `(vx, vy)` velocity
* constant speed normalization

Polar coordinates were kept only for rendering and broad-phase collision checks.

This fixed the “weird ellipse” motion and made collision response much easier.

---

## Week 8 (Nov 8–13) — Paddle collisions (curved geometry)

I implemented paddle collision using an annular sector model:

* inner arc
* outer arc
* two radial edges

Initially, collisions were incorrect:

* only one paddle reacted
* the ball sometimes passed through
* the paddle curvature barely affected bounce direction

The fix involved:

* true closest-point computation on the annular sector
* checking all paddles (3 paddles equally spaced)
* sub-stepping to prevent tunneling
* enforcing a minimum separating velocity after collision

This was one of the hardest parts of the project.

---

## Week 9 (Nov 15–20) — Brick collisions & gameplay rules

I implemented ball-brick collisions using the same curved annular sector approach.

Mistake:

* At first, I removed the *top* brick of a column whenever a collision occurred.

After re-reading the assignment, I corrected the logic:

* the **bottom brick** is removed first
* bricks above fall down
* the ball can only pass once all bricks in a column are removed

This required redesigning the brick data structure.

---

## Week 10 (Nov 22–27) — Brick collapse & animation

I implemented proper brick collapse:

* each column is a stack of slots
* removing one brick shifts all above bricks down

Initially, the whole column disappeared due to repeated collision resolution in a single frame. This was fixed by:

* limiting brick removal to once per physics step
* adding a small hit cooldown

For visual feedback, I added a simple falling animation using:

* `row_visual`
* `row_target`
* linear interpolation per frame

This finally made the wall behavior match the assignment description.

---

## Week 11 (Nov 29–Dec 4) — Gameplay tuning

I focused on playability:

* added “aiming” influence based on where the ball hits the paddle
* added small tangential bias so the ball doesn’t stay in the same column
* prevented the ball from gliding or sticking to paddles

This made the game much more controllable and fun to play.

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

* very shallow paddle hits
* fast ball speeds
* hitting brick edges

I fixed remaining bugs related to collision cooldowns and small penetration errors.

---

## January polish (Jan 2–6)

After a short break, I did final polish:

* cleaned comments
* removed dead code
* verified behavior against assignment requirements
* re-read seminar slides to ensure compliance

---
