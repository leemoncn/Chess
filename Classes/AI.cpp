#include "AI.h"


//enum TYPE{ CHE, MA, PAO, BING, JIANG, SHI, XIANG };

static int g_score[7] =
{
	100,
	45,
	45,
	20,
	1500,
	10,
	10
};

// 计算局面分
int AI::getScore(SceneGame* game)
{
	int blackScore = 0;
	int redScore = 0;

	for (int i = 16; i < 32; ++i)
	{
		if (!game->_c->at(i)->_dead)
			blackScore += g_score[game->_c->at(i)->_type];
	}

	for (int i = 0; i < 16; ++i)
	{
		if (!game->_c->at(i)->_dead)
			redScore += g_score[game->_c->at(i)->_type];
	}

	return blackScore - redScore;
}

std::vector<Step*> AI::getAllPossibleMove(SceneGame* game)
{
	std::vector<Step*> ret;
	for (int i = 16; i < 32; ++i)
	{
		if (!game->_c->at(i)->_dead)
		{
			for (int row = 0; row <= 9; ++row)
				for (int col = 0; col <= 8; ++col)
				{
					int killid = game->getChessFromRowCol(row, col);
					if (game->canMove(i, row, col, killid))
					{
						Step* step = new Step;
						step->moveid = i;
						step->killid = killid;
						step->rowFrom = game->_c->at(i)->_row;
						step->colFrom = game->_c->at(i)->_col;
						step->rowTo = row;
						step->colTo = col;
						ret.push_back(step);
					}
				}
		}
	}

	return ret;
}

void AI::fakeMove(SceneGame* game, Step* step)
{
	Chess* moveChess = game->_c->at(step->moveid);
	moveChess->_row = step->rowTo;
	moveChess->_col = step->colTo;

	if (step->killid != -1)
	{
		Chess* killChess = game->_c->at(step->killid);
		killChess->_dead = true;
	}
}

void AI::unfakeMove(SceneGame* game, Step* step)
{
	Chess* moveChess = game->_c->at(step->moveid);
	moveChess->_row = step->rowFrom;
	moveChess->_col = step->colFrom;

	if (step->killid != -1)
	{
		Chess* killChess = game->_c->at(step->killid);
		killChess->_dead = false;
	}
}

Step* AI::getStep(SceneGame* game)
{
	int highScore = -3000;
	Step* ret = nullptr;

	
	std::vector<Step*> allMove = getAllPossibleMove(game);
	std::vector<Step*>::iterator it;
	for (it = allMove.begin(); it != allMove.end(); ++it)
	{
		Step* step = *it;
		fakeMove(game, step);
		int score = getScore(game);
		unfakeMove(game, step);
		if (score > highScore)
		{
			highScore = score;
			ret = step;
		}
	}

	for (it = allMove.begin(); it != allMove.end(); ++it)
	{
		Step* step = *it;
		if (step != ret) delete step;
	}

	return ret;
}
///////////////////////////////////////////////////////////////////////
 /*
 多步智能算法的思想是，继承单步智能算法的主体思想，往下多测试几步
 如 4 步，则为  电脑 - 玩家 - 电脑 - 玩家
 所以取的方式是 电脑最大分 - 玩家最少分 - 电脑最大分 - 玩家最少分
 此过程为一个递推的过程，逆推则是 Min - Max - Min - Max
 故getStep中调用Min，Min中调用Max，Max中调用Min，退出条件是level = 0；
 */
Step* AI::getStep(SceneGame* game, int level)
{
	int highScore = -3000;
	Step* ret = NULL;

	
	std::vector<Step*> allMove = getAllPossibleMove(game);
	std::vector<Step*>::iterator it;
	for (it = allMove.begin(); it != allMove.end(); ++it)
	{
		Step* step = *it;
		fakeMove(game, step);
		int score = getMinScore(game, level - 1,highScore);
		unfakeMove(game, step);
		if (score > highScore)
		{
			highScore = score;
			ret = step;
		}
	}

	for (it = allMove.begin(); it != allMove.end(); ++it)
	{
		Step* step = *it;
		if (step != ret) delete step;
	}

	return ret;
}

int AI::getMinScore(SceneGame* game, int level,int curMinScore)
{
	if (level == 0)
		return getScore(game);

	int minScore = 3000;

	std::vector<Step*> allMove = getAllPossibleMove(game);
	std::vector<Step*>::iterator it;
	for (it = allMove.begin(); it != allMove.end(); ++it)
	{
		Step* step = *it;
		fakeMove(game, step);
		int score = getMaxScore(game, level - 1,minScore);
		unfakeMove(game, step);

		if (score <= curMinScore)
		{
			minScore = score;
			break;
		}

		if (score < minScore)
		{
			minScore = score;
		}
	}

	for (it = allMove.begin(); it != allMove.end(); ++it)
	{
		Step* step = *it;
		delete step;
	}

	return minScore;
}

int AI::getMaxScore(SceneGame* game, int level, int curMaxScore)
{
	if (level == 0)
		return getScore(game);

	int maxScore = -3000;

	std::vector<Step*> allMove = getAllPossibleMove(game);
	std::vector<Step*>::iterator it;
	for (it = allMove.begin(); it != allMove.end(); ++it)
	{
		Step* step = *it;
		fakeMove(game, step);
		int score = getMinScore(game, level - 1,maxScore);
		unfakeMove(game, step);

		if (score >= curMaxScore)
		{
			maxScore = score;
			break;
		}
		if (score > maxScore)
		{
			maxScore = score;
		}
	}

	for (it = allMove.begin(); it != allMove.end(); ++it)
	{
		Step* step = *it;
		delete step;
	}

	return maxScore;
}

Step* AI::_step = nullptr;
int AI::_maxLevel = 0;

Step* AI::getStepByAlphaBetaSearch(SceneGame* game, int level)
{
	
	_maxLevel = level;
	_step = nullptr;

	alphaBetaSearch(game, -3000, 3000, level);

	return _step;
}

int AI::alphaBetaSearch(SceneGame* game, int alpha, int beta, int level)
{
	if (level == 0)
		return getScore(game);

	Step* recordStep = nullptr;
	std::vector<Step*> allMove = getAllPossibleMove(game);
	std::vector<Step*>::iterator iter;
	for (iter = allMove.begin(); iter != allMove.end(); ++iter)
	{
		Step* step = *iter;
		fakeMove(game, step);
		int score = -alphaBetaSearch(game, -beta, -alpha, level - 1);
		unfakeMove(game, step);
	
		if (score >= beta)
		{
			alpha = score;
			break;
		}

		if (score > alpha)
		{
			alpha = score;
			
			if (level == _maxLevel)
			{
				recordStep = step;
			}
		}
	}

	for (iter = allMove.begin(); iter != allMove.end(); ++iter)
	{
		Step* step = *iter;
		if (recordStep != step) delete step;
	}

	if (recordStep)
	{
		if (_step) delete _step;
		_step = recordStep;
	}

	return alpha;
}