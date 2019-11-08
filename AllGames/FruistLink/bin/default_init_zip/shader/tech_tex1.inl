/*[DirectX9]*/
matrix MVP;
int filterMin = 2;
int filterMag = 2;
int filterMip = 2;
texture texture1;

sampler sampler1 =	sampler_state
{
	Texture		= <texture1>;
	MinFilter	=  <filterMin>;
	MagFilter	=  <filterMag>;
	MipFilter	=  <filterMip>;
};

struct VInput{
	float4 Pos		: POSITION;
	float2 uv		: TEXCOORD0;
};
struct VOutput{
	float4 Pos		: POSITION;
	float4 Color	: TEXCOORD1;
	float2 uv		: TEXCOORD0;
};

VOutput Vshader(VInput input)
{
	VOutput output;
	output.Pos		=	mul(input.Pos, MVP);
	output.Color	=	float4(1,1,1,1);
	output.uv		=	input.uv;
	return output;
}
float4	Pshader(VOutput output) : COLOR0
{
	float4 ret = output.Color;
	ret	*= tex2D(sampler1, output.uv);
	return ret;
}

technique main
{
	pass P
	{
		VertexShader = compile vs_2_0 Vshader();
		PixelShader  = compile ps_2_0 Pshader();
	}
}

/*[/DirectX9]*/

/*[OpenglES_Vertex]*/
attribute vec4	inPosition;
attribute vec2	inTexcoord;
//
uniform mat4	MVP;
varying vec2	passCoord;
varying vec4	passColor0;
void main()
{
	passCoord		=	inTexcoord;
	passColor0		=	vec4(1,1,1,1);
	gl_Position	=	MVP * inPosition;
}
/*[/OpenglES_Vertex]*/
/*[OpenglES_Pixel]*/
precision lowp float;
varying vec4		passColor0;
varying vec2		passCoord;
uniform sampler2D texture1;
void main()
{
	gl_FragColor = texture2D(texture1, passCoord);
}
/*[/OpenglES_Pixel]*/
