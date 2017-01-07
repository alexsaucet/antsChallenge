//
//  Location.h
//  srcXcode
//
//  Created by Alexandre on 07/01/2017.
//  Copyright © 2017 Alexandre. All rights reserved.
//

#ifndef LOCATION_H_
#define LOCATION_H_

/*
 struct for representing locations in the grid.
 */
struct Location
{
	int row, col;
	
	Location()
	{
		row = col = 0;
	};
	
	Location(int r, int c)
	{
		row = r;
		col = c;
	};
};

#endif //LOCATION_H_
