//
//  Bot.cpp
//  srcXcode
//
//  Created by Alexandre on 07/01/2017.
//  Copyright © 2017 Alexandre. All rights reserved.
//

#include <iostream>
#include "Bot.hpp"


using namespace std;

Bot::Bot()
{
	numberOfTurns = 0;
	totalDecisionTime = 0.0;
	orders.clear();
	foodRoutes.clear();
	foodTargets.clear();
};

/**
 Determines where to move each ant depending on different factors, and makes 
 the moves.
 */
void Bot::makeMoves()
{
	state.bug << "turn " << state.turn << ":" << endl;
	state.bug << state << endl;	// Display map in text
	
	numberOfTurns++;
	
	analyzeSituation();		// Calculates targets

	//picks out moves for each ant
	for(int i=0; i<(int)state.myAnts.size(); i++)
	{
		Location loc = state.myAnts[i];	// location of current ant
		
		if(foodTargets.find(loc) != foodTargets.end())	// Move toward food if this ant has an assigned target
		{
			int direction	= calculateDirectionToTarget(loc, foodTargets.at(loc));	// find direction to get to target
			state.bug << "Direction for ant " << loc << ": " << CDIRECTIONS[i] << endl;
			Location newLoc	= state.getLocation(loc, direction);	// find location of the tile we're moving to
			if(!state.grid[newLoc.row][newLoc.col].isWater
			   && !state.grid[newLoc.row][newLoc.col].isMyAnt()
			   && !isOrder(newLoc))
			{
				state.makeMove(loc, direction);		// Perform move
				orders.push_back(newLoc);			// Add location to orders
			}
		}
		else	// If no assigned target, just go to any free location
		{
			// Get new location for all 4 directions
			for(int d=0; d<TDIRECTIONS; d++)
			{
				loc = state.getLocation(state.myAnts[i], d);
				// Move to direction if it's safe
				if(!state.grid[loc.row][loc.col].isWater
				   && !state.grid[loc.row][loc.col].isMyAnt()
				   && !isOrder(loc))
				{
					state.makeMove(state.myAnts[i], d);	// Perform move
					orders.push_back(loc);				// Add location to orders
					break;
				}
			}
		}
	}
	
	// Update stats on time taken and print in debug file
	double timeTaken = state.timer.getTime();
	totalDecisionTime += timeTaken;
	state.bug << "time taken: " << timeTaken << "ms" << endl;
	state.bug << "average decision time so far: " << (totalDecisionTime/numberOfTurns) << "ms" << endl << endl;
};

/** @brief Assign targets to ants depending on the situation.
 @discussion Currently, it will assign food targets to the ants.
 Only one ant is sent to each food to avoid sending multiple to a same target.
 @param
 @return
 @see calculateFoodRoutes()
 @see calculateFoodTargets()
 */
void Bot::analyzeSituation()
{
	orders.clear();
	foodRoutes.clear();
	foodTargets.clear();
	// Get all distances between ants and food
	calculateFoodRoutes();
	// Assign food targets from calculated routes
	assignFoodTargets();
	state.bug << "ants number : " << (int)state.myAnts.size() << endl;
}


/** @brief Determines whether a Location is in the list of current orders.
 @discussion Goes through the list of orders currently assigned to our ants, and returns true only if the location is one of the orders.
 @param loc The location to find in list of orders
 @return Boolean indicating whether the location was found
 */
 bool Bot::isOrder(Location loc)
{
	for(int i=0 ; i<orders.size() ; i++)
		if(orders[i] == loc)
			return true;
	return false;
}

/** @brief populates the list foodRoutes with routes and orders them from smallest to longest
 @discussion Calculates routes for every (ant;food) couple, and orders the list by ascending distance. This order will make sure only the closest routes are chosen by assignFoodTargets().
 @param
 @return
 */
void Bot::calculateFoodRoutes()
{
	for(int i=0 ; i<(int)state.food.size() ; i++)
	{
		for(int j=0 ; j<(int)state.myAnts.size() ; j++)
		{
			Route r = Route(state.myAnts[j], state.food[i], state.distance(state.myAnts[j], state.food[i]));
			foodRoutes.push_back(r);
		}
	}
	
	std::sort(foodRoutes.begin(), foodRoutes.end());
	
	printFoodRoutes();
}

/** @brief Assigns food targets to ants depending on food routes
 @discussion Reads the list foodRoutes and assign every ant to the closest food. Because foodRoutes is ordered by ascending distance, we only assign ants to the closest food. We also make sure to assign only one ant to each food (and one food to each ant).
 @param
 @return
 */
void Bot::assignFoodTargets()
{
	for(int i=0 ; i<(int)foodRoutes.size() ; i++)
	{
		if(foodTargets.find(foodRoutes[i].start) == foodTargets.end()	// if the ant food is not found in map
		   && !foodInFoodTarget(foodRoutes[i].end))				// and food is not found in map
		{
			  foodTargets.insert(std::make_pair(foodRoutes[i].start, foodRoutes[i].end));
		}
	}
	
	printFoodTargets();
}

/** @brief Determines whether the position is an ant already assigned to a target
 @discussion Goes through the map foodTargets and returns true only if the given location is found.
 @param loc Location we're looking for in the map foodTargets
 @return Boolean indicating whether the Location has been found
 */
bool Bot::foodInFoodTarget(Location loc)
{
	std::map<Location, Location>::iterator it;
	for (it = foodTargets.begin(); it != foodTargets.end(); ++it )
		if (it->second == loc)
			return true;
	return false;
}

/** @brief Print list of food routes for debug
 @discussion Only effective if DEBUG is defined in Bug.h
 @see Bug.h
 */
void Bot::printFoodRoutes()
{
	state.bug << "FOOD ROUTES: " << (int)state.myAnts.size() << " ants ; " << (int)state.food.size() << " foods = " << (int)foodRoutes.size() << " routes" << endl;
	for(int i=0 ; i<(int)foodRoutes.size() ; i++)
	{
		state.bug << "--- Route: Distance: " << foodRoutes[i].distance << " - ant(" << foodRoutes[i].start.row << "," << foodRoutes[i].start.col<< ") food(" << foodRoutes[i].end.row << "," << foodRoutes[i].end.col << ")" << std::endl;
	}
}

/** @brief Print food targets assigned to ants
 @discussion Only effective if DEBUG is defined in Bug.h
 @see Bug.h
 */
void Bot::printFoodTargets()
{
	std::map<Location, Location>::iterator it;
	state.bug << "FOOD TARGETS: " << (int)state.myAnts.size() << " ants ; " << (int)state.food.size() << " foods = " << (int)foodTargets.size() << " targets assigned" << endl;
	for (it = foodTargets.begin(); it != foodTargets.end(); ++it )
	{
		int distance = state.distance(it->first, it->second);
		state.bug << "--- Target: Distance: " << distance << " - ant" << it->first << " food" << it->second << std::endl;
	}

}

/** @brief Determines in what direction is the target for the ant
 @discussion Given the Location of an ant and of a target, returns an int (0-3) indicating in what direction we must move the ant to get it closer to the target.

 Current implementation:
 
 - does not take care of obstacles
 
 - will move the ant NORTH or SOUTH until it is on the same row as the target
 
 - will then move EAST or WEST until it reaches the target
 
 @param1 ant	Location of the ant
 @param2 target	Location of the target
 @return Direction as an integer (0-3), or -1 if no direction was found
 @note Direction constants are defined in State.hpp
 */
int Bot::calculateDirectionToTarget(const Location ant, const Location target)
{
	// N - ant.row > target.row
	if(ant.row > target.row)
		return DIR_NORTH;
	if(ant.row < target.row)
		return DIR_SOUTH;
	if(ant.col > target.col)
		return DIR_WEST;
	if(ant.col < target.col)
		return DIR_EAST;
	return -1;	// if no direction is found
}


/* Do not touch following functions */

/** @brief Plays a single game of Ants.
 @discussion
 @param
 @return
 @note Default function provided by the AI Challenge
 */
void Bot::playGame()
{
	//reads the game parameters and sets up
	cin >> state;
	state.setup();
	endTurn();
	
	//continues making moves while the game is not over
	while(cin >> state)
	{
		state.updateVisionInformation();
		makeMoves();
		endTurn();
	}
};


/** @brief Finishes the turn
 @discussion
 @param
 @return
 @note Default function provided by the AI Challenge
*/
void Bot::endTurn()
{
	if(state.turn > 0)
		state.reset();
	state.turn++;
	
	cout << "go" << endl;
};

