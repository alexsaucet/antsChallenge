//
//  Path.cpp
//  srcXcode
//
//  Created by Alexandre on 10/01/2017.
//  Copyright © 2017 Alexandre. All rights reserved.
//

#include "Path.hpp"

Path::Path(Location start, Location end, int direction, int distance)
{
	this->start		= start;
	this->end		= end;
	this->direction	= direction;
	this->distance	= distance;
}
