//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
	Design Writer :   赵德贤 Dexian Zhao
	Email: yuzhou_995@hotmail.com
	 
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_ENUM_H__
#define __PHANTOM_ENUM_H__
#define	TRACK_SECOND_LENGTH				20.0f
namespace Phantom{
	//
	enum FVF
	{
		FVF_XYZ			=	0x002,
		FVF_NORMAL		=	0x010,
		FVF_DIFFUSE		=	0x040,
		FVF_SPECULAR	=	0x080,
		FVF_TEX1		=	0x100,
		FVF_TEX2		=	0x200,
		FVF_TEX3		=	0x300,
		FVF_TEX4		=	0x400,
		FVF_TEX5		=	0x500,
	};
	//
	enum DrawMode
	{
		DrawMode_Points			=	1,
		DrawMode_Lines			=	2,
		DrawMode_LineStrip		=	3,
		DrawMode_Triangles		=	4,
		DrawMode_TriangleStrip	=	5,
		DrawMode_TriangleFan	=	6,
		DrawMode_Count			=	7,
	};
	/*ENUM*/enum TextureStage/*纹理索引*/
	{
		TextureStage_Channel0/*纹理索引_0*/	=	0,
		TextureStage_Channel1/*纹理索引_1*/	=	1,
		TextureStage_Channel2/*纹理索引_2*/	=	2,
		TextureStage_Channel3/*纹理索引_3*/	=	3,
		TextureStage_Channel4/*纹理索引_4*/	=	4,
	};
	enum RenderMode
	{
		//基础渲染模式
		RenderMode_Color		=	0,
		RenderMode_Tex1,
		RenderMode_ColorTex1,
		RenderMode_ColorTex1Disable,
		RenderMode_TextChannel,
		RenderMode_ColorRect,
		RenderMode_TextureRect,
		RenderMode_TextureRectDisable,
		//模型渲染模式
		RenderMode_Default,
		RenderMode_PlaneShadow,
		RenderMode_Custom_Max,
		RenderMode_Max			=	300,	//300个不同渲染组合
	};
	/*ENUM*/enum VspsType/*着色器数据类型*/
	{
		VSPS_TYPE_FLOAT	/*着色器数据类型_小数型*/	=	0,//浮点数据
		VSPS_TYPE_INT	/*着色器数据类型_整数型*/	=	1,//整数数据
		VSPS_TYPE_VECTOR/*着色器数据类型_向量型*/	=	2,//VECTOR4数据
		VSPS_TYPE_MATRIX/*着色器数据类型_矩阵型*/	=	3,//矩阵数据
	};
	/*ENUM*/enum FogMode/*雾类型*/
	{
		FogMode_Fog3D		/*3D雾模式*/		= 0,//3D雾模式
		FogMode_Fog3DGray	/*3D雾模式灰度*/	= 1,//3D雾模式
		FogMode_Fog3DSub	/*3D雾模式减值*/	= 2,//3D雾模式减值
		FogMode_Fog3DHide	/*3D雾模式隐藏有效*/= 3,//仅隐藏有效，不做任何雾效果增加
		FogMode_Fog2D		/*2D雾模式*/		= 5,//2D雾模式，Z周不参与计算
		FogMode_Fog2DGray	/*2D雾模式灰度*/	= 6,//2D雾模式，Z不参与计算，远距离对象进行灰度处理
		FogMode_Fog2DSub	/*2D雾模式减值*/	= 7,//2D雾模式减值，Z不参与计算
		FogMode_Fog2DHide	/*2D雾模式隐藏有效*/= 8,//仅隐藏有效，不做任何雾效果增加
	};

	/*ENUM*/enum ParticleRenderType/*喷射渲染类型*/
	{
		SHOW_Particle_Billboard/*喷射渲染类型_Billboard*/	=	0,
		SHOW_Particle_Cameraboard/*喷射渲染类型_Cameraboard*/	=	1,
		SHOW_Particle_ZBoard/*喷射渲染类型_ZBoard*/		=	2,
		SHOW_Particle_DirectionYZ/*喷射渲染类型_DirectionYZ*/	=	3,	//根据之前的位置叠加渲染
		//SHOW_Particle_DirectionBoard/*喷射渲染类型_DirectionBoard*/	=	4,	//根据之前的位置叠加渲染
		SHOW_Particle_DirectionXYZ/*喷射渲染类型_DirectionXYZ*/	=	5,
		SHOW_Particle_BillboardXY/*喷射渲染类型_BillboardXY*/		=	6,
		SHOW_Particle_BindDirection/*喷射渲染类型_BindDirection*/	=	7,	//此项由内部调用，外部调用视为无效
		SHOW_Particle_XBoard/*喷射渲染类型_XBoard*/		=	8,
		SHOW_Particle_YBoard/*喷射渲染类型_YBoard*/		=	9,
		SHOW_Particle_DirectionXY/*喷射渲染类型_DirectionXY*/	=	10,	//根据之前的位置叠加渲染
		SHOW_Particle_DirectionZX/*喷射渲染类型_DirectionZX*/	=	11,	//根据之前的位置叠加渲染
		SHOW_Particle_ZBoardScene/*喷射渲染类型_ZBoardScene*/		=	12,
	};
	/*ENUM*/enum UnitMoveMode/*单位运动模式*/
	{
		UnitMoveMode_Move			/*单位运动模式_走路模式*/			=	1,
		UnitMoveMode_Run			/*单位运动模式_跑步模式*/			=	2,
		UnitMoveMode_Fly			/*单位运动模式_飞行模式*/			=	3,
		UnitMoveMode_Count			/*单位运动模式_数量*/				=	10,
	};

	/*ENUM*/enum TimeCalculateType/*计算方式*/
	{
		CALC_TYPE_Linear/*计算方式_线性运算*/		=	0,	//线形运算
		CALC_TYPE_Changeless/*计算方式_前一个值固定不变*/	=	1,			//前一个值固定不变
		CALC_TYPE_Hermite/*计算方式_曲线运算*/		=	2,				//曲线运算
	};

	/*ENUM*/enum TextureBlend/*纹理参数混合*/
	{
		// Control
		TextureBlend_DISABLE        = 1,      // disables stage
		TextureBlend_SELECTARG1        = 2,      // the default
		TextureBlend_SELECTARG2        = 3,
		// Modulate
		TextureBlend_MODULATE      /*纹理参数混合_MODULATE  */       = 4,      // multiply args together
		TextureBlend_MODULATE2X    /*纹理参数混合_MODULATE2X*/       = 5,      // multiply and  1 bit
		TextureBlend_MODULATE4X    /*纹理参数混合_MODULATE4X*/       = 6,      // multiply and  2 bits
		// Add
		TextureBlend_ADD          =  7,   // add arguments together
		TextureBlend_ADDSIGNED        =  8,   // add with -0.5 bias
		TextureBlend_ADDSIGNED2X       =  9,   // as above but left  1 bit
		TextureBlend_SUBTRACT         = 10,   // Arg1 - Arg2, with no saturation
		TextureBlend_ADDSMOOTH      = 11,   // add 2 args, subtract product
		// Arg1 + Arg2 - Arg1*Arg2
		// = Arg1 + (1-Arg1)*Arg2
		// Linear alpha blend: Arg1*(Alpha) + Arg2*(1-Alpha)
		TextureBlend_BLENDDIFFUSEALPHA    = 12, // iterated alpha
		TextureBlend_BLENDTEXTUREALPHA    = 13, // texture alpha
		TextureBlend_BLENDFACTORALPHA     = 14, // alpha from D3DRS_TEXTUREFACTOR
		// Linear alpha blend with pre-multiplied arg1 input: Arg1 + Arg2*(1-Alpha)
		TextureBlend_BLENDTEXTUREALPHAPM  = 15, // texture alpha
		TextureBlend_BLENDCURRENTALPHA    = 16, // by alpha of current color
		// Specular mapping
		TextureBlend_PREMODULATE           = 17,     // modulate with next texture before use
		TextureBlend_MODULATEALPHA_ADDCOLOR = 18,     // Arg1.RGB + Arg1.A*Arg2.RGB
		// COLOROP only
		TextureBlend_MODULATECOLOR_ADDALPHA= 19,     // Arg1.RGB*Arg2.RGB + Arg1.A
		// COLOROP only
		TextureBlend_MODULATEINVALPHA_ADDCOLOR = 20,  // (1-Arg1.A)*Arg2.RGB + Arg1.RGB
		// COLOROP only
		TextureBlend_MODULATEINVCOLOR_ADDALPHA = 21,  // (1-Arg1.RGB)*Arg2.RGB + Arg1.A
		// COLOROP only
		// Bump mapping
		TextureBlend_BUMPENVMAP           = 22, // per pixel env map perturbation
		TextureBlend_BUMPENVMAPLUMINANCE  = 23, // with luminance channel
		// This can do either diffuse or specular bump mapping with correct input.
		// Performs the function (Arg1.R*Arg2.R + Arg1.G*Arg2.G + Arg1.B*Arg2.B)
		// where each component has been scaled and offset to make it signed.
		// The result is replicated into all four (including alpha) channels.
		// This is a valid COLOROP only.
		TextureBlend_DOTPRODUCT3          = 24,
		// Triadic ops
		TextureBlend_MULTIPLYADD          = 25, // Arg0 + Arg1*Arg2
		TextureBlend_LERP                 = 26, // (Arg0)*Arg1 + (1-Arg0)*Arg2
	};

	//RenderModeV_ColorUV
	//RenderModeV_NormalColor,
	//RenderModeV_NormalUV,
	//RenderModeV_NormalColorUV,
	enum TextureIndex{
		TextureIndex_0	=	0,
		TextureIndex_1,
		TextureIndex_2,
		TextureIndex_3,
		TextureIndex_4,
		TextureIndex_5,
		TextureIndex_Max,
	};
	enum TextureFormat{
		FMT_R8G8B8              =	0,
		FMT_A8R8G8B8            =	1,
		TextureFormat_Max		=	2,
	};
	/*ENUM*/enum AlphaMode/*透明混合模式*/
	{
		AlpahMode_Unknown	=	-1,
		AlphaMode_None	/*透明混合模式_关闭*/=	0,
		AlphaMode_Test /*透明混合模式_测试模式*/ = 1,
		AlphaMode_Blend /*透明混合模式_混合模式*/ = 2,
		AlphaMode_Count = 3,
	};
	/*ENUM*/enum DrawTextAlign/*文本位置_多选*/
	{
		DrawTextAlign_TOP				/*文本位置_上对齐*/=	0x00000000,
		DrawTextAlign_LEFT				/*文本位置_左对齐*/=	0x00000000,
		DrawTextAlign_CENTER			/*文本位置_横向中间对齐*/=	0x00000001,
		DrawTextAlign_RIGHT				/*文本位置_右对齐*/=	0x00000002,
		DrawTextAlign_VCENTER			/*文本位置_纵向中间对齐*/=	0x00000004,
		DrawTextAlign_BOTTOM			/*文本位置_下对齐*/=	0x00000008,
		DrawTextAlign_WORDBREAK			/*文本位置_自动换行*/=	0x00000010,
		DrawTextAlign_SINGLELINE		/*文本位置_仅一行*/=	0x00000020,
		DrawTextAlign_EXPANDTABS		/*文本位置_TAB键展开*/=	0x00000040,
		DrawTextAlign_TABSTOP			/*文本位置_TAB停止*/=	0x00000080,
		DrawTextAlign_NOCLIP			/*文本位置_忽略输出矩形区域*/=	0x00000100,
		DrawTextAlign_CALCRECT			/*文本位置_计算举行区域*/=	0x00000400,
	};
	/*ENUM*/enum ActionType/*单位动作类型*/
	{
		ActionType_None			/*单位动作类型_无*/				=	0,
		ActionType_Idle			/*单位动作类型_默认*/			=	1,
		ActionType_Walk			/*单位动作类型_走路*/			=	2,
		ActionType_Run			/*单位动作类型_跑步*/			=	3,
		ActionType_Down			/*单位动作类型_掉落*/			=	4,
		ActionType_Jump			/*单位动作类型_跳跃*/			=	5,
		ActionType_Hit		/*单位动作类型_被攻击*/				=	6,
		ActionType_Die			/*单位动作类型_死亡*/			=	7,
		ActionType_Attack			/*单位动作类型_攻击*/		=	8,
		ActionType_Hide			/*单位动作类型_隐藏*/			=	9,
		ActionType_Idle2			/*单位动作类型_默认2*/		=	10,
		ActionType_Walk2			/*单位动作类型_走路2*/		=	11,
		ActionType_Run2			/*单位动作类型_跑步2*/			=	12,
		ActionType_Jump2			/*单位动作类型_跳跃2*/		=	13,
		ActionType_Hit2		/*单位动作类型_被攻击2*/				=	14,
		ActionType_Die2			/*单位动作类型_死亡2*/			=	15,
		ActionType_Attack2			/*单位动作类型_攻击2*/		=	16,
		ActionType_Idle3			/*单位动作类型_默认3*/		=	17,
		ActionType_Walk3			/*单位动作类型_走路3*/		=	18,
		ActionType_Run3			/*单位动作类型_跑步3*/			=	19,
		ActionType_Jump3			/*单位动作类型_跳跃3*/		=	20,
		ActionType_Hit3		/*单位动作类型_被攻击3*/				=	21,
		ActionType_Die3			/*单位动作类型_死亡3*/			=	22,
		ActionType_Attack3			/*单位动作类型_攻击3*/		=	23,
		ActionType_Other1			/*单位动作类型_其他1*/		=	24,
		ActionType_Other2			/*单位动作类型_其他2*/		=	25,
		ActionType_Other3			/*单位动作类型_其他3*/		=	26,
		ActionType_Other4			/*单位动作类型_其他4*/		=	27,
		ActionType_Other5			/*单位动作类型_其他5*/		=	28,
		ActionType_Other6			/*单位动作类型_其他6*/		=	29,
		ActionType_Other7			/*单位动作类型_其他7*/		=	30,
		ActionType_Other8			/*单位动作类型_其他8*/		=	31,
		ActionType_Other9			/*单位动作类型_其他9*/		=	32,
		ActionType_Other10			/*单位动作类型_其他10*/		=	33,
		ActionType_Count			/*单位动作类型_数量*/		=	34,
		ActionType_Max	=	6,//每个类型最多动作叠加数量,比如最多休闲动作6种
	};
	extern	const char* GetActionTypeName(int t);
	/*ENUM*/enum UIProgressType/*界面进度条*/
	{
		UIProgressType_HORZ		/*界面进度条_横向*/=	0,
		UIProgressType_VERT		/*界面进度条_纵向*/=	1,
		UIProgressType_SKILL		/*界面进度条_技能*/=	2,
	};

	enum UIButtonType
	{
		BUTTON_TYPE_NORMAL		/*界面按钮_正常*/=	0,
		BUTTON_TYPE_OK			/*界面按钮_默认*/=	1,
		BUTTON_TYPE_CANCEL		/*界面按钮_取消*/=	2,
	};

	/*ENUM*/enum UIPopupType/*界面弹出位置_多选*/
	{
		UIPopupType_LEFT			/*界面弹出位置_左*/=	0,
		UIPopupType_TOP			/*界面弹出位置_上*/=	1,
		UIPopupType_RIGHT			/*界面弹出位置_右*/=	2,
		UIPopupType_BOTTOM		/*界面弹出位置_下*/=	3,
		UIPopupType_AUTO			/*界面弹出位置_无*/=	4,
		UIPopupType_LEFT_TOP		/*界面弹出位置_左上*/=	5,
		UIPopupType_RIGHT_TOP		/*界面弹出位置_右上*/=	6,
		UIPopupType_LEFT_BOTTOM	/*界面弹出位置_左下*/=	7,
		UIPopupType_RIGHT_BOTTOM	/*界面弹出位置_右下*/=	8,
	};
	/*ENUM*/enum UIControlType/*控件类型*/
	{
		UIControlType_NONE				/*控件类型_无*/=	0,
		UIControlType_BUTTON			/*控件类型_按钮*/=	1,
		UIControlType_CHECKBOX			/*控件类型_单选框*/=	2,
		UIControlType_COMBOBOX			/*控件类型_组合框*/=	4,
		UIControlType_SLIDER			/*控件类型_Slider*/=	5,
		UIControlType_EDITBOX			/*控件类型_编辑框*/=	6,
		UIControlType_IMEEDITBOX		/*控件类型_带有IME的编辑框*/=	7,
		UIControlType_LISTBOX			/*控件类型_列表框*/=	8,
		UIControlType_SCROLLBAR			/*控件类型_滚动条*/=	9,
		UIControlType_PROGRESS			/*控件类型_进度条*/=	10,
		UIControlType_STATIC			/*控件类型_文本框*/=	11,
		UIControlType_FORMAT_EDITBOX	/*控件类型_格式化编辑框*/=	14,	//格式化输入框
		UIControlType_VIEW_FORMAT_TEXT	/*控件类型_格式化文本框*/=	15,	//格式化显示框
		UIControlType_LISTCTRL3D		/*控件类型_行列框*/=	16,	//格式化显示框
		UIControlType_ui2DEffect			/*控件类型_2D特效*/	=	19,	//
	};
	enum Unit2DAngle//8个方向，如果是平面游戏，只需要设置2个方向，左右就可以
	{
		Unit2DAngle_Bottom	=	0,
		Unit2DAngle_LeftBottom,	//左下
		Unit2DAngle_Left,	//左
		Unit2DAngle_LeftTop,
		Unit2DAngle_Top,
		Unit2DAngle_RightTop,
		Unit2DAngle_Right,
		Unit2DAngle_RightBottom,
		Unit2DAngle_Count,
	};
	enum ControlTrackType{
		ControlTrackType_TextColorR	=	0,
		ControlTrackType_TextColorG,
		ControlTrackType_TextColorB,
		ControlTrackType_TextColorA,
		ControlTrackType_TextureColorR,
		ControlTrackType_TextureColorG,
		ControlTrackType_TextureColorB,
		ControlTrackType_TextureColorA,
		ControlTrackType_RectX,
		ControlTrackType_RectY,
		ControlTrackType_RectW,
		ControlTrackType_RectH,
		ControlTrackType_CenterX,
		ControlTrackType_CenterY,
		ControlTrackType_Rotation,
		//
		ControlTrackType_SoundVolume,
		ControlTrackType_UStart,
		ControlTrackType_SoundSpeed,
		ControlTrackType_UEnd,
		ControlTrackType_VStart,
		ControlTrackType_VEnd,
		ControlTrackType_Count,
	};
	/*ENUM*/enum UIDialogAlign/*对话框对齐方式*/
	{
		UIDialogAlign_NONE	/*对话框对齐方式_无*/		=	0,
		UIDialogAlign_LEFT	/*对话框对齐方式_左对齐*/	=	1,
		UIDialogAlign_CENTER	/*对话框对齐方式_横向中对齐*/	=	2,
		UIDialogAlign_RIGHT	/*对话框对齐方式_右对齐*/	=	4,
		UIDialogAlign_TOP	/*对话框对齐方式_上对齐*/	=	8,
		UIDialogAlign_BOTTOM	/*对话框对齐方式_下对齐*/	=	16,
		UIDialogAlign_VCENTER/*对话框对齐方式_纵向中对齐*/	=	32,
	};
	enum ImportResult{
		ImportResult_FailureAuth		=	-6,	//授权码错误
		ImportResult_VersionLow			=	-5,	//程序版本太低，不能打开高级版本文件
		ImportResult_FailurePassword	=	-4,	//密码错误
		ImportResult_ReadFailure		=	-3,//读取数据错误
		ImportResult_OpenFailure		=	-2,//打开文件错误
		ImportResult_HeadFailure		=	-1,//文件头错误
		ImportResult_RequestPassword	=	0,//需要密码
		ImportResult_OK					=	1,//正确返回
	};
	enum TrackStatus{
		TrackStatus_Pause	=	0,
		TrackStatus_Play,
		TrackStatus_QuickPlay,
		TrackStatus_PlayBack,
		TrackStatus_QuickPlayBack,
		TrackStatus_Stop,
	};
	/*ENUM*/enum PlayAnimationType/*动画播放方式*/
	{
		PlayAnimationType_None		/*动画播放方式_不自动播放*/=	0,
		PlayAnimationType_Play		/*动画播放方式_自动向前播放*/=	1,
		PlayAnimationType_PlayBack	/*动画播放方式_自动向后播放*/=	2,
	};
	/*ENUM*/enum UISoundPlay/*界面声音格式*/
	{
		UISoundPlay_mouseIn		/*界面声音格式_鼠标进入*/=	0,
		UISoundPlay_buttonDown	/*界面声音格式_按钮按下*/=	1,
		UISoundPlay_count			/*界面声音格式_数量*/		=	10,
	};
	/*ENUM*/enum EventID/*事件类型*/
	{
		EventID_None					=	0,
		//
		EventID_Click			/*界面事件_单击*/=	1,//在bind_eventby中文本参数是onclick
		EventID_Down			/*界面事件_按下*/=	2,//在bind_eventby中文本参数是ondown
		EventID_Up				/*界面事件_弹起*/=	3,//onup
		EventID_Move			/*界面事件_移动*/=	4,//在bind_eventby中文本参数是onmove

		EventID_DoubleClick		/*界面事件_双击*/=	8,//在bind_eventby中文本参数是ondblclick
		EventID_Changed			/*界面事件_值变动*/=	9,//在bind_eventby中文本参数是onchanged,如果是按钮此功能有效，在按下之后持续按下状态下会引发此事件，只要是持续按下就会一直逐渐加速触发，触发速度最高会到每秒20次引发
		EventID_PlayOver		/*界面事件_播放完毕*/=	10,//在bind_eventby中文本参数是onplayover,界面控制动画(并非UV动画)播放完毕会被调用
		EventID_UVPlayOver		/*界面事件_UV动画播放完毕*/=	11,//在bind_eventby中文本参数是onplayoveruv界面UV动画播放完毕会被调用
	};
		//EventID_PinchOpen		/*界面事件_放大*/=	5,//onpinchopen
		//EventID_PinchClose		/*界面事件_缩小*/=	6,//onpinchclose
		//EventID_LongPress		/*界面事件_长按*/=	7,//onlongpress
		//EventID_Shake			/*界面事件_摇晃*/=	8,//onshake
		//EventID_UIFormatTextEventID	=	21,	/*界面事件_格式化文本框事件点击*/
	enum InputEventID{
		InputEventID_MouseMove	=	1,
		InputEventID_ButtonDown,
		InputEventID_ButtonDblClk,
		InputEventID_ButtonUp,
		InputEventID_Wheel,
		InputEventID_Return,
	};
	enum EffectRandom{
		EffectRandom_TextureColorMin	=	0,
		EffectRandom_TextureColorMax,
		EffectRandom_OffsetMin,
		EffectRandom_OffsetMax,
		EffectRandom_SizeMin,
		EffectRandom_SizeMax,
		EffectRandom_RotationMin,
		EffectRandom_RotationMax,
		EffectRandom_LifeMin,
		EffectRandom_LifeMax,
		EffectRandom_EmitCount,
		EffectRandom_Count,
	};
	/*ENUM*/enum CullMode/*渲染面选择*/
	{
		CullMode_UNKNOWN			 = 0,
		CullMode_Double        /*渲染面选择_双面*/        = 1,//正面和背面都渲染,也就是双面渲染
		CullMode_CW          /*渲染面选择_正面*/        = 2,//只渲染正面
		CullMode_CCW         /*渲染面选择_背面*/        = 3,//只渲染背面
		CullMode_Count			 = 4,
	};
	/*ENUM*/enum BlendMode/*混合模式*/
	{
		BlendMode_UNKNOWN			 = 0,
		BlendMode_ZERO              /*混合模式_ZERO           */ = 1,
		BlendMode_ONE               /*混合模式_ONE            */ = 2,
		BlendMode_SRCCOLOR          /*混合模式_SRCCOLOR       */ = 3,
		BlendMode_INVSRCCOLOR       /*混合模式_INVSRCCOLOR    */ = 4,
		BlendMode_SRCALPHA          /*混合模式_SRCALPHA       */ = 5,
		BlendMode_INVSRCALPHA       /*混合模式_INVSRCALPHA    */ = 6,
		BlendMode_DESTALPHA         /*混合模式_DESTALPHA      */ = 7,
		BlendMode_INVDESTALPHA      /*混合模式_INVDESTALPHA   */ = 8,
		BlendMode_DESTCOLOR         /*混合模式_DESTCOLOR      */ = 9,
		BlendMode_INVDESTCOLOR      /*混合模式_INVDESTCOLOR   */ = 10,
		BlendMode_SRCALPHASAT       /*混合模式_SRCALPHASAT    */ = 11,
		BlendMode_BOTHSRCALPHA      /*混合模式_BOTHSRCALPHA   */ = 12,
		BlendMode_BOTHINVSRCALPHA   /*混合模式_BOTHINVSRCALPHA*/ = 13,
		BlendMode_Count			 = 14,
	};
	enum ResultValue
	{
		Result_Fail	=	-1,
		Result_OK	=	0,
	};

	///

	enum ClearType
	{
		//清空缓冲区选项***************************************************************************************************************************************
		CLEAR_TARGET            =	0x00000001l,  // 清除目标缓冲 
		CLEAR_ZBUFFER           =	0x00000002l,  // 清除深度缓冲 
		CLEAR_STENCIL           =	0x00000004l,  // 清除模板缓冲 
	};
	enum SamplerState
	{
		//***************************************************************************************************************************************
		SAMP_MAGFILTER      = 5,  /* D3DTEXTUREFILTER filter to use for magnification */
		SAMP_MINFILTER      = 6,  /* D3DTEXTUREFILTER filter to use for minification */
		SAMP_MIPFILTER      = 7,  /* D3DTEXTUREFILTER filter to use between mipmaps during minification */
		SamplerState_Max	= 8,
	};
	enum TextureFilter
	{
		//***************************************************************************************************************************************
		TextureFilter_Point           = 1,    // nearest
		TextureFilter_Linear          = 2,    // linear interpolation
		TextureFilter_Max	 = 3,    // 4-sample tent
	};

	/*ENUM*/enum MeshShaderType/*模型渲染方案*/
	{
		MeshShaderType_Default		/*模型渲染方案_默认*/	=	0,	//默认
		MeshShaderType_Extend1		/*模型渲染方案_扩展1*/	=	1,
		MeshShaderType_Extend2		/*模型渲染方案_扩展2*/	=	2,
		MeshShaderType_Extend3		/*模型渲染方案_扩展3*/	=	3,
		MeshShaderType_Extend4		/*模型渲染方案_扩展4*/	=	4,
		MeshShaderType_Extend5		/*模型渲染方案_扩展5*/	=	5,
		MeshShaderType_Extend6		/*模型渲染方案_扩展6*/	=	6,
		MeshShaderType_Extend7		/*模型渲染方案_扩展7*/	=	7,
		MeshShaderType_Extend8		/*模型渲染方案_扩展8*/	=	8,
		MeshShaderType_Select		/*模型渲染方案_选择*/	=	9,
		MeshShaderType_Count		/*模型渲染方案_数量*/	=	10,
	};
	enum TEX_ID{
		TEX_DIFFUSE				=	0,	//漫射光
		TEX_SELF_ILLUMINATION	,	//自发光
		TEX_SPECULAR_MAP		,	//RGB高光贴图
		TEX_NORMAL_MAP			,	//法线贴图
		TEX_REFLECTION			,	//反射贴图
		TEX_SHADOWMAP			,	//阴影贴图
		TEXTURE_MAX_COUNT,
	};

	enum{
		RenderNode_SetState			=	0x01,	//设置状态
		RenderNode_SelfShadowOnly	=	0x02,	//仅自身阴影有效的模型
		RenderNode_NoSelfShadowOnly	=	0x04,	//仅自身阴影无效的模型
	};

	/*ENUM*/enum SoundType/*声效类型*/
	{
		SoundType_2D_MP3/*声效类型_MP3_2D*/				=	0,//无3D声效状态下的声音文件
		SoundType_3D_MP3/*声效类型_MP3_3D*/	=	1,
		SoundType_SOUND3D/*声效类型_WAV_3D*/	=	2,
		SoundType_SOUND2D/*声效类型_WAV_2D*/	=	3,
		SoundType_3D_VIDEO/*声效类型_视频_3D*/	=	4,
		SoundType_2D_VIDEO/*声效类型_视频_2D*/	=	5,
	};
	
	/*ENUM*/enum MatrixBoard/*矩阵蒙版*/
	{
		MatrixBoard_Billbard	/*矩阵蒙版_XY轴上面向镜头*/	=	0,//返回XY轴上始终面向用户的矩阵
		MatrixBoard_CameraBoard	/*矩阵蒙版_面向镜头*/	=	1,//返回始终面向用户的矩阵
		MatrixBoard_ZBoard		/*矩阵蒙版_面向上方*/	=	2,//返回Z轴向上矩阵
		BOARD_COUNT	=	3,
	};

};

#endif

