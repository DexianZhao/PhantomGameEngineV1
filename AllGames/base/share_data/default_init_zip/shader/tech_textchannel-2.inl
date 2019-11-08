/*[DirectX9]*/
matrix MVP;
int filterMin = 2;
int filterMag = 2;
int filterMip = 2;
texture texture1;
matrix 	sUVMatrix;
float4	sScreenDet;
float4	sUIScale;
sampler sampler1 =	sampler_state
{
	Texture		= <texture1>;
	MinFilter	=  POINT;
	MagFilter	=  POINT;
	MipFilter	=  POINT;
};

struct VInput{
	float4 Pos		: POSITION;
	float3 Normal	: Normal;
	float4 Color	: COLOR0;
	float2 uv		: TEXCOORD0;
};
struct VOutput{
	float4 Pos		: POSITION;
	float4 Color	: TEXCOORD1;
	float3 channel	: TEXCOORD2;
	float2 uv		: TEXCOORD0;
};

VOutput Vshader(VInput input)
{
	VOutput output;
	float4	pos		=	input.Pos;
	pos.xyz			=	mul(pos.xyz, (float3x3)sUVMatrix);
	pos.x			=	(pos.x + sUVMatrix[3][0])*sUIScale.x+sUIScale.z;
	pos.y			=	(pos.y + sUVMatrix[3][1])*sUIScale.y+sUIScale.w;
	pos.x			=	((pos.x/sScreenDet.x)-0.5)*sScreenDet.z;
	pos.y			=	(0.5-(pos.y/sScreenDet.y))*sScreenDet.w;
	output.Pos		=	mul(pos, MVP);
	output.Color	=	input.Color;
	output.uv		=	input.uv;
	output.channel	=	input.Normal;
	return output;
}
float4	Pshader(VOutput output) : COLOR0
{
	float4 ret = output.Color;
	float3 temp = tex2D(sampler1, output.uv).xyz * output.channel;
	float f = (temp.x + temp.y + temp.z);
	ret.xyz *= f;
	ret.a *= f;
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
//attribute in
attribute vec4		inPosition;
attribute vec2		inTexcoord;
attribute vec4		inColor;
//
uniform mat4		MVP;
varying vec2		passCoord;
varying vec4		passColor0;
void main()
{
	passCoord		=	inTexcoord;
	passColor0		=	vec4(inColor.b, inColor.g, inColor.r, inColor.a);
	gl_Position	=	MVP * inPosition;
}
/*[/OpenglES_Vertex]*/
/*[OpenglES_Pixel]*/
precision lowp		float;
varying vec4		passColor0;
varying vec2		passCoord;
uniform sampler2D	texture1;
uniform vec3		textChannel;
void main()
{
	vec3 v = texture2D(texture1, passCoord).xyz * textChannel;
	float f = (v.x+v.y+v.z);
	gl_FragColor = vec4(f*passColor0.xyz, f*passColor0.a*255.0);
}
/*[/OpenglES_Pixel]*/
