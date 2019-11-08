/*[DirectX9]*/
matrix	sWorld;
matrix	sViewProj;
float4	sDiffuse;
float4	sEmissive;
float3	sLightDir;
float3	sLightDirInv;
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
	output.Color.rgb     	=	input.Color.rgb;
	output.Color.a		    =   input.Color.a;
	return output;
}
float4	Pshader(VOutput output) : COLOR0
{
	float4 ret = output.Color;
	ret	*= tex2D(sampler1, output.uv);
	if(output.uv.x < 0 || output.uv.y < 0 || output.uv.x > 1.0 || output.uv.y > 1.0)
		discard;
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
uniform vec3	sLightDirInv;
uniform vec4	sEmissive;
uniform vec4	sDiffuse;
//
//
varying vec2	passCoord;
varying vec4	passColor0;
void main()
{
	/*[BlendPosNormal]*/
	pos				=	sWorld * pos;
	passCoord		=	inTexcoord;
	passColor0.rgb	=	inColor.bgr;
	passColor0.a	=	inColor.a;
	gl_Position		=	sViewProj * pos;
}

/*[/OpenglES_Vertex]*/

/*[OpenglES_Pixel]*/
precision mediump float;
varying vec4		passColor0;
varying vec2		passCoord;
uniform sampler2D	texture1;
void main()
{
	gl_FragColor = texture2D(texture1, passCoord) * passColor0;
	if( passCoord.x < 0.0 || passCoord.y < 0.0 || passCoord.x > 1.0 || passCoord.y > 1.0 )
		discard;
}
/*[/OpenglES_Pixel]*/
