vs_1_1

// Constants
//	c0-c3	world-projection matrix

// Inputs
dcl_position0	v0			// position data
dcl_color0		v1			// color data
dcl_texcoord0	v2			// uv coords

// Outputs
//	oPos	vertex coordinates
//	oD0		diffuse vertex color
//	oT0		texture uv

m4x4		oPos, v0, c0	// transform vertices by world-projection matrix
mov			oD0, v1			// pass diffuse color
mov			oT0, v2			// pass uv
