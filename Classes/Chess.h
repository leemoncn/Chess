#ifndef __STONE_H__
#define __STONE_H__

#include "cocos2d.h"

USING_NS_CC;

class Chess : public Sprite
{
public:
	CREATE_FUNC(Chess);
	bool init();

	enum TYPE
	{
		CHE,MA,PAO,BING,JIANG,SHI,XIANG
	};

	void initChess(int id,bool bRestart = false);
	Vec2 fromPlate();

	int _id;
	bool _red;
	int _row;
	int _col;
	bool _dead;
	TYPE _type;

	static int _d;
	static int _offx;
	static int _offy;

	static int _redJiang;
	static int _blackJiang;
};

#endif