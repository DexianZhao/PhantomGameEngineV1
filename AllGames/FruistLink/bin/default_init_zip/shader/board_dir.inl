/*[DirectX9]*/
matrix	sWorld;
matrix	sViewProj;
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
	float3 Normal	: NORMAL;
	float4 Color	: COLOR0;
	float2 uv		: TEXCOORD0;
	float2 uv2		: TEXCOORD1;
	/*[Blend]*/
};
struct VOutput{
	float4 Pos		: POSITION;
	float4 Color	: TEXCOORD1;
	float2 uv		: TEXCOORD0;
};

VOutput Vshader(VInput input)
{
	VOutput output;
	//必须要加入此两行作为程序调用基础，可以自动调用骨骼混合
	/*[BlendPosNormal]*/
	float4 pos				=	mul(input.Pos, sWorld);
	output.Pos				=	mul(pos, sViewProj);
	output.uv				=	input.uv;
	output.Color	    	=	input.Color;
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
attribute vec3	inNormal;
attribute vec2	inTexcoord;
attribute vec4	inColor;
/*[Blend]*/
//
uniform mat4	sWorld;
uniform mat4	sViewProj;
//
varying vec2	passCoord;
varying vec4	passColor0;
void main()
{
	vec4 pos		=	sWorld * inPosition;
	passCoord		=	inTexcoord;
	passColor0.rgba	=	inColor.bgra;
	gl_Position		=	sViewProj * pos;
}

/*[/OpenglES_Vertex]*/

/*[OpenglES_Pixel]*/
precision mediump float;
varying vec4		passColor0;
varying vec2		passCoord;
uniform sampler2D	texture1;
uniform	int			sAlphaTest;
void main()
{
	gl_FragColor = texture2D(texture1, passCoord) * passColor0;
}
/*[/OpenglES_Pixel]*/
