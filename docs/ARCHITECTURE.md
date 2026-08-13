# PSX Maze Shooter — Architecture

## Stack

- **Window & input**: GLFW, fetched directly (not through raylib — see note below)
- **Rendering**: hand-written Vulkan, bootstrapped with vk-bootstrap
- **Logging**: fmt
- **Audio (later)**: raylib's `raudio` module — independent of the window, safe to add back in
- **Math (later)**: raylib's `raymath.h` — header-only, safe to use anywhere

### Why not raylib for the window?

raylib bundles its own internal copy of GLFW. This project also fetches its own
separate copy of GLFW directly, to call `glfwCreateWindowSurface()` for Vulkan.
Two separate compiled copies of GLFW means two separate pieces of global state —
raylib's `InitWindow()` initializes *its* copy, but our own linked GLFW functions
were never initialized, so anything Vulkan-related through our copy fails. Using
one GLFW instance (ours) for the whole window/input lifecycle avoids the conflict
entirely.

## Milestone status

- [x] Milestone 1 — Vulkan instance, device, swapchain, GPU selectable via `--gpu <name>`
- [x] Milestone 2 — clear the swapchain to a color every frame and present it
- [ ] Milestone 3 — maze generation + first-person camera/movement + wall collision
- [ ] Milestone 4 — weapons: knife melee + pickups + raycast hit detection
- [ ] Milestone 5 — monster FSM (patrol → alert → chase → attack) + pathing
- [ ] Milestone 6 — difficulty tiers wired to spawn tables
- [ ] Milestone 7 — render pass + graphics pipeline + PSX shader (vertex snap, affine UVs, low-res + dither)
- [ ] Milestone 8 — polish: HUD, sound, win/lose screens

## High-level flow



## Per-system flowcharts

Each major gameplay system gets its own diagram in this folder as it's built
(e.g. `docs/maze-generation.md`, `docs/monster-fsm.md`), rather than one
diagram per line of code — a literal per-line flowchart of the whole game
would be thousands of unreadable boxes. Function/system-level is the useful
granularity for actually understanding or documenting the code.
