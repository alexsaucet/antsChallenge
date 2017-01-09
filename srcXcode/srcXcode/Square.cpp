//
//  Square.cpp
//  srcXcode
//
//  Created by Alexandre on 07/01/2017.
//  Copyright © 2017 Alexandre. All rights reserved.
//

#include <stdio.h>
#include "Square.h"
#include "State.hpp"

Square::Square()
{
	isVisible = isWater = isHill = isFood = 0;
	ant = hillPlayer = -1;
};

//resets the information for the square except water information
void Square::reset()
{
	isVisible = 0;
	isHill = 0;
	isFood = 0;
	ant = hillPlayer = -1;
	deadAnts.clear();
};

// Is this square occupied by one of my ants?
bool Square::isMyAnt()
{
	if(ant == MY_ANT)
		return 1;
	return 0;
}


