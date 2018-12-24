/** @file *//********************************************************************************************************

                                                 PointParticle.fx

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: $

	$NoKeywords: $

 ********************************************************************************************************************/

//float4x4	ViewMatrix;
//float4x4	ProjectionMatrix;
float		Size;

technique T0
{
    pass P0
    {
		// Vertex shader constants
		
//		VertexShaderConstant4[0]	= < ViewMatrix >;
//		VertexShaderConstant4[4]	= < ProjectionMatrix >;
//		VertexShaderConstant1[8]	= { 10, 0, 0, 1 };


		PointSize			= < Size >;
		PointSize_Min		= 0.5;
		PointScaleEnable	= TRUE;
		PointScale_A		= 0.0;
		PointScale_B		= 0.0;
		PointScale_C		= 1.0;

        VertexShader	= NULL;
//			asm
//			{
//				vs_1_1
//
//				// Constants
//				//	c0-c3	view-projection matrix
//
//				// Inputs
//				dcl_position0	v0			// position data
//				dcl_color0		v1			// color data
//
//				// Outputs
//				//	oPos	vertex coordinates
//				//	oD0		diffuse vertex color
//				//	oPts	point size
//
//				m4x4		r0, v0, c0		// transform vertex by view matrix
//				m4x4		oPos, r0, c4	// transform vertex by projection matrix
//				mov			r0.x, c4
//				m4x4		r1, r0, c0		// transform point size by projection matrix
//				rcp			r1.w, r1.w
//				mul			oPts, r1.x, r1.w
//				mov			oD0, v1			// pass diffuse color
//				
//			};
        PixelShader	= NULL;
//			asm
//			{
//				ps_1_1
//				
//				// Inputs
//				// v0		Diffuse color
//				
//				mov		r0, v0
//			};
	}
}
