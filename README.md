<h1 align="center">
  2D Physics Simulation (C++ / SDL3)
</h1>

<p align="center">
  <a href="#introduction">Introduction</a> •
  <a href="#project-progress">Project Progress</a> •
  <a href="#challenges">Challenges</a> •
  <a href="#future-work">Future Work</a> •
  <a href="#build">Build</a>
</p>

---

## Introduction

This project is a real-time 2D physics simulator written in C++ using SDL3.

It was developed to explore core concepts behind physics engines, including
numerical integration, collision detection, and low-level rendering, without
relying on external physics libraries.

The main focus is on building reusable simulation components and gaining
hands-on experience with engine-style architecture.

---

## Project Progress

### Current Features

- Real-time rendering using SDL3
- Frame-rate independent motion
- Gravity and velocity integration
- Boundary collision handling
- Rectangle and circle primitives
- Basic collision response

### Demo


### Planned Features

## Challenges
One of the initial challenges was becoming familiar with the SDL3 API and its
core structures, such as rendering contexts and rectangle primitives. Early
development focused on understanding the event loop and rendering pipeline,
with the first milestone being the successful creation of a basic window and
rendered object.

A significant learning curve involved transitioning from procedural C-style
programming to object-oriented design in C++. This required developing an
understanding of class structure, constructors and destructors, access
specifiers, and object interactions. Adopting a more modular and encapsulated
design approach represented a major shift from previous development patterns.

Implementing collision detection introduced further complexity. Early versions
used simple visual feedback, including dynamic colour changes (love flashing colours), to assist with
debugging collision events and validating behaviour during testing.

Extending movement beyond a single axis required introducing velocity-based
motion in both the x and y directions. This was followed by the implementation
of gravity and basic energy loss to simulate realistic bouncing behaviour.

Vertical motion was modelled using time-based integration:
```bash
vel_y += gravity * deltaTime
```
and bounce damping was introduced using a restitution factor:
```bash
vel_y = -vel_y * 0.8
```
Balancing stability, realism, and performance during these stages required
iterative testing and refinement.
