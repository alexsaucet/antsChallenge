//
//  Location.h
//  srcXcode
//
//  Created by Alexandre on 07/01/2017.
//  Copyright © 2017 Alexandre. All rights reserved.
//

#ifndef LOCATION_H_
#define LOCATION_H_

#define MAX_ROWS 200
#define MAX_COLS 200

#include <iostream>

#define NULL_LOCATION Location(-1,-1)

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
		
	int hashCode() const
	{
		return (row*MAX_ROWS*MAX_COLS + col);
	}
	
	friend bool operator==(const Location &l1, const Location &l2)
	{
		return ((l1.row == l2.row) && (l1.col == l2.col));
	}
	
	friend bool operator!=(const Location &l1, const Location &l2)
	{
		return !(l1 == l2);
	}
	
	friend bool operator<(const Location &l1, const Location &l2)
	{
		return (l1.hashCode() < l2.hashCode());
	}
};

std::ostream& operator<<(std::ostream &os, const Location &loc);


#endif //LOCATION_H_
