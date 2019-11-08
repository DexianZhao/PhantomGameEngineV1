/*[DirectX9]*/
matrix MVP;
int filterMin = 2;
int filterMag = 2;
int filterMip = 2;
float4	sDiffuse;
matrix 	sUVMatrix;
float4	sScreenDet;
float4	sUIScale;
float4	sUV;
struct VInput{
	float4 Pos		: POSITION;
	float2 uv		: TEXCOORD0;
	float2 uv2		: TEXCOORD1;
};
struct VOutput{
	float4 Pos		: POSITION;
	float4 Color	: TEXCOORD1;
	float2	uv		: TEXCOORD0;
};
texture texture1;
sampler sampler1 =	sampler_state
{
	Texture		= <texture1>;
	MinFilter	=  <filterMin>;
	MagFilter	=  <filterMag>;
	MipFilter	=  <filterMip>;
};
VOutput Vshader(VInput input)
{
	VOutput output;
	float4	pos		=	float4(input.Pos.x-sUVMatrix[3][2],input.Pos.y-sUVMatrix[3][3],input.Pos.z,input.Pos.w);
	pos.xyz			=	mul(pos.xyz, (float3x3)sUVMatrix);
	pos.x			=	(pos.x + sUVMatrix[3][0])*sUIScale.x+sUIScale.z;
	pos.y			=	(pos.y + sUVMatrix[3][1])*sUIScale.y+sUIScale.w;
	pos.x			=	((pos.x/sScreenDet.x)-0.5)*sScreenDet.z;
	pos.y			=	(0.5-(pos.y/sScreenDet.y))*sScreenDet.w;
	output.Pos		=	mul(pos, MVP);
	output.Color	=	sDiffuse;
	output.uv		=	float2(input.uv.x*sUV.x+input.uv2.x*sUV.z, input.uv.y*sUV.y+input.uv2.y*sUV.w);
	return output;
}
float4	Pshader(VOutput output) : COLOR0
{
	float4 ret = output.Color * tex2D(sampler1, output.uv);
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
attribute vec2		inTexcoord1;
//
uniform mat4		MVP;
uniform vec4		sDiffuse;
uniform mat4 		sUVMatrix;
uniform vec4		sScreenDet;
uniform vec4		sUIScale;
uniform	vec4		sUV;
varying vec2		passCoord;
varying vec4		passColor0;
void main()
{
	passColor0		=	sDiffuse;
	vec4	pos		=	vec4(inPosition.x-sUVMatrix[3][2],inPosition.y-sUVMatrix[3][3],inPosition.z,inPosition.w);
	pos.xyz			=	mat3(sUVMatrix) * pos.xyz;
	pos.x			=	(pos.x + sUVMatrix[3][0])*sUIScale.x+sUIScale.z;
	pos.y			=	(pos.y + sUVMatrix[3][1])*sUIScale.y+sUIScale.w;
	pos.x			=	((pos.x/sScreenDet.x)-0.5)*sScreenDet.z;
	pos.y			=	(0.5-(pos.y/sScreenDet.y))*sScreenDet.w;
	passCoord		=	vec2(inTexcoord.x*sUV.x+inTexcoord1.x*sUV.z, inTexcoord.y*sUV.y+inTexcoord1.y*sUV.w);
	gl_Position	=	MVP * pos;
}
/*[/OpenglES_Vertex]*/
/*[OpenglES_Pixel]*/
precision lowp		float;
varying vec4		passColor0;
varying vec2		passCoord;
uniform sampler2D	texture1;
void main()
{
    gl_FragColor = texture2D(texture1, passCoord) * passColor0;
}
/*[/OpenglES_Pixel]*/
