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
	, mPressedButtonSpase(false)
	, mDelayTime(0)
	, mLastRotateTime(0)
	, mScore(0)	
//-----------------------------------------------------------------------------
{
	initFields();
	generateNextObject();
	changeCurrentObject();
}

//-----------------------------------------------------------------------------
void Tetris::KeyPressed(int btnCode)
//-----------------------------------------------------------------------------
{
    switch (btnCode) {
	case NTetris::Left_Button:
	case NTetris::A_Button:
		--mObject.posX;
        break;	
	case NTetris::Down_Button:
	case NTetris::S_Button:
		mPressedButtonS = true;
        break;
	case NTetris::Right_Button:
    case NTetris::D_Button:
		++mObject.posX;
        break;
	case NTetris::Auto_Button:
		++mObject.posY;
		break;
	case NTetris::Spase_Button:
		if (mDelayTime - mLastRotateTime > TIME_BETWEEN_ROTATES) {
			mLastRotateTime = mDelayTime;
			rotate();
			mPressedButtonSpase = true;
		}
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
	} else if (mDelayTime > TIME_AUTO_SHIFT && mPressedButtonSpase == false) {
		KeyPressed(NTetris::Auto_Button);
		mDelayTime = 0;
		mLastRotateTime = 0;
	}
    if (checkCollision()) {
		processingCollision();
    } else {
		moveObject();
		mObjectOld = mObject;
    }
	mPressedButtonSpase = false;
}

//-----------------------------------------------------------------------------
void Tetris::generateNextObject()
//-----------------------------------------------------------------------------
{
	wipeOffItem(mNextObject);
	
	mNextObject.type = getRandomType();
	mNextObject.orientation = NTetris::UP;
	mNextObject.posX = STATUS_FIELD_X_START + STATUS_FIELD_X_SIZE/2-1;
	mNextObject.posY = STATUS_FIELD_Y_START + STATUS_FIELD_Y_SIZE/2;
	mNextObject.mItem = generateItem(mNextObject.type);

	drawItem(mNextObject);
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
	initGameField();
	//printLinesForTesting();
}

//-----------------------------------------------------------------------------
void Tetris::initGameField()
//-----------------------------------------------------------------------------
{
	for (uint8_t y = GAME_FIELD_Y_START; y <= GAME_FIELD_Y_SIZE; ++y) {
		for (uint8_t x = GAME_FIELD_X_START; x <= GAME_FIELD_X_SIZE; ++x) {
			SetChar(x,y, SPACE_SYMBOL);
		}
	}
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
	printScore();
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
	drawItem(mObject);
}

//-----------------------------------------------------------------------------
bool Tetris::checkCollision()
//-----------------------------------------------------------------------------
{
	wipeOffItem(mObjectOld);

	NTetris::T_OBJECT tempObject = mObject;
	++tempObject.posY;
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
	for (uint8_t y = GAME_FIELD_Y_START; y < mObject.posY + mObject.mItem.size(); ++y) {
		if (findSumOfLine(y) >= FULL_LINE_X) {
			changeScore();
			clearLine(y);
			makeLinesOffset(y);
		}
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
	if (findSumOfLine(GAME_FIELD_Y_START) != EMPTY_LINE_X) {
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
	if (hasCollisions()) {
		mObject = mObjectOld;
	}
}

//-----------------------------------------------------------------------------
Item Tetris::generateItem(NTetris::E_FIG_TYPE figure) const
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
		Item({
			{OBJECT_SYMBOL}}),
	};
	return randomItem[figure];
	//return randomItem[S_FIGURE];
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
				if (object.posX + x >= STATUS_FIELD_X_START) {
					SetChar(object.posX + x, object.posY + y, OBJECT_EMPTY);
				}
				else {
					SetChar(object.posX + x, object.posY + y, SPACE_SYMBOL);
				}
				
			}
		}
	}
}

//-----------------------------------------------------------------------------
void Tetris::rotate()
//-----------------------------------------------------------------------------
{
	if ((mObject.type == NTetris::LINE
		|| mObject.type == NTetris::S_FIGURE
		|| mObject.type == NTetris::Z_FIGURE)
		&& mObject.orientation == NTetris::LEFT) {
		mObject.mItem = generateItem(mObject.type);
		mObject.orientation = NTetris::UP;
	}
	else {
		Item tempItem(mObject.mItem.size());
		for (uint8_t y = 0; y < mObject.mItem.size(); ++y) {
			tempItem[y].resize(mObject.mItem.size());
			for (uint8_t x = 0; x < mObject.mItem.size(); ++x) {
				tempItem[y][x] = mObject.mItem[x][mObject.mItem.size() - y - 1];
			}
		}
		mObject.mItem = tempItem;
		changeOrientation();
	}
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
uint32_t Tetris::findSumOfLine(uint8_t y)
//-----------------------------------------------------------------------------
{
	uint32_t sum = 0;
	for (uint8_t x = GAME_FIELD_X_START; x <= GAME_FIELD_X_FINISH; ++x) {
		sum += (GetChar(x, y));
	}
	return sum;
}

//-----------------------------------------------------------------------------
NTetris::E_FIG_TYPE Tetris::getRandomType() const
//-----------------------------------------------------------------------------
{
	srand(time(0));
	int rand = std::rand() % NTetris::LAST_FIGURE;
	return static_cast<NTetris::E_FIG_TYPE> (rand);
}

//-----------------------------------------------------------------------------
void Tetris::changeOrientation()
//-----------------------------------------------------------------------------
{
	switch (mObject.orientation)
	{
	case NTetris::UP:
		mObject.orientation = NTetris::LEFT;
		break;
	case NTetris::LEFT:
		mObject.orientation = NTetris::DOWN;
		break;
	case NTetris::DOWN:
		mObject.orientation = NTetris::RIGHT;
		break;
	case NTetris::RIGHT:
		mObject.orientation = NTetris::UP;
		break;
	default:
		break;
	}
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
