/*[DirectX9]*/
matrix	sWorld;
matrix	sViewProj;
matrix	sWorldViewIT;
float4	sDiffuse;
float4	sEmissive;
float3	sLightDir;
float3	sLightDirInv;
int filterMin = 2;
int filterMag = 2;
int filterMip = 2;
texture texture1;
int	sAlphaTest = 0;
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
	float2 uv		: TEXCOORD0;
	/*[Blend]*/
};
struct VOutput{
	float4 Pos		: POSITION;
	float4 Color	: TEXCOORD1;
	float2 uv		: TEXCOORD0;
};
float		sBrightness = 0;
float		sContrast	= 1;

VOutput Vshader(VInput input)
{
	VOutput output;
	//必须要加入此两行作为程序调用基础，可以自动调用骨骼混合
	/*[BlendPosNormal]*/
	float4 pos				=	mul(input.Pos, sWorld);
	float3 Normal			=	normalize(mul(input.Normal, (float3x3)sWorldViewIT));
	output.Pos				=	mul(pos, sViewProj);
	output.uv				=	input.uv;
	output.Color.rgb     	=	sDiffuse.rgb * saturate( dot( sLightDirInv, Normal ) ) + sEmissive.rgb;
	output.Color.a		    =   sDiffuse.a;
	return output;
}
float4	Pshader(VOutput output) : COLOR0
{
	float4 ret = output.Color;
	ret	*= tex2D(sampler1, output.uv);
	//ret.rgb = ((ret.rgb - 0.5f) * sContrast) + 0.5f;
	//ret.rgb += sBrightness;
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
/*[Blend]*/
//
uniform mat4	sWorld;
uniform mat4	sViewProj;
uniform mat4	sWorldViewIT;
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
	Normal			=	normalize(mat3(sWorldViewIT) * Normal);
	passCoord		=	inTexcoord;
	passColor0.rgb	=	sDiffuse.rgb * clamp(dot(sLightDirInv, Normal), 0.0, 1.0) + sEmissive.rgb;
	passColor0.a	=	sDiffuse.a;
	gl_Position		=	sViewProj * pos;
}

/*[/OpenglES_Vertex]*/

/*[OpenglES_Pixel]*/
precision mediump float;
varying vec4		passColor0;
varying vec2		passCoord;
uniform sampler2D	texture1;
uniform int		sAlphaTest;
uniform	float		sBrightness;
uniform	float		sContrast;

void main()
{
	vec4 ret = texture2D(texture1, passCoord) * passColor0;
	//ret.rgb = ((ret.rgb - 0.5) * sContrast) + 0.5;
	//ret.rgb += sBrightness;
	gl_FragColor	=	ret;
}
/*[/OpenglES_Pixel]*/
