// Copyright 2009-2014 Blam Games, Inc. All Rights Reserved.

#pragma once

#include <iostream>
#include <Windows.h>
#include <vector>
#include <string>

using namespace std;

class BaseApp
{
private:
	HANDLE mConsole;
	HANDLE mConsoleIn;

	CHAR_INFO* mChiBuffer;
	COORD mDwBufferSize;
	COORD mDwBufferCoord;
	SMALL_RECT mLpWriteRegion;

	void Render();

public:	
	const int X_SIZE;
	const int Y_SIZE;

    BaseApp(int xSize=100, int ySize= 80);
	virtual ~BaseApp();

	void Run();	
	void SetChar(int x, int y, wchar_t c);
	wchar_t GetChar(int x, int y);	
	virtual void UpdateF (float deltaTime){}	
	virtual void KeyPressed (int btnCode){}
};
