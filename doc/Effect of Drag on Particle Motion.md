Effect of Drag on Particle Motion
===================================

Overview
--------

This paper describes the derivation and application of the effect of drag on the position and velocity of each particle.

Foundations
-----------

Drag is a force due to the friction between the particle and the medium that it travels through. The force is in the opposite direction of the motion and is proportional to the square of the speed. The magnitude of the drag is defined by the following equation:

![Drag Definition](drag-1.png)
<!-- (1) \left | D \right | = \frac { r A C_d \left | v \right |^2 } {2} -->

where,

 * *C<sub>d</sub>* is the "coefficient of drag"
 * *r* is the density of the medium
 * *v* is the velocity of the particle
 * *A* is the equivalent area of the particle

Including the acceleration due to gravity, *g*, the overall acceleration is:

![Overall Acceleration](drag-2.png)
<!-- (2) a = g + \frac {D} {m} -->

In this particle system, the mass of a particle is assumed to be 1. However, the actual mass can be included as a factor in the value of *C<sub>d</sub>* (or *r* or *A*).

The velocity of the particle at time *t* is given by this equation (https://en.wikipedia.org/wiki/Drag_(physics)#Velocity_of_a_falling_object):

![Basic Velocity Over Time](drag-3.png)
<!-- (3) v(t) = v_T \tanh { \frac { t g } { v_T } + \tanh^{-1} { \frac { v_0 } { v_T } } } -->

where *v<sub>0</sub>* is the initial velocity and *v<sub>T</sub>* is the terminal velocity computed as:

![Terminal Velocity](drag-4.png)
<!-- (4) v_T = \sqrt { \frac { 2 m g} { r A C_d } } -->

The equations can be simplified by assuming that *C<sub>d</sub>*, *r*, and *A* are constant, allowing them to be combined:

![Aggregated Constants](drag-5.png)
<!-- (5) k = \frac { r A C_d } { 2 } -->

This gives:

![Simplified Drag](drag-6.png)
<!-- (6) \\ \left | D \right | = k \left | v \right |^2 \\ \\ v_T = \sqrt { \frac { g } { k } } -->

The distance traveled after time *t* is given by this equation:

![Distance over time](drag-7.png)
<!-- (7) d(t) = \frac { v_T^2 } { g } \ln {\frac { v_T^2 - v_0^2 } { v_T^2 }} + \frac { v_T^2 } { g } \ln ( \cosh ( \frac { t  g } { v_T } + \tanh^{-1} { \frac { v_0 } { v_T } }  )D_g + D_f -->

These computations all assume that the velocity is always in the direction of gravity. If the initial velocity is not in the same direction, then the computation can be split into the sum of two independent vectors:

![Componentized Drag](drag-8.png)
<!-- (8) \mathbf{D} = \mathbf{D}_g + \mathbf{D}_f -->

where *D<sub>g</sub>* is the drag in the direction of *g* and *D<sub>f</sub>* is the drag perpendicular to *g*.

The perpendicular velocity is simply:

![Perpendicular Velocity Over Time](drag-9.png)
<!-- (9) \boldsymbol{v}_f(t) = \frac { \boldsymbol{v}_0_f } { v_0_f k t + 1 } -->

Combined:

![Overall Velocity Over Time](drag-10.png)
<!-- (10) \boldsymbol{v}(t) = \frac { \mathbf{v}_0_f } { v_0_f k t + 1 } + \mathbf{v}_T tanh ( \frac { t g } { v_T } + tanh^{-1} { \frac { v_0_g } { v_T } } ) -->

Implementation
--------------

Calculating the position is expensive. Euler integration is used to calculate the position because it is probably good enough:

![Euler Integration](drag-11.png)
<!-- (11) \mathbf{x}(i) = \mathbf{x}(i-1) + \mathbf{v}(i-1) \Delta t -->

*v(i)* is computed using the equations above substituting *v(i-1)* for *v<sub>0</sub>*. Here is the code:

Computed once:

	k = r * A * Cd / 2
	direction_g = normalize(g)
	direction_f = [-direction_g.y, direction_g.x]
	x = x0
	vT = direction_g * sqrt(|g| / k)
	vg = direction_g * dot(v0, direction_g)
	vf = direction_f * dot(v0, direction_f)
	v = vf0 + vg0

Computed each update:

	x = x + v * dt
	vg = vT * tanh(dt * g / |vT| + atanh(|vg| / |vT|))
	vf = direction_f * (|vf| / (|vf| * k * dt + 1))
	v = vf + vg
