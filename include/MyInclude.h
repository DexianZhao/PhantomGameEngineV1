//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	文件       :   MyInclude.h
	幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

	Design Writer :   赵德贤 Dexian Zhao
	Email: yuzhou_995@hotmail.com
	 
	
	Copyright 2009-2016 Zhao Dexian

	-------------------------------------------------------------------------------------------------
	通用开源库使用说明：幻影游戏引擎使用的第三方库有pnglib,zlib,box2d
	-------------------------------------------------------------------------------------------------
	感谢box2d作者：
		Copyright (c) 2006-2010 Erin Catto http://www.gphysics.com
		This software is provided 'as-is', without any express or implied
		warranty.  In no event will the authors be held liable for any damages
		arising from the use of this software.

		Permission is granted to anyone to use this software for any purpose,
		including commercial applications, and to alter it and redistribute it
		freely, subject to the following restrictions:

		声明：本引擎对此开源库的使用仅在连接层上，并未对其进行任何修改。

	-------------------------------------------------------------------------------------------------
	感谢pnglib,zlib作者：
		http://www.libpng.org
		This code is released under the libpng license.
		libpng versions 1.2.6, August 15, 2004, through 1.5.10, March 29, 2012, are
		Copyright (c) 2004, 2006-2011 Glenn Randers-Pehrson, and are
		distributed according to the same disclaimer and license as libpng-1.2.5
		with the following individual added to the list of Contributing Authors
		Cosmin Truta
		libpng versions 1.0.7, July 1, 2000, through 1.2.5 - October 3, 2002, are
		Copyright (c) 2000-2002 Glenn Randers-Pehrson, and are
		distributed according to the same disclaimer and license as libpng-1.0.6
		with the following individuals added to the list of Contributing Authors

		Simon-Pierre Cadieux
		Eric S. Raymond
		Gilles Vollant

		and with the following additions to the disclaimer:

		There is no warranty against interference with your enjoyment of the
		library or against infringement.  There is no warranty that our
		efforts or the library will fulfill any of your particular purposes
		or needs.  This library is provided with all faults, and the entire
		risk of satisfactory quality, performance, accuracy, and effort is with
		the user.

		libpng versions 0.97, January 1998, through 1.0.6, March 20, 2000, are
		Copyright (c) 1998, 1999 Glenn Randers-Pehrson, and are
		distributed according to the same disclaimer and license as libpng-0.96,
		with the following individuals added to the list of Contributing Authors:

		Tom Lane
		Glenn Randers-Pehrson
		Willem van Schaik

		libpng versions 0.89, June 1996, through 0.96, May 1997, are
		Copyright (c) 1996, 1997 Andreas Dilger
		Distributed according to the same disclaimer and license as libpng-0.88,
		with the following individuals added to the list of Contributing Authors:

		John Bowler
		Kevin Bracey
		Sam Bushell
		Magnus Holmgren
		Greg Roelofs
		Tom Tanner

		libpng versions 0.5, May 1995, through 0.88, January 1996, are
		Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.

		For the purposes of this copyright and license, "Contributing Authors"
		is defined as the following set of individuals:

		Andreas Dilger
		Dave Martindale
		Guy Eric Schalnat
		Paul Schmidt
		Tim Wegner

		声明：本引擎对此开源库的使用仅在连接层上，并未对其进行任何修改。
		Disclaimer: This engine uses this open source library only on the connection layer and does not modify it.
	-------------------------------------------------------------------------------------------------
	-------------------------------------------------------------------------------------------------
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___MYINCLUDE__H___
#define ___MYINCLUDE__H___
enum OSType_Enum{
    OSType_IPhone = 0,
    OSType_IPad,
    OSType_Android,
	OSType_Windows,
};
int		OnAppCreate(const char* szAppPath, int nDivTexture, int nOSType, int* authcodes);
void	OnAppDestroy();
void	OnAppPause();
void	OnAppResume();
void	SetCharLocale(const char* szCode, int bIsUTF8);//chs[简体中文], en_US.UTF-8[uf8编码]
void	AddZipFile(const char* szZipFile, const char* szFirstDir);
//device
int		IsAppRequestCreateSDK();
void	OnAppCreateSDK();
void	OnAppInputEvent(int nEvent, int x, int y, unsigned int touchIndex);
void	OnAppResize(int ori, int w, int h, int deviceWidth, int deviceHeight);
int     OnAppFrameMove();
int		OnAppRender();
void	OnAppRestore();
void	OnAppLost();
double  GetAppTime();//
void    UpdateMotion(float gravityX,float gravityY,float gravityZ, float zTheta, float xyTheta, float rotationRateX, float rotationRateY,float rotationRateZ,
                     float roll, float pitch, float yaw);

#endif
