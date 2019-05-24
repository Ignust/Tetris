// Copyright 2009-2014 Blam Games, Inc. All Rights Reserved.

#pragma once

#include "./GameDevClasses/BaseApp.h"
#include "./tools/Types.hpp"

//typedef vector< vector < int > > Item;

class Tetris : public BaseApp
{
	typedef BaseApp Parent;

public:
	Tetris();

private:
	virtual void KeyPressed(int btnCode);
	virtual void UpdateF(float deltaTime);

	void initFields();
	void printGameField();
	void printStatusField();
	void printScoreField();
	void printRectangleBoudary(const uint8_t x1, const uint8_t y1, const uint8_t x2, const uint8_t  y2);
	void printLine(const uint8_t x1, const uint8_t y1, const uint8_t x2, const uint8_t  y2);

	void printScoreTitle(const wchar_t* line);
	void changeScore();
	void printScore();

	bool checkCollision();
	void processingCollision();

	void generateNextObject();
	void changeCurrentObject();
	void moveObject();
	void checkFullLine();
	void clearLine(const uint8_t y);
	void makeLinesOffset(const uint8_t y);
	
	void checkGameOver();
	void checkOverrun();
	Item generateRandom();
	void drawItem(const NTetris::T_OBJECT& object);
	void wipeOffItem(const NTetris::T_OBJECT& object);
	void rotate();


	void printLinesForTesting();
	

private:
	NTetris::T_OBJECT mObject;
	NTetris::T_OBJECT mNextObject;
	NTetris::T_OBJECT mObjectOld;
	bool mPressedButtonS;
	float mDelayTime;
	uint32_t mScore;
	//Item mItem;
	
};
