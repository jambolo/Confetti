/** @file *//********************************************************************************************************

                                                 TexturedParticle.fx

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: $

	$NoKeywords: $

 ********************************************************************************************************************/


texture		g_texture;
float4x4	g_viewProjection	: ViewProjection;
float4x4	g_cameraOrientation	: InverseView;

sampler	s0 = sampler_state
		{
			Texture		= ( g_texture );
			MinFilter	= LINEAR;
			MagFilter	= LINEAR;
			MipFilter	= LINEAR;
			AddressU	= CLAMP;
			AddressV	= CLAMP;
		};


float4 RotateZ( float4 v, float angle )
{
	float	s, c;
	sincos( angle, s, c );

	float4	result;
	
	result.x = c * v.x - s * v.y;
	result.y = s * v.x + c * v.y;
	result.z = v.z;
	result.w = v.w;
	
	return result;
}

struct VSOutput
{
	float4	position		: POSITION;
	float4	color			: COLOR;
	float2	texturePosition	: TEXCOORD0;
};

VSOutput VS( float4 position		: POSITION0,
			 float4 color			: COLOR0,
			 float2 texturePosition	: TEXCOORD0,
			 float radius			: TEXCOORD1,
			 float rotation			: TEXCOORD2 )
{
	VSOutput	result;

	
	// Convert the texture position into the offset to the vertex
	
	float4	offset	= { ( 2.0 * texturePosition - 1.0 ) * radius, 0, 0 };
	
	// Rotate
	
	offset = RotateZ( offset, rotation );
	
	// Align the offset with the camera's view plane
	
	float4 worldOffset = mul( offset, g_cameraOrientation );

	// Set return values
	
	result.position			= mul( position + worldOffset, g_viewProjection );
	result.color			= color;
	result.texturePosition	= texturePosition;

	return result;
}

struct PSOutput
{
	float4	color	: COLOR;
};

PSOutput PS( float4 color			: COLOR,
			 float2 texturePosition	: TEXCOORD )
{
	PSOutput	result;

	result.color = color * tex2D( s0, texturePosition );
	
	return result;
}


technique T0
{
	pass P0
	{
		Sampler[0]		= ( s0 );
		VertexShader	= compile vs_2_0 VS();
		PixelShader		= compile ps_2_0 PS();
	}
}
