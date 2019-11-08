/*[DirectX9]*/
matrix MVP;
int filterMin = 2;
int filterMag = 2;
int filterMip = 2;
texture texture1;
float4 textChannel;
sampler sampler1 =	sampler_state
{
	Texture		= <texture1>;
	MinFilter	=  POINT;
	MagFilter	=  POINT;
	MipFilter	=  POINT;
};

struct VInput{
	float4 Pos		: POSITION;
	float4 Color	: COLOR0;
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
	output.Color	=	input.Color;
	output.uv		=	input.uv;
	return output;
}
float4	Pshader(VOutput output) : COLOR0
{
	float4 ret = output.Color;
	float3 temp = tex2D(sampler1, output.uv).xyz * textChannel.xyz;
	float f = (temp.x + temp.y + temp.z);
	ret.xyz *= f;
	ret.rgb		=	ret.r*0.11 + ret.g * 0.59 + ret.b * 0.30;
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
	v.r = v.g = v.b = f * (passColor0.r*0.11 + passColor0.g * 0.59 + passColor0.b * 0.30);
	gl_FragColor = vec4(v.rgb, passColor0.a*f*255.0);
}
/*[/OpenglES_Pixel]*/
