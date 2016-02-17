#ifndef __SCENEGAME_SCENE_H__
#define __SCENEGAME_SCENE_H__

#include "cocos2d.h"
#include "Chess.h"	
#include "AI.h"
USING_NS_CC;


struct Step
{
	int moveid;
	int killid;
	int rowFrom;
	int colFrom;
	int rowTo;
	int colTo;
};

class SceneGame : public CCLayer
{
public:
	static Scene* scene();
	CREATE_FUNC(SceneGame);
	bool init();
	void onExit();

	void createPlate();

	std::vector<Chess*>* _c;

	virtual bool onTouchBegan(Touch*, Event*);
	virtual void onTouchEnd(Touch*, Event*);

	void addCtrlPanel();
	void Regret(Ref*);

	void doRegret();
	void doRegret2();

	void startServer(Ref*);
	void startClient(Ref*);
	void Restart(Ref*);
	void restartInit();
	void CheckRecv(float);
	void CheckListen(float);

	void moveNode(Node* node, Point pt)
	{
		node->setPosition(node->getPosition() + pt);
	}

	void ComputerMove(float dt);
	
	void SelectChess(Touch* touch);
	void MoveChess(Touch* touch);

	void AfterMove(Node* node, void* killid);

	void recordStep(int moveid, int killid, int rowFrom, int colFrom, int rowTo, int colTo);

	
	bool canMove(int moveid, int row, int col, int killid);
	bool canMoveChe(int moveid, int row, int col);
	bool canMovePao(int moveid, int row, int col, int killid);
	bool canMoveMa(int moveid, int row, int col);
	bool canMoveBing(int moveid, int row, int col);
	bool canMoveJiang(int moveid, int row, int col, int killid);
	bool canMoveShi(int moveid, int row, int col);
	bool canMoveXiang(int moveid, int row, int col);


	bool Screen2Plate(const Vec2& ptScreen, int& row, int& col);
	CCPoint Plate2Screen(int row, int col);

	int getChessCount(int row1, int col1, int row2, int col2);

	int getChessFromRowCol(int row, int col);

	bool isSameColor(int id1, int id2);

	
	int _selectid;


	bool _bRedTurn;


	CCSprite* _selectSprite;

	
	std::vector<Step*>* _steps;

	static bool _bRedSide;
	static bool _bRestart;
};



#endif // __SceneGame_SCENE_H__
