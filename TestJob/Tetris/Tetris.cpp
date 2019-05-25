// Copyright 2009-2014 Blam Games, Inc. All Rights Reserved.

#include<ctime>
#include<cstdlib>

#include "./Tetris.hpp"

//-----------------------------------------------------------------------------
Tetris::Tetris()
    : Parent(MAIN_WINDOWS_X_SIZE, MAIN_WINDOWS_Y_SIZE)
	, mObject()
	, mNextObject()
	, mObjectOld()
	, mPressedButtonS(false)
	, mDelayTime(0)
	, mScore(0)
	//, mItem(SPACE_SYMBOL)
//-----------------------------------------------------------------------------
{
	generateNextObject();
	changeCurrentObject();
	initFields();
}

//-----------------------------------------------------------------------------
void Tetris::KeyPressed(int btnCode)
//-----------------------------------------------------------------------------
{
    switch (btnCode) {
	case NTetris::Left_Button:
	case NTetris::A_Button:
		mObject.posX--;
        break;	
	case NTetris::Down_Button:
	case NTetris::S_Button:
		mPressedButtonS = true;
        break;
	case NTetris::Right_Button:
    case NTetris::D_Button:
		mObject.posX++;
        break;
	case NTetris::Auto_Button:
		mObject.posY++;
		break;
	case NTetris::Spase_Button:
		//orientation
		rotate();
		break;
	default:
		//
		break;
	}

	checkOverrun();
}

//-----------------------------------------------------------------------------
void Tetris::UpdateF(float deltaTime)
//-----------------------------------------------------------------------------
{
	mDelayTime += deltaTime;
	if (mPressedButtonS) {
		KeyPressed(NTetris::Auto_Button);
	} else if (mDelayTime > 1) {
		KeyPressed(NTetris::Auto_Button);
		mDelayTime = 0;
	}
    if (checkCollision()) {
		processingCollision();
    } else {
		moveObject();
		mObjectOld = mObject;
    }
	//moveObject();
	//mObjectOld = mObject;
}

//-----------------------------------------------------------------------------
void Tetris::generateNextObject()
//-----------------------------------------------------------------------------
{
	wipeOffItem(mNextObject);

	mNextObject.type = NTetris::POINT;		// TODO: add random type 
	mNextObject.orientation = NTetris::UP;	// TODO: add random orientation 
	mNextObject.posX = STATUS_FIELD_X_START + STATUS_FIELD_X_SIZE/2-1;
	mNextObject.posY = STATUS_FIELD_Y_START + STATUS_FIELD_Y_SIZE/2;
	mNextObject.mItem = generateRandom();

	drawItem(mNextObject);
	//SetChar(mNextObject.posX, mNextObject.posY, OBJECT_SYMBOL);
}
//-----------------------------------------------------------------------------
void Tetris::changeCurrentObject()
//-----------------------------------------------------------------------------
{
	checkGameOver();

	mObject = mNextObject;
	mObject.posX = GAME_FIELD_X_START+GAME_FIELD_X_FINISH/2;
	mObject.posY = GAME_FIELD_Y_START;

	mObjectOld = mObject;
	generateNextObject();
}

//-----------------------------------------------------------------------------
void Tetris::initFields()
//-----------------------------------------------------------------------------
{
	printGameField();
	printStatusField();
	printScoreField();
}

//-----------------------------------------------------------------------------
void Tetris::printGameField()
//-----------------------------------------------------------------------------
{
	printRectangleBoudary(GAME_FIELD_X_START, GAME_FIELD_Y_START, GAME_FIELD_X_FINISH, GAME_FIELD_Y_FINISH);
	printLinesForTesting();
}

//-----------------------------------------------------------------------------
void Tetris::printStatusField()
//-----------------------------------------------------------------------------
{
	printRectangleBoudary(STATUS_FIELD_X_START, STATUS_FIELD_Y_START, STATUS_FIELD_X_FINISH, STATUS_FIELD_Y_FINISH);
}

//-----------------------------------------------------------------------------
void Tetris::printScoreField()
//-----------------------------------------------------------------------------
{
	printRectangleBoudary(SCORE_FIELD_X_START, SCORE_FIELD_Y_START, SCORE_FIELD_X_FINISH, SCORE_FIELD_Y_FINISH);
	printScoreTitle(L">Score:");
}

//-----------------------------------------------------------------------------
void Tetris::printRectangleBoudary(const uint8_t x1, const uint8_t y1, const uint8_t x2, const uint8_t  y2)
//-----------------------------------------------------------------------------
{
	printLine(x1 - 1, y1 - 1, x2 + 1 , y1 - 1); // TOP LINE
	printLine(x1 - 1, y2 + 1, x2 + 1 , y2 + 1); // DOWN LINE
	printLine(x1 - 1, y1 - 1, x1 - 1, y2 + 1);  // LEFT LINE
	printLine(x2 + 1, y1 - 1, x2 + 1, y2 + 1);  // RIGHT LINE
}

//-----------------------------------------------------------------------------
void Tetris::printLine(const uint8_t x1, const uint8_t y1, const uint8_t x2, const uint8_t  y2)
//-----------------------------------------------------------------------------
{
	if (x1 == x2) {
		for (uint8_t i = y1; i <= y2; ++i) {
			SetChar(x1, i, BOUNDARY_SYMBOL);
		}
	}
	else if (y1 == y2) {
		for (uint8_t i = x1; i <= x2; ++i) {
			SetChar(i, y1, BOUNDARY_SYMBOL);
		}
	}
}

//-----------------------------------------------------------------------------
void Tetris::moveObject()
//-----------------------------------------------------------------------------
{
	wipeOffItem(mObjectOld);
	//drawItem(mObjectOld);
	drawItem(mObject);
	//SetChar(mObjectOld.posX, mObjectOld.posY, SPACE_SYMBOL);
	//SetChar(mObject.posX, mObject.posY, OBJECT_SYMBOL);
}

//-----------------------------------------------------------------------------
bool Tetris::checkCollision()
//-----------------------------------------------------------------------------
{
	/*return (mObject.posY == GAME_FIELD_Y_FINISH)
			|| (GetChar(mObject.posX, mObject.posY + 1) == OBJECT_SYMBOL);*/
			wipeOffItem(mObjectOld);
	NTetris::T_OBJECT tempObject = mObject;
	tempObject.posY++;
	for (uint8_t y = 0; y < tempObject.mItem.size(); ++y) {
		for (uint8_t x = 0; x < tempObject.mItem.size(); ++x) {
			if (tempObject.mItem[y][x] == OBJECT_SYMBOL) {
				if (GetChar(tempObject.posX + x, tempObject.posY + y) != SPACE_SYMBOL) {
					return true;
				}
			}
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
void Tetris::processingCollision()
//-----------------------------------------------------------------------------
{
	moveObject();
	checkFullLine(mObject.mItem.size());
	changeCurrentObject();
	mPressedButtonS = false;
}

//-----------------------------------------------------------------------------
void Tetris::checkFullLine(uint8_t size)
//-----------------------------------------------------------------------------
{	
	while (size > 0)
	{
		uint16_t sum = 0;
		for (uint8_t i = GAME_FIELD_X_START; i <= GAME_FIELD_X_FINISH; ++i) {
			sum += GetChar(i, mObject.posY + size - 1);
		}
		if (sum >= FULL_LINE_X) {
			changeScore();
			clearLine(mObject.posY + size - 1);
			//makeLinesOffset(mObject.posY + size - 1);
		}
		size--;
	}
	
}

//-----------------------------------------------------------------------------
void Tetris::clearLine(const uint8_t y)
//-----------------------------------------------------------------------------
{
	for (uint8_t i = GAME_FIELD_X_START; i <= GAME_FIELD_X_FINISH; ++i) {
		SetChar(i, y, SPACE_SYMBOL);
	}
}

//-----------------------------------------------------------------------------
void Tetris::makeLinesOffset(const uint8_t y)
//-----------------------------------------------------------------------------
{
	wchar_t tempChar = SPACE_SYMBOL;
	wchar_t currentChar = 0;
	for (uint8_t i = GAME_FIELD_X_START; i <= GAME_FIELD_X_FINISH; ++i) {
		for (uint8_t b = GAME_FIELD_Y_START; b <= y; ++b) {
			currentChar =  GetChar(i, b);
			SetChar(i, b, tempChar);
			tempChar = currentChar;
		}
	}
}

//-----------------------------------------------------------------------------
void Tetris::printScoreTitle(const wchar_t* line)
//-----------------------------------------------------------------------------
{
	if (line == nullptr) {
		return;
	}

	const uint8_t length = wcslen(line);
	const uint8_t shift_X = SCORE_FIELD_X_START + 2;
	const uint8_t shift_Y = SCORE_FIELD_Y_START + SCORE_FIELD_Y_SIZE / 2;

	for (uint8_t i = 0; i < length; ++i) {
		SetChar(shift_X + i, shift_Y, line[i]);
	}
}

//-----------------------------------------------------------------------------
void Tetris::printScore()
//-----------------------------------------------------------------------------
{
	const uint8_t size = 10;
	wchar_t scoreString[size] = {};
	swprintf_s(scoreString, L"%d", mScore);

	const uint8_t length = wcslen(scoreString);
	const uint8_t shift_X = SCORE_FIELD_X_START + 10;
	const uint8_t shift_Y = SCORE_FIELD_Y_START + SCORE_FIELD_Y_SIZE / 2;

	for (uint8_t i = 0; i < length; ++i) {
		SetChar(shift_X + i, shift_Y, scoreString[i]);
	}
}

//-----------------------------------------------------------------------------
void Tetris::checkGameOver()
//-----------------------------------------------------------------------------
{
	const uint8_t objectStartPosX = GAME_FIELD_X_START + GAME_FIELD_X_FINISH / 2;
	const uint8_t objectStartPosY = GAME_FIELD_Y_START;
	if (GetChar(objectStartPosX, objectStartPosY) == OBJECT_SYMBOL) {
		exit(0);
	}
}

//-----------------------------------------------------------------------------
void Tetris::changeScore()
//-----------------------------------------------------------------------------
{
	mScore += 100;
	printScore();
}

//-----------------------------------------------------------------------------
void Tetris::checkOverrun()
//-----------------------------------------------------------------------------
{
	/*if (mObject.posX < GAME_FIELD_X_START) {
		mObject.posX = GAME_FIELD_X_START;
	}
	else if (mObject.posX >= GAME_FIELD_X_FINISH) {
		mObject.posX = GAME_FIELD_X_FINISH;
	}
	if (mObject.posY >= GAME_FIELD_Y_FINISH) {
		mObject.posY = GAME_FIELD_Y_FINISH;
	}*/
	if (hasCollisions()) {
		mObject = mObjectOld;
	}
		
}

//-----------------------------------------------------------------------------
Item Tetris::generateRandom()
//-----------------------------------------------------------------------------
{
	static const vector < Item > randomItem = {
		Item({
			{SPACE_SYMBOL, OBJECT_SYMBOL, SPACE_SYMBOL, SPACE_SYMBOL},
			{SPACE_SYMBOL, OBJECT_SYMBOL, SPACE_SYMBOL, SPACE_SYMBOL},
			{SPACE_SYMBOL, OBJECT_SYMBOL, SPACE_SYMBOL, SPACE_SYMBOL},
			{SPACE_SYMBOL, OBJECT_SYMBOL, SPACE_SYMBOL, SPACE_SYMBOL},}),
		Item({
			{OBJECT_SYMBOL, OBJECT_SYMBOL},
			{OBJECT_SYMBOL, OBJECT_SYMBOL},}),
		Item({
			{SPACE_SYMBOL, OBJECT_SYMBOL, OBJECT_SYMBOL},
			{OBJECT_SYMBOL, OBJECT_SYMBOL, SPACE_SYMBOL},
			{SPACE_SYMBOL, SPACE_SYMBOL, SPACE_SYMBOL},}),
		Item({
			{OBJECT_SYMBOL, OBJECT_SYMBOL, SPACE_SYMBOL},
			{SPACE_SYMBOL, OBJECT_SYMBOL, OBJECT_SYMBOL},
			{SPACE_SYMBOL, SPACE_SYMBOL, SPACE_SYMBOL},}),
		Item({
			{SPACE_SYMBOL, OBJECT_SYMBOL, SPACE_SYMBOL},
			{OBJECT_SYMBOL, OBJECT_SYMBOL, OBJECT_SYMBOL},
			{SPACE_SYMBOL, SPACE_SYMBOL, SPACE_SYMBOL},}),
		Item({
			{SPACE_SYMBOL, OBJECT_SYMBOL, SPACE_SYMBOL},
			{SPACE_SYMBOL, OBJECT_SYMBOL, SPACE_SYMBOL},
			{SPACE_SYMBOL, OBJECT_SYMBOL, OBJECT_SYMBOL},}),
		Item({
			{SPACE_SYMBOL, OBJECT_SYMBOL, SPACE_SYMBOL},
			{SPACE_SYMBOL, OBJECT_SYMBOL, SPACE_SYMBOL},
			{OBJECT_SYMBOL, OBJECT_SYMBOL, SPACE_SYMBOL},}),

	};
	srand(time(0));
	int rand = std::rand() % randomItem.size();
	return randomItem[rand];
}

//-----------------------------------------------------------------------------
void Tetris::drawItem(const NTetris::T_OBJECT& object)
//-----------------------------------------------------------------------------
{
	for (uint8_t y = 0; y < object.mItem.size(); ++y) {
		for (uint8_t x = 0; x < object.mItem.size(); ++x)
		{
			if (object.mItem[y][x] == OBJECT_SYMBOL) {
				SetChar(object.posX + x, object.posY + y, object.mItem[y][x]);
		}
			}
	}
}

//-----------------------------------------------------------------------------
void Tetris::wipeOffItem(const NTetris::T_OBJECT& object)
//-----------------------------------------------------------------------------
{
	for (uint8_t y = 0; y < object.mItem.size(); ++y) {
		for (uint8_t x = 0; x < object.mItem.size(); ++x) {
			if (object.mItem[y][x] == OBJECT_SYMBOL) {
				SetChar(object.posX + x, object.posY + y, SPACE_SYMBOL);
			}
		}
	}
}

//-----------------------------------------------------------------------------
void Tetris::rotate()
//-----------------------------------------------------------------------------
{
	Item tempItem(mObject.mItem.size());
	for (uint8_t y = 0; y < mObject.mItem.size(); ++y) {
		tempItem[y].resize(mObject.mItem.size());
		for (uint8_t x = 0; x < mObject.mItem.size(); ++x) {
			tempItem[y][x] = mObject.mItem[x][mObject.mItem.size() - y - 1];
		}
	}
	mObject.mItem = tempItem;
}

//-----------------------------------------------------------------------------
bool Tetris::hasCollisions()
//-----------------------------------------------------------------------------
{
	wipeOffItem(mObjectOld);
	for (uint8_t y = 0; y < mObject.mItem.size(); ++y) {
		for (uint8_t x = 0; x < mObject.mItem.size(); ++x) {
			if (mObject.mItem[y][x] == OBJECT_SYMBOL) {
				if (GetChar(mObject.posX + x, mObject.posY + y) != SPACE_SYMBOL) {					
					return true;
				}
			}
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
void Tetris::printLinesForTesting()
//-----------------------------------------------------------------------------
{
	const uint8_t empty_X_place = 8;
	const uint8_t numberOfLinesY = 6;
	for (uint8_t i = GAME_FIELD_Y_FINISH; i >= GAME_FIELD_Y_FINISH- numberOfLinesY; --i) {
		for (uint8_t j = GAME_FIELD_X_START; j <= GAME_FIELD_X_SIZE; ++j) {
			SetChar(j, i, OBJECT_SYMBOL);
		}
	}

	for (uint8_t i = GAME_FIELD_Y_FINISH; i >= GAME_FIELD_Y_FINISH - numberOfLinesY; --i) {
		SetChar(empty_X_place, i, SPACE_SYMBOL);
	}
}
