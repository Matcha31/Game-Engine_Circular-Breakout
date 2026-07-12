# PA199 Project Corrections Devlog

This correction addressed the issues reported during the evaluation of the
final project.

### Gameplay difficulty

- Reduced the speed of the ball to make the level reasonably playable.
- Adjusted paddle movement and paddle influence on the ball.
- Tuned collision parameters to prevent moving paddles from sending the ball at
  excessively tangential angles.
- Preserved a constant ball speed after collisions.

### Camera corrections

- Corrected the initial position and orientation of the perspective camera.
- Corrected camera updates after resizing the window.
- Ensured that the top camera remains an orthographic top-down view after a resize.
- Verified switching between the perspective and top cameras using the `1` and
  `2` keys.

### Lighting corrections

- Corrected the light direction so that the light is located above the play area.
- Reduced the lighting intensity that made the scene excessively bright in the
  top view.
- Retuned ambient and specular contributions for better readability of the ball,
  paddles, bricks, and textured ground.

### Brick wall corrections

- Corrected the selection and destruction of the exposed brick in each column.
- Fixed the collapse of bricks after the bottom brick is destroyed.
- Added a visible falling transition from the previous row to the new row.
- Corrected brick colour assignment after bricks move to another row.
- Corrected the handling of the empty top position after a column collapses.

### Collision detection and response

- Reworked paddle and brick collision detection using their curved annular-sector
  geometry.
- Added collision checks for the inner arc, outer arc, and both radial side edges.
- Used closest-point calculations for collisions near the ends of paddles and
  bricks.
- Added physics substeps to reduce tunnelling and frame-rate-dependent collision
  errors.
- Added penetration correction to move the ball outside a collider after contact.
- Added a minimum separating velocity and a short paddle collision cooldown.
- Corrected collisions against the sides of paddles where the ball could
  previously become stuck.
- Corrected cases where the ball could glide along a moving paddle.
- Reduced excessive sideways deflection caused by paddle movement.
- Kept paddle movement involved in the resulting bounce direction without
  allowing it to dominate the ball trajectory.
