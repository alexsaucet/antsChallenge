//
//  Square.h
//  srcXcode
//
//  Created by Alexandre on 07/01/2017.
//  Copyright © 2017 Alexandre. All rights reserved.
//

#ifndef SQUARE_H_
#define SQUARE_H_

#include <vector>

/*
 struct for representing a square in the grid.
 */
struct Square
{
	bool isVisible, isWater, isHill, isFood;
	int ant, hillPlayer;
	std::vector<int> deadAnts;
	
	Square();
	
	//resets the information for the square except water information
	void reset();
	
	bool isMyAnt();
	
};

#endif //SQUARE_H_
