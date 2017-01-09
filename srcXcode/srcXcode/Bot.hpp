//
//  Bot.hpp
//  srcXcode
//
//  Created by Alexandre on 07/01/2017.
//  Copyright © 2017 Alexandre. All rights reserved.
//

#ifndef BOT_H_
#define BOT_H_

#include <list>
#include <map>
#include "State.hpp"
#include "Location.h"
#include "Route.hpp"

/**
 @brief The struct Bot represents my bot in the game.
 */
struct Bot
{
	State state;
	
	Bot();
	
	void playGame();    //plays a single game of Ants
	void makeMoves();   //makes moves for a single turn
	void endTurn();     //indicates to the engine that it has made its moves
	
	/* My elements */
	int numberOfTurns;				// total number of turns
	double totalDecisionTime;		// Cumulative time taken by the makeMoves() function
	std::vector<Location> orders;	// current orders to move (= next locations of ants)
	std::vector<Route> foodRoutes;	// routes between ants and food, we use list to allow easy sorting
	std::map<Location, Location> foodTargets;	// food targets targeted by ants ; key is ant, value is food
	
	void analyzeSituation();		// Determines targets
	
	void calculateFoodRoutes();		// populates the foodRoutes list with routes
	void assignFoodTargets();		// Assigned food targets to ants depending on food routes
	int calculateDirectionToTarget(const Location ant, const Location target);	// Determines in what direction is the target for the ant
	
	bool isOrder(Location loc);				// determines whether a location is in the orders
	bool foodInFoodTarget(Location loc);	// determines whether the position is an ant already assigned to a target
	
	void printFoodRoutes();			// print food routes for debug
	void printFoodTargets();		// Print food targets assigned to ants
};

#endif //BOT_H_
