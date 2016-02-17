#ifndef __AI_H__
#define __AI_H__

#include "SceneGame.h"

class SceneGame;
class Step;

class AI
{
public:

	static Step* getStep(SceneGame* game);


	static int getScore(SceneGame* game);

	static std::vector<Step*> getAllPossibleMove(SceneGame* game);

	static void fakeMove(SceneGame* game, Step* step);
	static void unfakeMove(SceneGame* game, Step* step);
	
	static Step* getStep(SceneGame* game, int level);
	static int getMinScore(SceneGame* game, int level,int curMinScore);
	static int getMaxScore(SceneGame* game, int level, int curMaxScore);


	// alphaBeta 
	static Step* getStepByAlphaBetaSearch(SceneGame* game, int level);
	static int alphaBetaSearch(SceneGame* game, int alpha, int beta, int level);
	
	static int _maxLevel;
	static Step* _step;
};

#endif
