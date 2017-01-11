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
#include <queue>
#include <set>
#include "State.hpp"
#include "Location.h"
#include "Route.hpp"
#include "Path.hpp"

/**
 @brief The struct Bot represents my bot in the game.
 */
struct Bot
{
	State state;
	
	Bot();
	
	int rows;
	int cols;
	
	void playGame();    //plays a single game of Ants
	void makeMoves();   //makes moves for a single turn
	void endTurn();     //indicates to the engine that it has made its moves
	
	/* My elements */
	int numberOfTurns;				// total number of turns
	double totalDecisionTime;		// Cumulative time taken by the makeMoves() function
	std::vector<Location> orders;	// current orders to move (= next locations of ants)
	std::vector<Path> foodPaths;
	std::map<Location, Path> assignedFoodTargets;	// key: Location of ant, value: path
	
	void analyzeSituation();		// Determines targets
	
	void calculateFoodPaths();	// Calculate foodPaths between food and ant
	void assignFoodPaths();		// Assigned food targets to ants depending on food routes
	
	bool isOrder(Location loc);				// determines whether a location is in the orders
	bool isMyAnt(Location loc);
	bool isWater(Location loc);
	bool isVisible(Location loc);
	bool isValidStepInPath(Location loc);	// Determines if given location can be part of a path
	bool isAntInFoodPaths(Location loc);
	int findPrevious(std::vector<std::vector<int> > distances, Location loc);
	bool foodInFoodTarget(Location loc);	// determines whether the position is an ant already assigned to a target
	bool foodInFoodPaths(Location loc);
	
	void printDistances(std::vector<std::vector<int> > distances);
	void printAssignedFoodTargets();
	void printFoodPaths();

};

#endif //BOT_H_
