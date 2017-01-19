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
	rows = -1;
	cols = -1;
	numberOfTurns = 0;
	totalDecisionTime = 0.0;
	orders.clear();
	foodPaths.clear();
	assignedFoodTargets.clear();
	unexploredLocations.clear();
	assignedUnexploredTargets.clear();
	exploredGrid.clear();
};

/**
 Determines where to move each ant depending on different factors, and makes 
 the moves.
 */
void Bot::makeMoves()
{
	// First call: set rows and cols, and initially add every location to the set of unexplored locations
	// Also, initially set every location of the explored grid to a '.' square (accessible)
	if(rows == -1 && cols == -1)
	{
		rows = state.rows;
		cols = state.cols;
		for(int i=0 ; i<rows ; i++)
			for(int j=0 ; j<cols ; j++)
				unexploredLocations.insert(Location(i,j));
		initExploredGrid();
	}
	
	state.bug << "turn " << state.turn << ":" << endl;
	state.bug << state << endl;	// Display map in text
	
	numberOfTurns++;
	
	updateExploredGrid();	// update the explored grid
	
	analyzeSituation();		// Calculates targets
	
	moveAntsToFoodOrders();			// Assigns ants to food, from the assignedFoodTarget map
	moveAntsToUnexploredOrders();	// Assigns ants to unexplored locations, from the assignedUnexploredTarget map

	//picks out moves for each ant
	for(int i=0; i<(int)state.myAnts.size(); i++)
	{
		Location loc = state.myAnts[i];	// location of current ant
		
		// If the ant has no assigned food target, explore randomly:
		if(assignedFoodTargets.find(loc) == assignedFoodTargets.end()
		   && assignedUnexploredTargets.find(loc) == assignedUnexploredTargets.end())	// If no assigned target to this ant, just go to any free location
		{
			// Get new location for all 4 directions
			for(int d=0; d<TDIRECTIONS; d++)
			{
				loc = state.getLocation(state.myAnts[i], d);
				// Move to direction if it's safe
				if(!isWater(Location(loc.row,loc.col))
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
}

bool Bot::hasAntAssignedFoodTarget(Location loc)
{
	return (assignedFoodTargets.find(loc) != assignedFoodTargets.end());
}

/*
 Initialize the explored grid to all visible accessible squares. It will then be updated at every turn to add the walls (water). Other elements are not tracked here as they move during the game.
 */
void Bot::initExploredGrid()
{
	exploredGrid = std::vector<std::vector<Square> >(rows, vector<Square>(cols, Square()));
	for(int i=0 ; i<rows ; i++)
		for(int j=0 ; j<cols ; j++)
			exploredGrid[i][j].isVisible = 1;
}

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
	orders.clear();		// Empty the list of orders
	foodPaths.clear();	// Empty food paths
	assignedFoodTargets.clear();
	assignedUnexploredTargets.clear();
	calculateFoodPaths();
	assignFoodPaths();
	for(int i=0 ; i<state.myAnts.size() ; i++)
		if(!hasAntAssignedFoodTarget(state.myAnts[i]))
			calculateClosestUnexplored(state.myAnts[i]);
	printAssignedFoodTargets();
	printAssignedUnexploredPaths();
}

/* Every turn, add newly discovered water locations to the grid of explored locations.
 Also, remove newly discovered locations from the unexploredLocations set. */
void Bot::updateExploredGrid()
{
	for(int i=0 ; i<rows ; i++)
	{
		for(int j=0 ; j<cols ; j++)
		{
			if(state.grid[i][j].isWater)
				exploredGrid[i][j].isWater = 1;
			if(state.grid[i][j].isVisible)
				unexploredLocations.erase(Location(i,j));
		}
	}
	
	printExploredGrid();
}

void Bot::printExploredGrid()
{
	state.bug << "    ";
	for(int i=0 ; i<cols ; i++)
	{
		if(i%5 == 0)
		{
			if(i < 10)
				state.bug << i << "    ";
			else if(i < 100)
				state.bug << i << "   ";
			else
				state.bug << i << "  ";
		}
	}
	state.bug << endl;
	for(int row=0; row<rows; row++)
	{
		if(row < 10)
			state.bug << row << "   ";
		else if(row < 100)
			state.bug << row << "  ";
		else
			state.bug << row << " ";
		for(int col=0; col<state.cols; col++)
		{
			if(exploredGrid[row][col].isWater)
				state.bug << '%';
			else if(exploredGrid[row][col].isFood)
				state.bug << '*';
			else if(exploredGrid[row][col].isHill)
				state.bug << (char)('A' + state.grid[row][col].hillPlayer);
			else if(exploredGrid[row][col].ant >= 0)
				state.bug << (char)('a' + state.grid[row][col].ant);
			else if(exploredGrid[row][col].isVisible)
				state.bug << '.';
			else
				state.bug << '?';
		}
		state.bug << endl;
	}
	// Col numbers on bottom of chart, every 5 col
	state.bug << "    ";
	for(int i=0 ; i<state.cols ; i++)
	{
		if(i%5 == 0)
		{
			if(i < 10)
				state.bug << i << "    ";
			else if(i < 100)
				state.bug << i << "   ";
			else
				state.bug << i << "  ";
		}
	}
	state.bug << endl;
}







/* Finds the closest unexplored Location for the given ant.
 For each ant a with no assigned target:
	- Calculate the approximate distance to unexplored locations (state.distance()).
	- Pick the closest location l to this ant
	- BFS algorithm from the unexplored location l to the ant a => find the path but this time we care about water
	- from the ant a, pick the direction getting it closer to the objective location
 - When we have the path, add the <ant, Path> to the assignedUnexploredTarget map
 
 */
void Bot::calculateClosestUnexplored(Location ant)
{
	int maxDistance = 100;
	// 2D vector representing the distances from the unexplored location l
	std::vector<std::vector<int> > distances;
	// Queue used to store locations to be visited:
	std::queue<Location> queue;
	// Target: closest unexplored location:
	Location target = NULL_LOCATION;

	// If ant already has an assigned food target, skip it:
	if(!hasAntAssignedFoodTarget(ant))
	{
		int minDistance = UINT16_MAX;
		// Find closest unexplored location from this ant (basic state.distance() distance):
		for(std::set<Location>::iterator it = unexploredLocations.begin() ; it!=unexploredLocations.end() ; ++it)
		{
			int dist = state.distance(ant, *it);
			if(dist < minDistance && !locationInUnexploredPaths(*it))
			{
				minDistance = dist;
				target = *it;
			}
		}
		if(target != NULL_LOCATION)	// BFS from target to ant to find shortest path
		{
			// Reset distances to -1, except 0 for current food tile:
			distances = std::vector<std::vector<int> >(rows, std::vector<int>(cols, -1));
			distances[target.row][target.col] = 0;
			// Empty queue and push food location to it:
			queue = std::queue<Location>();
			queue.push(target);
			// Until queue is empty: pop a from queue, add its neighbours b to queue and update distances
			while(!queue.empty())
			{
				// Get next element in queue and pop it:
				Location a = queue.front();
				queue.pop();
				// If the distance of current location to objective >= maxDistance, break;
				if(distances[a.row][a.col] >= maxDistance)
					break;
				// If current location a is the ant we're looking for (ant), stop
				if(a == ant && findPrevious(distances, a) != -1)
				{
					Path p = Path(a, target, findPrevious(distances, a), distances[a.row][a.col]);
					assignedUnexploredTargets.insert(std::make_pair(ant, p));
				}
				else
				{
					// Get locations of 4 neighbours; NULL_LOCATION is out of bounds:
					Location north, south, east, west;
					north	= state.getLocationNoWrap(a, DIR_NORTH);
					south	= state.getLocationNoWrap(a, DIR_SOUTH);
					east	= state.getLocationNoWrap(a, DIR_EAST);
					west	= state.getLocationNoWrap(a, DIR_WEST);
					// Add those locations to the queue if they're valid:
					if(isValidStepInPath(north)
					   && distances[north.row][north.col] == -1)
					{
						queue.push(north);
						distances[north.row][north.col] = distances[a.row][a.col] + 1;
					}
					if(isValidStepInPath(south)
					   && distances[south.row][south.col] == -1)
					{
						queue.push(south);
						distances[south.row][south.col] = distances[a.row][a.col] + 1;
					}
					if(isValidStepInPath(east)
					   && distances[east.row][east.col] == -1)
					{
						queue.push(east);
						distances[east.row][east.col] = distances[a.row][a.col] + 1;
					}
					if(isValidStepInPath(west)
					   && distances[west.row][west.col] == -1)
					{
						queue.push(west);
						distances[west.row][west.col] = distances[a.row][a.col] + 1;
					}
				}
			}
		}
	}
}




/* For each myAnt on the map:
	- Check if the ant has an assigned unexplored target
	- If yes, then we find if it can move in the direction that gets it closer to the unexplored location
 - If it can, we make the move
 */
void Bot::moveAntsToUnexploredOrders()
{
	for(int i=0; i<(int)state.myAnts.size(); i++)
	{
		Location loc = state.myAnts[i];	// location of current ant
		
		if(assignedUnexploredTargets.find(loc) != assignedUnexploredTargets.end())	// Move toward food if this ant has an assigned target
		{
			Path p = assignedUnexploredTargets.find(loc)->second;
			Location newLoc	= state.getLocation(loc, p.direction);	// find location of the tile we're moving to
			if(!isWater(Location(newLoc.row, newLoc.col))
			   && !isMyAnt(Location(newLoc.row, newLoc.col))
			   && !isOrder(newLoc))
			{
				state.makeMove(loc, p.direction);		// Perform move
				orders.push_back(newLoc);			// Add location to orders
			}
		}
	}
}


/* For each myAnt on the map:
	- Check if the ant has an assigned food target
	- If yes, then we find if it can move in the direction that gets it closer to the food
		- If it can, we make the move
 */
void Bot::moveAntsToFoodOrders()
{
	for(int i=0; i<(int)state.myAnts.size(); i++)
	{
		Location loc = state.myAnts[i];	// location of current ant
		
		if(assignedFoodTargets.find(loc) != assignedFoodTargets.end())	// Move toward food if this ant has an assigned target
		{
			Path p = assignedFoodTargets.find(loc)->second;
			Location newLoc	= state.getLocation(loc, p.direction);	// find location of the tile we're moving to
			if(!isWater(Location(newLoc.row, newLoc.col))
			   && !isMyAnt(Location(newLoc.row, newLoc.col))
			   && !isOrder(newLoc))
			{
				state.makeMove(loc, p.direction);		// Perform move
				orders.push_back(newLoc);			// Add location to orders
			}
		}
	}
}

/* For each food tile f:
		- Calculate the grid of distances to f
		- If we see an ant a, it will be the closest to f
			- Save the location of a and f, with the distance and direction a has to take
			in foodTargets (modify it if necessary)
		- If no ant is seen, don't do anything
 Note: limits range to 50 moves not to take too much time
 */
void Bot::calculateFoodPaths()
 {
	 const int maxDistance = 50;
	 // 2D vector representing the distances from the food tile f
	 std::vector<std::vector<int> > distances;
	 // Queue used to store locations to be visited:
	 std::queue<Location> queue;
	 // Tmp Location used to store the current food tile location:
	 Location food;
	 // For each food tile f currently visible on the map (stop if my ant number is >= number of food path assigned)
	 for(int f=0 ; f<(int)state.food.size() ; f++)
	 {
		 food = state.food[f];
		 // Reset distances to -1, except 0 for current food tile:
		 distances = std::vector<std::vector<int> >(rows, std::vector<int>(cols, -1));
		 distances[food.row][food.col] = 0;
		 // Empty queue and push food location to it:
		 queue = std::queue<Location>();
		 queue.push(food);
		 // Until queue is empty: pop a from queue, add its neighbours b to queue and update distances
		 while(!queue.empty())
		 {
			 // Get next element in queue and pop it:
			 Location a = queue.front();
			 queue.pop();
			 // If the distance of current location to objective >= maxDistance, break;
			 if(distances[a.row][a.col] >= maxDistance)
				 break;
			 // It a is an ant, stop; we'll assign this one to this food tile
			 for(int iAnts=0 ; iAnts<(int)state.myAnts.size() ; iAnts++)
			 {
				 if(state.myAnts[iAnts] == a
					&& !isOrder(a))
				 {
					 int directionToFood = findPrevious(distances, a);
					 if(directionToFood != -1)	// Valid direction: assign food to ant
					 {
						 Path p = Path(a, food, directionToFood, distances[a.row][a.col]);
						 foodPaths.push_back(p);
					 }
				 }
			 }
			 // Get locations of 4 neighbours; NULL_LOCATION is out of bounds:
			 Location north, south, east, west;
			 north	= state.getLocationNoWrap(a, DIR_NORTH);
			 south	= state.getLocationNoWrap(a, DIR_SOUTH);
			 east	= state.getLocationNoWrap(a, DIR_EAST);
			 west	= state.getLocationNoWrap(a, DIR_WEST);
			 // Add those locations to the queue if they're valid:
			 if(isValidStepInPath(north)
				&& distances[north.row][north.col] == -1)
			 {
				 queue.push(north);
				 distances[north.row][north.col] = distances[a.row][a.col] + 1;
			 }
			 if(isValidStepInPath(south)
				&& distances[south.row][south.col] == -1)
			 {
				 queue.push(south);
				 distances[south.row][south.col] = distances[a.row][a.col] + 1;
			 }
			 if(isValidStepInPath(east)
				&& distances[east.row][east.col] == -1)
			 {
				 queue.push(east);
				 distances[east.row][east.col] = distances[a.row][a.col] + 1;
			 }
			 if(isValidStepInPath(west)
				&& distances[west.row][west.col] == -1)
			 {
				 queue.push(west);
				 distances[west.row][west.col] = distances[a.row][a.col] + 1;
			 }
		 }
	 }
	 // Once we have the path for every (food;ant) couple, we sort the list by ascending distance:
	 std::sort(foodPaths.begin(), foodPaths.end());
 }

/* Go through elements of the vector paths and for each path, add it to the food targets if ant and food are not already assigned.
 Note that the vector paths is sorted in ascending distance, so shortest paths will be assigned first*/
void Bot::assignFoodPaths()
{
	for(int i=0 ; i<(int)foodPaths.size() ; i++)
	{
		if(assignedFoodTargets.find(foodPaths[i].start) == assignedFoodTargets.end()
		   && !foodInFoodPaths(foodPaths[i].end))
		{
			assignedFoodTargets.insert(std::make_pair(foodPaths[i].start, foodPaths[i]));
		}
	}
}

bool Bot::locationInUnexploredPaths(Location loc)
{
	std::map<Location, Path>::iterator it;
	for (it = assignedUnexploredTargets.begin(); it != assignedUnexploredTargets.end(); ++it )
		if (it->second.end == loc)
			return true;
	return false;
}

bool Bot::foodInFoodPaths(Location loc)
{
	std::map<Location, Path>::iterator it;
	for (it = assignedFoodTargets.begin(); it != assignedFoodTargets.end(); ++it )
		if (it->second.end == loc)
			return true;
	return false;
}

void Bot::printFoodPaths()
{
	state.bug << "FOOD PATHS: " << (int)state.myAnts.size() << " ants ; " << (int)state.food.size() << " foods = " << (int)foodPaths.size() << " paths" << endl;
	for(int i=0 ; i<(int)foodPaths.size() ; i++)
	{
		state.bug << "- Path: Distance: " << foodPaths[i].distance << " - ant" << foodPaths[i].start <<  " food" << foodPaths[i].end << " Direction: " << foodPaths[i].direction << std::endl;
	}
}


void Bot::printAssignedFoodTargets()
{
	std::map<Location, Path>::iterator it;
	state.bug << "ASSIGNED FOOD PATHS: " << (int)state.myAnts.size() << " ants ; " << (int)state.food.size() << " foods = " << (int)assignedFoodTargets.size() << " targets assigned" << endl;
	for (it = assignedFoodTargets.begin(); it != assignedFoodTargets.end(); ++it )
	{
		state.bug << "--- Target: Distance: " << it->second.distance << " - ant" << it->first << " food" << it->second.end << " direction:" << it->second.direction << std::endl;
	}
	
}



void Bot::printAssignedUnexploredPaths()
{
	std::map<Location, Path>::iterator it;
	state.bug << "ASSIGNED UNEXPLORED PATHS: " << (int)assignedUnexploredTargets.size() << " targets assigned" << endl;
	for (it = assignedUnexploredTargets.begin(); it != assignedUnexploredTargets.end(); ++it )
	{
		state.bug << "--- Target: Distance: " << it->second.distance << " - ant" << it->first << " unexplored" << it->second.end << " direction:" << it->second.direction << std::endl;
	}
	
}


void Bot::printDistances(std::vector<std::vector<int> > distances)
{
	for(int i=0 ; i<rows ; i++)
	{
		for(int j=0 ; j<cols ; j++)
		{
			if(distances[i][j] > 100)
				state.bug << distances[i][j] << " ";
			else if(distances[i][j] < 0 || distances[i][j] > 10)
				state.bug << distances[i][j] << "  ";
			else
				state.bug << distances[i][j] << "   ";
		}
		state.bug << endl;
	}
	state.bug << endl;
}

// Go through foodPath and return true if this ant location is found as a start point of a path
/*bool Bot::isAntInFoodPaths(Location loc)
{
	if(foodPaths.find(loc) != foodPaths.end())
		return true;
	return false;
}*/

int Bot::findPrevious(std::vector<std::vector<int> > distances, Location loc)
{
	int currentDistance = distances[loc.row][loc.col];
	int tmpDist;	// Will store distance at the neighbour locations
	Location neighbour;
	
	neighbour	= state.getLocationNoWrap(loc, DIR_NORTH);	// NORTH
	if(neighbour != NULL_LOCATION)
	{
		tmpDist = distances[neighbour.row][neighbour.col];
		if(tmpDist < currentDistance && tmpDist != -1)
			return DIR_NORTH;
	}
	
	neighbour	= state.getLocationNoWrap(loc, DIR_SOUTH);	// SOUTH
	if(neighbour != NULL_LOCATION)
	{
		tmpDist = distances[neighbour.row][neighbour.col];
		if(tmpDist < currentDistance && tmpDist != -1)
			return DIR_SOUTH;
	}
	
	neighbour	= state.getLocationNoWrap(loc, DIR_EAST);	// EAST
	if(neighbour != NULL_LOCATION)
	{
		tmpDist = distances[neighbour.row][neighbour.col];
		if(tmpDist < currentDistance && tmpDist != -1)
			return DIR_EAST;
	}
	
	neighbour	= state.getLocationNoWrap(loc, DIR_WEST);	// WEST
	if(neighbour != NULL_LOCATION)
	{
		tmpDist = distances[neighbour.row][neighbour.col];
		if(tmpDist < currentDistance && tmpDist != -1)
			return DIR_WEST;
	}
	
	return -1;	// If no direction found
}


/* Not valid tile location:
	- loc == NULL_LOCATION
	- water: state.grid[row][col].isWater - Can't step on water
	//false- myAnt: state.grid[row][col].isMyAnt()	- 2 ants on a same tile would die
	//deleter- unvisible tiles:	state.grid[row][col].isVisible - No ant will be in the unvisible space
 First version didn't consider invisible tiles as valid. Changed this and now consider that invisible space is all accessible space (in the invisible space, the exploredGrid is the grid taken into consideration). Note that the function calculateFoodPaths() now has a limit of distance it will explore to try and find the paths, or it could potentially take a very long time on huge maps.
 */
bool Bot::isValidStepInPath(Location loc)
{
	if(loc == NULL_LOCATION || isWater(loc))
		return false;
	return true;
}

bool Bot::isMyAnt(Location loc)
{
	return state.grid[loc.row][loc.col].isMyAnt();
}

bool Bot::isWater(Location loc)
{
	return exploredGrid[loc.row][loc.col].isWater;
}

bool Bot::isVisible(Location loc)
{
	return state.grid[loc.row][loc.col].isVisible;
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

