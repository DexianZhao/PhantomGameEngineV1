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
float       m_alphaAmount	=	0.05f;
float		sBrightness = 0;
float		sContrast	= 1;

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
	float3 Normal			=	normalize(mul(input.Normal, (float3x3)sWorldViewIT));
	output.Pos				=	mul(pos, sViewProj);
	output.uv				=	input.uv;
	output.Color.rgb     	=	sDiffuse.rgb * input.Color.rgb * saturate( dot( sLightDirInv, Normal ) ) + sEmissive.rgb;
	output.Color.a		    =   sDiffuse.a * input.Color.a;
	return output;
}
float4	Pshader(VOutput output) : COLOR0
{
	float4 ret = output.Color;
	ret	*= tex2D(sampler1, output.uv);
	if(ret.a < m_alphaAmount)
		discard;
	else
	{
		ret.rgb = ((ret.rgb - 0.5f) * sContrast) + 0.5f;
		ret.rgb += sBrightness;
		//ret.rgb	+=	ret.rgb * ret.a;
	}
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
	passColor0.rgb	=	sDiffuse.rgb * inColor.bgr * clamp(dot(sLightDirInv, Normal), 0.0, 1.0) + sEmissive.rgb;
	passColor0.a	=	sDiffuse.a * inColor.a;
	gl_Position		=	sViewProj * pos;
}

/*[/OpenglES_Vertex]*/

/*[OpenglES_Pixel]*/
precision mediump float;
varying vec4		passColor0;
varying vec2		passCoord;
uniform sampler2D	texture1;
uniform	float		m_alphaAmount;
uniform	float		sBrightness;
uniform	float		sContrast;

void main()
{
	vec4 ret = texture2D(texture1, passCoord) * passColor0;
	ret.rgb = ((ret.rgb - 0.5) * sContrast) + 0.5;
	ret.rgb += sBrightness;
	if(ret.a < 0.05)//0.2f是错误的，不能带f, IPhone 中clamp等，必须要使用.0,否则编译不通过
		discard;
	gl_FragColor	=	ret;
}
/*[/OpenglES_Pixel]*/
