#include "SceneGame.h"
#include "AppMacros.h"
#include "Chess.h"
#include "AI.h"
#include "NetBattle.h"

USING_NS_CC;


bool IS_NET = 0;
bool SceneGame::_bRestart = false;
bool SceneGame::_bRedSide = true;


Scene* SceneGame::scene()
{
  
    auto scene = Scene::create();
  
    auto layer = SceneGame::create();

    scene->addChild(layer);

	
   
    return scene;
}


bool SceneGame::init()
{
  
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	_c = new std::vector<Chess*>();
	_c->resize(1000);
	_steps = new std::vector<Step*>();
	_steps->reserve(1000);


	createPlate();

	Chess::_d = Director::getInstance()->getWinSize().height / 10;
	Chess::_offx = Chess::_d;
	Chess::_offy = Chess::_d / 2;

	for (int i = 0; i < 32; i++)
	{
		_c->at(i) = Chess::create();
		_c->at(i)->initChess(i);
		addChild(_c->at(i));
	}


	EventDispatcher *eventDispatcher = Director::getInstance()->getEventDispatcher();
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(SceneGame::onTouchBegan, this);
	listener->onTouchEnded = CC_CALLBACK_2(SceneGame::onTouchEnd, this);
	eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	_selectid = 1;
	_selectSprite = Sprite::create("selected.png");
	_selectSprite->setVisible(false);
	addChild(_selectSprite);
	_selectSprite->setScale(0.6f);
	_selectSprite->setZOrder(100);

	_bRedTurn = true;

	_steps->clear();
	addCtrlPanel();

	return true;
}

void SceneGame::createPlate()
{
	auto plate = Sprite::create("background.png");
	this->addChild(plate);
	plate->setPosition(Vec2(0, 0));
	plate->setAnchorPoint(Vec2(0, 0));
	plate->setScale(winSize.height / plate->getContentSize().height);
}

bool SceneGame::onTouchBegan(Touch *, Event *)
{
	return true;
}

void SceneGame::onTouchEnd(Touch *touch, Event *)
{
	if (_selectid == -1)
	{
		SelectChess(touch);
	}
	else
	{
		MoveChess(touch);
	}
}

bool SceneGame::Screen2Plate(const Vec2& ptScreen, int& row, int& col)
{
	int distance = Chess::_d * Chess::_d / 4;

	for (row = 0;row <= 9;++row)
	{
		for (col = 0;col <= 8;++col)
		{
			Vec2 ptCenter = Plate2Screen(row, col);
			if (ptCenter.getDistanceSq(ptScreen) < distance)
			{
				return true;
			}
		}
	}
	return false;
}

cocos2d::Vec2 SceneGame::Plate2Screen(int row, int col)
{
	int x = col * Chess::_d + Chess::_offx;
	int y = row * Chess::_d + Chess::_offy;

	return Vec2(x, y);
}

int SceneGame::getChessFromRowCol(int row, int col)
{
	for (int i = 0; i < 32;++i)
	{
		if (_c->at(i)->_row == row && _c->at(i)->_col == col && !_c->at(i)->_dead)
			return i;
	}

	return -1;
}

bool SceneGame::isSameColor(int id1, int id2)
{
	return _c->at(id1)->_red == _c->at(id2)->_red;
}

void SceneGame::SelectChess(Touch* touch)
{
#if 0
	Vec2 ptClicked = touch->getLocation();
	int row, col;
	bool bClick = Screen2Plate(ptClicked, row, col);
	
	if (!bClick)
	{
		return;
	}

	int clickid = getChessFromRowCol(row, col);

	if (clickid == -1)
	{
		return;
	}

	if (_c->at(clickid)->_red != _bRedTurn)
	{
		return;
	}

	if (_c->at(clickid)->_red != _bRedSide)
	{
		return;
	}

	_selectid = clickid;

	_selectSprite->setPosition(Plate2Screen(row, col));
	//_selectSprite->setVisible(true);
	_selectSprite->setOpacity(255);

	char buf[2];
	buf[0] = 1;
	buf[1] = _selectid;
	NetBattle::Send(buf, 2);
#endif


	Vec2 ptClicked = touch->getLocation();
	int row, col;
	bool bClick = Screen2Plate(ptClicked, row, col);
	// 如果点击在棋盘外，该次点击无效
	if (!bClick)
	{
		return;
	}

	int clickid = getChessFromRowCol(row, col);
	// 如果点击位置没有象棋，那么点击也无效
	if (clickid == -1)
	{
		return;
	}

	// 如果被点中的棋子，和_bRedTurn颜色不一致，不让选中
	if (_c->at(clickid)->_red != _bRedTurn)
		return;

	// 黑方只能点黑棋，红方只能点红棋，用于网络版
	if (_c->at(clickid)->_red != _bRedSide)
		return;

	// 记录这次的点击
	_selectid = clickid;

	// 显示该棋子被选中的效果
	_selectSprite->setPosition(Plate2Screen(row, col));
	//_selectSprite->setVisible(true);
	_selectSprite->setOpacity(255);

	// 发送选中棋子的信息给对方
	char buf[2];
	buf[0] = 1;
	buf[1] = _selectid;
	NetBattle::Send(buf, 2);
}

void SceneGame::MoveChess(Touch* touch)
{

#if 0
	Vec2 ptClicked = touch->getLocation();
	int row, col;

	bool bClick = Screen2Plate(ptClicked, row, col);

	if (!bClick)
	{
		return;
	}

	int clickid = getChessFromRowCol(row, col);

	if (clickid != -1)
	{
		if (isSameColor(clickid, _selectid))
		{
			_selectid = clickid;
			Vec2 pt = Plate2Screen(row, col);
			_selectSprite->setPosition(pt);
			//_selectSprite->setVisible(true);
			_selectSprite->setOpacity(255);
			return;
		}
		/*else
		{
			_c->at(clickid)->setVisible(false);
			_c->at(clickid)->_dead = true;
		}*/

	}

	if (!canMove(_selectid, row, col, clickid))
	{
		return;
	}

	recordStep(_selectid, clickid, _c->at(_selectid)->_row, _c->at(_selectid)->_col, row, col);

	if (clickid != -1)
	{
		_c->at(clickid)->setVisible(false);
		_c->at(clickid)->_dead = true;
	}



	_c->at(_selectid)->_row = row;
	_c->at(_selectid)->_col = col;
	//_c->at(_selectid)->setPosition(Plate2Screen(row, col));

	_c->at(_selectid)->setZOrder(100);
	MoveTo* moveto = MoveTo::create(1.0f, Plate2Screen(row, col));
	CallFuncN* callfuncN = CallFuncN::create(CC_CALLBACK_1(SceneGame::AfterMove,this, (void*)(intptr_t)clickid));
	_c->at(_selectid)->runAction(CCSequence::createWithTwoActions(moveto, callfuncN));

	FadeOut* fadeout = FadeOut::create(1.0f);
	MoveTo* SelectMoveto = MoveTo::create(1.0f, _c->at(_selectid)->fromPlate());
	Spawn* spawn = Spawn::createWithTwoActions(fadeout, SelectMoveto);
	_selectSprite->runAction(spawn);

	_selectid = -1;
	//_selectSprite->setVisible(false);
	_bRedTurn = !_bRedTurn;
	/*
	if (!_bRedTurn)
	{		
		//单步智能
		Step* step = AI::getStep(this);

		Chess* moveChess = _c->at(step->moveid);
		moveChess->_row = step->rowTo;
		moveChess->_col = step->colTo;
		_bRedTurn = !_bRedTurn;
		moveChess->setPosition(moveChess->fromPlate());

		if (step->killid != -1)
		{
			Chess* killChess = _c->at(step->killid);
			killChess->_dead = true;
			killChess->setVisible(false);
		}
		_steps->push_back(step);
		//多步智能
		// scheduleOnce(schedule_selector(SceneGame::computerMove), 0.01f);

		//网络对战

	}
*/

	Step* step = *_steps->rbegin();
	char buf[4];
	buf[0] = 2;
	buf[1] = step->moveid;
	buf[2] = step->rowTo;
	buf[3] = step->colTo;
	NetBattle::Send(buf, 4);


	NetBattle::RecvStart();
	schedule(schedule_selector(SceneGame::CheckRecv));
#endif


	Vec2 ptClicked = touch->getLocation();
	int row, col;
	bool bClick = Screen2Plate(ptClicked, row, col);
	
	if (!bClick)
	{
		return;
	}


	int clickid = getChessFromRowCol(row, col);
	// 如果点击位置没有象棋，那么点击也无效
	if (clickid != -1)
	{
		// 如果后一次点击的棋子和前一次相同，那么换选择
		if (isSameColor(clickid, _selectid))
		{
			_selectid = clickid;
			// 显示该棋子被选中的效果
			Vec2 pt = Plate2Screen(row, col);
			_selectSprite->setPosition(pt);
			//_selectSprite->setVisible(true);

			_selectSprite->setOpacity(255);
			// 换选择之后，这次点击处理就结束了
			return;
		}
	}

	// 判断棋子是否可以移动
	if (!canMove(_selectid, row, col, clickid))
	{
		return;
	}

	// 可以移动
	// 记录移动信息
	recordStep(_selectid, clickid, _c->at(_selectid)->_row, _c->at(_selectid)->_col, row, col);

	// 走棋相关
	if (clickid != -1)
	{
		// 杀掉
		//_s[clickid]->setVisible(false);
		_c->at(clickid)->_dead = true;
	}

	_c->at(_selectid)->_row = row;
	_c->at(_selectid)->_col = col;

	//	_s[_selectid]->setPosition(Plate2Screen(row, col));
	_c->at(_selectid)->setZOrder(100);
	MoveTo* moveto = MoveTo::create(1.0f, Plate2Screen(row, col));
	CallFuncN* callfuncN = CallFuncN::create(CC_CALLBACK_1(SceneGame::AfterMove, this,(void*)(intptr_t)clickid));
	_c->at(_selectid)->runAction(Sequence::createWithTwoActions(moveto, callfuncN));

	FadeOut* fadeout = FadeOut::create(1.0f);
	MoveTo* SelectMoveto = MoveTo::create(1.0f, _c->at(_selectid)->fromPlate());
	Spawn* spawn = CCSpawn::createWithTwoActions(fadeout, SelectMoveto);
	_selectSprite->runAction(spawn);

	_selectid = -1;
	//_selectSprite->setVisible(false);
	_bRedTurn = !_bRedTurn;


	// 发送移动棋子报文
	Step* step = *_steps->rbegin();
	char buf[4];
	buf[0] = 2;
	buf[1] = step->moveid;
	buf[2] = step->rowTo;
	buf[3] = step->colTo;
	NetBattle::Send(buf, 4);

	// 接收信息
	NetBattle::RecvStart();
	schedule(schedule_selector(SceneGame::CheckRecv));

}

bool SceneGame::canMove(int moveid, int row, int col, int killid)
{
	Chess* s =_c->at(moveid);
	switch (s->_type)
	{
	case Chess::CHE:
		return canMoveChe(moveid, row, col);

	case Chess::MA:
		return canMoveMa(moveid, row, col);

	case Chess::PAO:
		// 炮有两种走法，走直线（吃） 和 跳子（不吃）
		return canMovePao(moveid, row, col, killid);

	case Chess::BING:
		return canMoveBing(moveid, row, col);

	case Chess::JIANG:
		return canMoveJiang(moveid, row, col, killid);

	case Chess::SHI:
		return canMoveShi(moveid, row, col);

	case Chess::XIANG:
		return canMoveXiang(moveid, row, col);
		break;
	}
	return false;
}

bool SceneGame::canMoveChe(int moveid, int row, int col)
{
	Chess * s = _c->at(moveid);

	return getChessCount(s->_row, s->_col, row, col) == 0;
}

bool SceneGame::canMovePao(int moveid, int row, int col, int killid)
{
	Chess * s = _c->at(moveid);
	if (killid == -1)
	{
		return getChessCount(s->_row, s->_col, row, col)==0;
	}
	
	return getChessCount(s->_row, s->_col, row, col) == 1;
}

bool SceneGame::canMoveMa(int moveid, int row, int col)
{
	Chess * s = _c->at(moveid);

	int dRow = std::abs(s->_row - row);
	int dCol = std::abs(s->_col - col);
	int d = dRow * 10 + dCol;

	if (d == 12 || d == 21)
	{
		int cRow, cCol;
		if (d == 12)
		{
			cCol = (col + s->_col) / 2;
			cRow = s->_row;
		}
		else
		{
			cCol = s->_col;
			cRow = (s->_row + row) / 2;
		}

		if (getChessFromRowCol(cRow, cCol) == -1)
			return true;
	}

	return false;
}

bool SceneGame::canMoveBing(int moveid, int row, int col)
{
	Chess * s = _c->at(moveid);

	int dRow = std::abs(s->_row - row);
	int dCol = std::abs(s->_col - col);
	int d = dRow * 10 + dCol;

	if (d != 1 && d != 10)
	{
		return false;
	}
#if IS_NET
	if (s->_red)
#endif
	{
		if (row < s->_row) return false;

		if (s->_row <= 4 && s->_row == row) return false;
	}
#if IS_NET
	else
	{
		if (row > s->_row) return false;
		if (s->_row >= 5 && s->_row == row) return false;
	}
#endif
	return true;
}

bool SceneGame::canMoveJiang(int moveid, int row, int col, int killid)
{
	Chess *s = _c->at(moveid);
	//将照面
	if (killid != -1)
	{
		Chess *kill = _c->at(killid);
		if (kill->_type == Chess::JIANG)
		{
			return canMoveChe(moveid, row, col);
		}
	}

	int dRow = abs(s->_row - row);
	int dCol = abs(s->_col - col);
	int d = dRow * 10 + dCol;

	if (d != 1 && d != 10 && d != 11)
		return false;
	
	if (col < 3 || col > 5) return false;
#if IS_NET
	if (s->_red)
#endif
	{
		if (row < 0 || row > 2) return false;
		if (d == 11)
		{
			bool crossline = (s->_col == 3 && (s->_row == 0 || s->_row == 2)) || ((s->_col == 5) && (s->_row == 0 || s->_row == 2)) ||
				(s->_col == 4 && s->_row == 1);
			if (!crossline)
			{
				return false;
			}
		}
	}
#if IS_NET
	else
	{
		if (row < 7 || row > 9) return false;

		if (d == 11)
		{
			bool crossline = (s->_col == 3 && (s->_row == 7 || s->_row == 9)) || ((s->_col == 5) && (s->_row == 7 || s->_row == 9)) ||
				(s->_col == 4 && s->_row == 8);
			if (!crossline)
			{
				return false;
			}
		}
	}
#endif
	return true;
}

bool SceneGame::canMoveShi(int moveid, int row, int col)
{
	Chess * s = _c->at(moveid);
	int dRow = abs(s->_row - row);
	int dCol = abs(s->_col - col);
	int d = dRow * 10 + dCol;
	// crossline
	if (d != 11) return false;

	if (col < 3 || col > 5) return false;
#if IS_NET
	if (s->_red)
#endif
	{
		if (row < 0 || row > 2) return false;
	}
#if IS_NET
	else
	{
		if (row < 7 || row > 9) return false;
	}
#endif
	return true;
}

bool SceneGame::canMoveXiang(int moveid, int row, int col)
{
	Chess * s = _c->at(moveid);
	int dRow = abs(s->_row - row);
	int dCol = abs(s->_col - col);
	int d = dRow * 10 + dCol;
	// crossline
	if (d != 22) return false;

	int cRow, cCol;
	cRow = (row + s->_row) / 2;
	cCol = (col + s->_col) / 2;
	if (getChessFromRowCol(cRow, cCol) != -1)
		return false;
#if IS_NET
	if (s->_red)
#endif
	{
		if (row > 4) return false;
	}
#if IS_NET
	else if (row < 5)
	{
		return false;
	}
#endif
	return true;
}

int SceneGame::getChessCount(int row1, int col1, int row2, int col2)
{
	int ret = 0;

	if (row1 != row2 && col1 != col2) return -1;
	if (row1 == row2 && col1 == col2) return -1;

	if (row1 == row2)
	{
		int min, max;
		min = col1 < col2 ? col1 : col2;
		max = col1 > col2 ? col1 : col2;

		for (int col = min + 1; col < max; ++col)
		{
			int id = getChessFromRowCol(row1, col);
			if (id != -1) ++ret;
		}
	}

	if (col1 == col2)
	{
		int min, max;
		min = row1 < row2 ? row1 : row2;
		max = row1 > row2 ? row1 : row2;

		for (int row = min + 1; row < max; ++row)
		{
			int id = getChessFromRowCol(row, col1);
			if (id != -1) ++ret;
		}
	}
	return ret;
}

void SceneGame::onExit()
{
	Layer::onExit();
}




void SceneGame::addCtrlPanel()
{
	Menu* menu = Menu::create();
	this->addChild(menu);


	LayerColor* bg = LayerColor::create(Color4B(150, 80, 50, 255), winSize.width, winSize.height);
	addChild(bg, -1);
	bg->setGlobalZOrder(-300);

	MenuItemImage *regretItem = MenuItemImage::create("regret.jpg", "regret.jpg"
		, CC_CALLBACK_1(SceneGame::Regret,this));

	menu->addChild(regretItem);

	moveNode(regretItem, Vec2(160, 60));

	
	MenuItem* startServerItem = MenuItemFont::create("StartServer",CC_CALLBACK_1(SceneGame::startServer,this));
	menu->addChild(startServerItem);
	moveNode(startServerItem, Vec2(160, 0));


	MenuItem* startClientItem = MenuItemFont::create("StartClient",CC_CALLBACK_1(SceneGame::startClient,this));
	menu->addChild(startClientItem);
	moveNode(startClientItem, Vec2(160, -60));

	
	startServerItem->setUserObject(startClientItem);
	startClientItem->setUserObject(startServerItem);


	MenuItem* restartItem = MenuItemFont::create("Restart", CC_CALLBACK_1(SceneGame::Restart,this));
	menu->addChild(restartItem);
	moveNode(restartItem, Vec2(160, -120));

	if (_bRestart)
	{
		startServerItem->setEnabled(false);
		startClientItem->setEnabled(false);

		NetBattle::clear();

		if (_bRedSide)
		{
			startServer(startServerItem);
		}
		else
		{
			startClient(startClientItem);
		}
	}
}



void SceneGame::Regret(Ref*)
{
	/*
	for (int i = 0;i < 2;++i)
	{

		if (_steps->size() == 0)
			return;

		Step* step = *(_steps->rbegin());
		_steps->pop_back();

		Chess* s = _c->at(step->moveid);
		s->_row = step->rowFrom;
		s->_col = step->colFrom;
		s->setPosition(s->fromPlate());

		Chess* kill;
		if (step->killid != -1)
		{
			kill = _c->at(step->killid);
			kill->_dead = false;
			kill->setVisible(true);
		}

		_bRedTurn = !_bRedTurn;
		delete step;

		_selectid = -1;
		_selectSprite->setVisible(false);
	}
	*/

	
	if (_bRedSide != _bRedTurn)
		return;

	
	if (_steps->size() < 2)
		return;

	doRegret2();

	char buf = 3;
	NetBattle::Send(&buf, 1);
}

void SceneGame::recordStep(int moveid, int killid, int rowFrom, int colFrom, int rowTo, int colTo)
{
	Step* s = new Step();
	s->colFrom = colFrom;
	s->colTo = colTo;
	s->killid = killid;
	s->moveid = moveid;
	s->rowFrom = rowFrom;
	s->rowTo = rowTo;

	_steps->push_back(s);
}

void SceneGame::ComputerMove(float dt)
{
	Step* step = AI::getStepByAlphaBetaSearch(this, 2);

	Chess* moveChess = _c->at(step->moveid);
	moveChess->_row = step->rowTo;
	moveChess->_col = step->colTo;
	_bRedTurn = !_bRedTurn;
	moveChess->setPosition(moveChess->fromPlate());

	if (step->killid != -1)
	{
		Chess* killChess = _c->at(step->killid);
		killChess->_dead = true;
		killChess->setVisible(false);
	}
	
	_steps->push_back(step);
}
void SceneGame::startClient(Ref* sender)
{
	MenuItem* item = (MenuItem*)sender;
	item->setEnabled(false);

	Ref* obj = item->getUserObject();
	item = (MenuItem*)obj;
	item->setEnabled(false);


	_bRedSide = false;
	if (NetBattle::Connect("127.0.0.1"))
	{
	
		cocos2d::log("client Start Game");

		
		for (int i = 0; i < 32; i++)
		{
			Chess* s = _c->at(i);
			s->_row = 9 - s->_row;
			s->_col = 8 - s->_col;
			s->setPosition(s->fromPlate());
		}

		
		NetBattle::RecvStart();
		schedule(schedule_selector(SceneGame::CheckRecv));
	}
	else
	{
		cocos2d::log("Connect failure....");
	}
}

void SceneGame::CheckRecv(float)
{
	if (NetBattle::isRecvComplete())
	{
		unschedule(schedule_selector(SceneGame::CheckRecv));

		int len;
		char* data = NetBattle::RecvData(len);

		if (data[0] == 4)
		{
			_bRedSide = !_bRedSide;

			restartInit();
		}
		else if (data[0] == 3)
		{
			doRegret2();
		
			NetBattle::RecvStart();
			schedule(schedule_selector(SceneGame::CheckRecv));
		}
		else if (data[0] == 1)
		{
			
			_selectid = data[1];
			_selectSprite->setPosition(_c->at(_selectid)->fromPlate());
			_selectSprite->setVisible(true);

		
			NetBattle::RecvStart();
			schedule(schedule_selector(SceneGame::CheckRecv));
		}
		else if (data[0] == 2)
		{
			
			Chess* s = _c->at(data[1]);
			int row = 9 - data[2];
			int col = 8 - data[3];
			int killid = getChessFromRowCol(row, col);

			recordStep(_selectid, killid, _c->at(_selectid)->_row, _c->at(_selectid)->_col, row, col);

			
			s->_row = row;
			s->_col = col;
			//s->setPosition(s->fromPlate());

			MoveTo* moveto = MoveTo::create(1.0f, s->fromPlate());
			//    CCCallFuncN* callfuncN = CCCallFuncN::create(this, callfuncN_selector(SceneGame::setChessZOrder));
			CallFuncN* callfuncN = CallFuncN::create(CC_CALLBACK_1(SceneGame::AfterMove,this,(void*)(intptr_t)killid));

			s->runAction(Sequence::createWithTwoActions(moveto, callfuncN));

			FadeOut* fadeout = FadeOut::create(1.0f);
			MoveTo* SelectMoveto = MoveTo::create(1.0f, s->fromPlate());
			Spawn* spawn = Spawn::createWithTwoActions(fadeout, SelectMoveto);
			_selectSprite->runAction(spawn);

			if (killid != -1)
			{
				Chess* ks = _c->at(killid);
				ks->_dead = true;
				ks->setVisible(false);
			}

			_selectid = -1;
			_selectSprite->setVisible(false);
			_bRedTurn = !_bRedTurn;
		}
	}
}

void SceneGame::startServer(Ref* sender)
{
	_bRedSide = true;
	NetBattle::Listen();
	schedule(schedule_selector(SceneGame::CheckListen));

	MenuItem* item = (MenuItem*)sender;
	item->setEnabled(false);

	Ref* obj = item->getUserObject();
	item = (MenuItem*)obj;
	item->setEnabled(false);
}

void SceneGame::CheckListen(float)
{
	
	if (NetBattle::isConnected())
	{
		unschedule(schedule_selector(SceneGame::CheckListen));

		cocos2d::log("server Start Game ......");
	}
}

void SceneGame::doRegret()
{
	
	if (_steps->size() == 0)
		return;

	
	Step* step = *_steps->rbegin();
	_steps->pop_back();

	Chess* s = _c->at(step->moveid);
	s->_row = step->rowFrom;
	s->_col = step->colFrom;
	s->setPosition(s->fromPlate());

	Chess* kill;
	if (step->killid != -1)
	{
		kill = _c->at(step->killid);
		kill->_dead = false;
		kill->setVisible(true);
	}

	_bRedTurn = !_bRedTurn;
	delete step;

	
	_selectid = -1;
	_selectSprite->setVisible(false);
}
void SceneGame::doRegret2()
{
	for (int i = 0; i < 2; ++i)
	{
		doRegret();
	}
}

void SceneGame::Restart(Ref *)
{
	if (_bRedSide != _bRedTurn)
		return;
	_bRedSide = !_bRedSide;

	char buf;
	buf = 4;
	NetBattle::Send(&buf, 1);

	restartInit();
#if 0
	_bRestart = true;
	CCScene* scene = CCScene::create();
	SceneGame* game = SceneGame::create();
	scene->addChild(game);
	CCDirector::sharedDirector()->replaceScene(scene);
#endif
}

void SceneGame::restartInit()
{
	// init Chess
	for (int i = 0; i < 32; ++i)
	{
		_c->at(i)->initChess(i, true);
	}

	if (!_bRedSide)
	{
		
		for (int i = 0; i < 32; i++)
		{
			Chess* s = _c->at(i);
			s->_row = 9 - s->_row;
			s->_col = 8 - s->_col;
			s->setPosition(s->fromPlate());
		}

	
		NetBattle::RecvStart();
		schedule(schedule_selector(SceneGame::CheckRecv));
	}

	_selectSprite->setOpacity(0);
	_selectid = -1;
	_bRedTurn = true;
	_steps->clear();

}

void SceneGame::AfterMove(Node *node, void* id)
{
	node->setZOrder(0);

	int killid = (int)(intptr_t)id;
	if (killid != -1)
	{
		_c->at(killid)->setVisible(false);
	}

	
	if (_bRedTurn)
	{
		for (int i = 16; i < 32; ++i)
		{
			if (_c->at(i)->_dead) continue;
			if (canMove(i, _c->at(Chess::_redJiang)->_row, _c->at(Chess::_redJiang)->_col, Chess::_redJiang))
			{
				cocos2d::log("oh red danger");

				break;
			}
		}
	}
	else
	{
		for (int i = 0; i < 16; ++i)
		{
			if (_c->at(i)->_dead) continue;
			if (canMove(i, _c->at(Chess::_blackJiang)->_row, _c->at(Chess::_blackJiang)->_col, Chess::_blackJiang))
			{
				cocos2d::log("oh black danger");

				break;
			}
		}
	}
}


