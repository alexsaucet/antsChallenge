//
//  Path.hpp
//  srcXcode
//
//  Created by Alexandre on 10/01/2017.
//  Copyright © 2017 Alexandre. All rights reserved.
//

#ifndef Path_hpp
#define Path_hpp

#include <stdio.h>
#include "Location.h"


struct Path{
	Location start;
	Location end;
	int direction;
	int distance;
	
	Path(Location start, Location end, int direction, int distance);
	
	int hashCode() const
	{
		return (start.hashCode() + end.hashCode() + direction + distance);
	}
	
	friend bool operator==(const Path &p1, const Path &p2)
	{
		return ((p1.start == p2.start) && (p1.end == p2.end));
	}
	
	// Redefine operator < to allow sorting of a vector of Route
	friend bool operator<(const Path &p1, const Path &p2)
	{
		return (p1.distance < p2.distance);
	}
	
	
};


#endif /* Path_hpp */
