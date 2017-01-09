//
//  Location.cpp
//  srcXcode
//
//  Created by Alexandre on 08/01/2017.
//  Copyright © 2017 Alexandre. All rights reserved.
//

#include <stdio.h>
#include "Location.h"

std::ostream& operator<<(std::ostream &os, const Location &loc)
{
	os << "(" << loc.row << "," << loc.col << ")";
	
	return os;
};