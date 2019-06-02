// Copyright 2009-2014 Blam Games, Inc. All Rights Reserved.

#pragma once

#include "./GameDevClasses/BaseApp.h"
#include "./tools/Types.hpp"


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
	void initGameField();
	void printStatusField();
	void printScoreField();
	void printRectangleBoudary(const uint8_t x1, const uint8_t y1, const uint8_t x2, const uint8_t  y2);
	void printLine(const uint8_t x1, const uint8_t y1, const uint8_t x2, const uint8_t  y2);

	void printScoreTitle(const wchar_t* line);
	void changeScore();
	void printScore();

	bool checkCollision();
	bool checkCollisionForPoint();
	void processingCollision();

	void generateNextObject();
	void changeCurrentObject();
	void moveObject();
	void checkFullLine(uint8_t size);
	void clearLine(const uint8_t y);
	void makeLinesOffset(const uint8_t y);
	
	void checkGameOver();
	void checkOverrun();
	Item generateItem(NTetris::E_FIG_TYPE ) const;
	void drawItem(const NTetris::T_OBJECT& object);
	void wipeOffItem(const NTetris::T_OBJECT& object);
	void rotate();
	bool hasCollisions();
	uint32_t findSumOfLine(uint8_t y);
	NTetris::E_FIG_TYPE getRandomType() const;
	void changeOrientation();


	void printLinesForTesting();// Test
	

private:
	NTetris::T_OBJECT mObject;
	NTetris::T_OBJECT mNextObject;
	NTetris::T_OBJECT mObjectOld;
	bool mPressedButtonS;
	bool mPressedButtonSpase;
	float mDelayTime;
	float mLastRotateTime;
	uint32_t mScore;
	wchar_t mNextSymbolY;
};
