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
#include "Square.h"

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
	std::set<Location> unexploredLocations;	// Set of unexplored locations
	std::vector<Location> orders;	// current orders to move (= next locations of ants)
	std::vector<Path> foodPaths;	// Paths between every (ant;food) couple
	std::map<Location, Path> assignedFoodTargets;	// key: Location of ant, value: path
	std::map<Location, Path> assignedUnexploredTargets;		// Paths between an ant and its closest unexplored location
	std::vector<std::vector<Square> > exploredGrid;	// Grid keeping previously explored squares in memory
	
	void analyzeSituation();		// Determines targets
	void updateExploredGrid();		// update the explored grid
	void initExploredGrid();	// Initialize the explored grid to all visible and accessible squares
	
	void calculateFoodPaths();	// Calculate foodPaths between food and ant
	void assignFoodPaths();		// Assigned food targets to ants depending on food routes
	void moveAntsToFoodOrders();		// Effectively moves ant to their assigned food target
	void moveAntsToUnexploredOrders();	// Effectively moves ant to their assigned unexplored target
	void calculateClosestUnexplored(Location loc);	// Finds the closest unexplored Location for the given ant
	
	bool isOrder(Location loc);				// determines whether a location is in the orders
	bool isMyAnt(Location loc);
	bool isWater(Location loc);
	bool isVisible(Location loc);
	bool isValidStepInPath(Location loc);	// Determines if given location can be part of a path
	bool isAntInFoodPaths(Location loc);
	bool hasAntAssignedFoodTarget(Location loc);
	int findPrevious(std::vector<std::vector<int> > distances, Location loc);
	bool foodInFoodTarget(Location loc);	// determines whether the position is an ant already assigned to a target
	bool foodInFoodPaths(Location loc);
	bool locationInUnexploredPaths(Location loc);
	
	void printDistances(std::vector<std::vector<int> > distances);
	void printAssignedFoodTargets();
	void printAssignedUnexploredPaths();
	void printFoodPaths();
	void printExploredGrid();

};

#endif //BOT_H_
