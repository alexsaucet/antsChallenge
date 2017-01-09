//
//  Bug.h
//  srcXcode
//
//  Created by Alexandre on 07/01/2017.
//  Copyright © 2017 Alexandre. All rights reserved.
//

#ifndef BUG_H_
#define BUG_H_

#include <fstream>

#ifndef DEBUG
#define DEBUG
#endif

/**
 @brief struct for debugging
 @discussion This is gross but can be used pretty much like an ofstream.
 @note DEBUG must be defined or messages will not be output to the file
 @code Bug bug;
 
 bug.open("./debug.txt");
 
 bug << "State: " << state << endl;
 
 bug.close();
 */
struct Bug
{
	std::ofstream file;
	
	Bug()
	{
		
	};
	
	//opens the specified file
	inline void open(const std::string &filename)
	{
#ifdef DEBUG
		file.open(filename.c_str());
#endif
	};
	
	//closes the ofstream
	inline void close()
	{
#ifdef DEBUG
		file.close();
#endif
	};
};

//output function for endl
inline Bug& operator<<(Bug &bug, std::ostream& (*manipulator)(std::ostream&))
{
#ifdef DEBUG
	bug.file << manipulator;
#endif
	
	return bug;
};

//output function
template <class T>
inline Bug& operator<<(Bug &bug, const T &t)
{
#ifdef DEBUG
	bug.file << t;
#endif
	
	return bug;
};

#endif //BUG_H_

