Effect of Drag on Particle Motion
===================================

Overview
--------

This paper describes the derivation and application of the effect of drag on the position and velocity of each particle.

Foundations
-----------

Drag is a force due to friction between the particle and the medium that it travels through. The force is in the opposite direction of the motion and is proportional to the square of the speed. Drag is defined by the following equation:

    D = -Cd * r * s^2 * A / 2

where,

* *Cd* is the "coefficient of drag"
* *r* is the density of the medium
* *s* is the speed of the particle
* *A* is the equivalent area of the particle 

Including the acceleration due to gravity, *g*, the overall acceleration is:

	g + D/m

In this particle system, the mass of a particle is fixed at 1. However, because the acceleration due to drag is proportional to the mass, the actual mass can be accounted for by changing the value of *c*.

The velocity of the particle at time *t* is given by this equation (https://en.wikipedia.org/wiki/Drag_(physics)#Velocity_of_a_falling_object):

	v = Vt tanh(t * g / Vt + atanh(V0 / Vt))

where *V0* is the initial velocity and *Vt* is the terminal velocity computed as:

	Vt = sqrt(2 * m * g / (r * A * Cd))

The equations can be simplified by assuming that *Cd*, *r*, and *A* are constant, allowing them to be combined into a single constant:

    c = Cd * r * A / 2

This gives:

    D = c * s^2
    Vt = sqrt(g / c)

The distance traveled after time *t* is given by this equation:

	d = Vt^2 / 2g * (2 ln(cosh((gt + Vt * atanh(V0/Vt)) / Vt) * (Vt^2-V0^2)/Vt^2))

These computations all assume that the velocity is always in the direction of gravity. If the initial velocity is not in the same direction, then the computation is slightly more complicated. Note that because *D* is derived from the square of the speed, it can be split into the sum of two vectors:

	D = Dg + Df

where *Dg* is the drag in the direction of gravity and *Df* is the perpendicular drag.

Without the acceleration due to gravity, the perpendicular velocity is simply

	Vf = V0f / (V0f * t + 1)

The velocities in each direction are independent so they can be combined:

	V = V0f / (V0f * t + 1) + Vt tanh(t * g / Vt + atanh(V0g / Vt))

Implementation
--------------

Calculating the position is expensive. This is a visual simulation and it is assumed that a particle does not need to follow its path precisely. So, a basic Euler method is used to calculate the position:

	p(i) = p(i-1) + v(i) * dt

*v(i)* is computed using the formula above using v(i-1) as *V0*. Here is the code:

Computed once:

	direction_g = normalize(g)
	direction_f = [direction_g.y, -direction_g.x]
	Vt = direction_g * sqrt(|g| / c)

Computed each update:

	V0g = direction_g * dot(v(i-1), direction_g)
	V0f = direction_f * dot(v(i-1), direction_f)
	Vg = Vt * tanh(t * g / |Vt| + atanh(|V0g| / |Vt|))
	Vf = direction_f * (|V0f| / (|V0f| * t + 1))
	v(i) = Vf + Vg
