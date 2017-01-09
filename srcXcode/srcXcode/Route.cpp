//
//  Route.cpp
//  srcXcode
//
//  Created by Alexandre on 07/01/2017.
//  Copyright © 2017 Alexandre. All rights reserved.
//

#include "Route.hpp"


Route::Route(Location start, Location end, int distance)
{
	this->start		= start;
	this->end		= end;
	this->distance	= distance;
}

int Route::hashCode() {
	return start.hashCode() + end.hashCode();
}
