//
//  Bot.hpp
//  srcXcode
//
//  Created by Alexandre on 07/01/2017.
//  Copyright © 2017 Alexandre. All rights reserved.
//

#ifndef BOT_H_
#define BOT_H_

#include "State.hpp"

/*
 This struct represents your bot in the game of Ants
 */
struct Bot
{
	State state;
	
	Bot();
	
	void playGame();    //plays a single game of Ants
	
	void makeMoves();   //makes moves for a single turn
	void endTurn();     //indicates to the engine that it has made its moves
};

#endif //BOT_H_
