/** @file *//********************************************************************************************************

                                                 StreakParticle.fx

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: $

	$NoKeywords: $

 ********************************************************************************************************************/

float4x4	ViewProjectionMatrix;

technique T0
{
    pass P0
    {
		// Vertex shader constants
		
		VertexShaderConstant4[0]	= < ViewProjectionMatrix >;

        VertexShader =
			asm
			{
				vs_1_1

				// Constants
				//	c0-c3	view-projection matrix

				// Inputs
				dcl_position0	v0			// position data
				dcl_color0		v1			// color data

				// Outputs
				//	oPos	vertex coordinates
				//	oD0		diffuse vertex color

				m4x4		oPos, v0, c0	// transform vertices by view-projection matrix
				mov			oD0, v1			// pass diffuse color
			};
        PixelShader  =
			asm
			{
				ps_1_1
				
				// Inputs
				// v0		Diffuse color
				
				// Texture Stages
				// t0		texture

				mov		r0, v0
			};
    }
}
