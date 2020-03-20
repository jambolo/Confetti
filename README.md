# Confetti

**A C++ particle system using Vulkan**

## Overview

Confetti is a Vulkan-based particle system written in C++. It is scalable, dynamic and configurable.

## Components Overview

Confetti has the following components:

### Particle System
A particle system contains of a list of emitters, and lists of appearances and environments used by the emitters.

### Particle
There are five different types of particles: point, streak, textured, sphere, and emitter. The basic particle has a lifetime, a color, and an initial position and velocity. The different types each add additional unique properties associated with the type. The point particle is the simplest, but the emitter particle itself emits others particles. When a particle reaches the end of its lifetime, it is reset to its initial conditions.

### Emitter
All particles are contained within an emitter. The characteristics of the particles being emitted and the emission itself are controlled by the emitter. An emitter has a volume from which the particles are emitted and maintains the appearance of the emitted particles. An emitter can move and be enabled and disabled.

### Environment
An environment describes the characteristics of the world in which an emitter exists. The environment has parameters that affect the paths of the particles: gravity, air friction, wind (and gusts), surfaces and clip planes. Emitters can share environments or have different environments.

### Emitter Volume
An emitter has a volume and particles are emitted from uniformly distributed random locations within that volume. There are eight types of volumes: point, line, rectangle, circle, sphere, box, cylinder, and cone.

### Clip plane
Particles that move through a clip plane are immediately reset. Clip planes are part of an environment.

### Surface
Particles bounce off of a surface according to its parameters. Surfaces are part of an environment.

### Appearance
An appearance describes various characteristics of appearance of the particles emitted by an emitter. An appearance can be shared by multiple emitters.

### Builder
A builder can instantiate any of the components listed above as well as a complete particle system based on a JSON-formatted configuration.
