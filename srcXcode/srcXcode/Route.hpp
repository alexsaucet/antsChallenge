//
//  Route.hpp
//  srcXcode
//
//  Created by Alexandre on 07/01/2017.
//  Copyright © 2017 Alexandre. All rights reserved.
//

#ifndef Route_hpp
#define Route_hpp

#include <stdio.h>
#include "Location.h"

/** @brief A Route epresents a route between two points.
 @discussion Contains a start and end Location, as well as the distance between the two.
 
 - If route between ant and food : start=ant ; end=food */
struct Route{
	Location start;
	Location end;
	int distance;
	
	Route(Location start, Location end, int distance);
	int hashCode();		// Calculate the hashcode as start.hashcode()+end.hashcode()
	
	friend bool operator==(const Route &r1, const Route &r2)
	{
		return ((r1.start == r2.start) && (r1.end == r2.end));
	}
	
	// Redefine operator < to allow sorting of a vector of Route
	friend bool operator<(const Route &r1, const Route &r2)
	{
		return (r1.distance < r2.distance);
	}
};

#endif /* Route_hpp */
