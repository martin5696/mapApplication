#pragma once
#include <algorithm>
#include <vector>
#include <string>
#include <queue>
#include <map>

#include <math.h>
#include "draw_map_functions.h"
#include "m1.h"
#include "StreetsGraph.h"

// Returns a path (route) between the start intersection and the end 
// intersection, if one exists. If no path exists, this routine returns 
// an empty (size == 0) vector. If more than one path exists, the path 
// with the shortest travel time is returned. The path is returned as a vector 
// of street segment ids; traversing these street segments, in the given order,
// would take one from the start to the end intersection.
std::vector<unsigned> find_path_between_intersections(unsigned
        intersect_id_start, unsigned intersect_id_end);


// Returns the time required to travel along the path specified in minutes. 
// The path is a vector of street segment ids, and this function can 
// assume the vector either forms a legal path or has size == 0.
// The travel time is the sum of the length/speed-limit of each street 
// segment, plus 15 seconds per turn implied by the path. A turn occurs
// when two consecutive street segments have different street names.
double compute_path_travel_time(const std::vector<unsigned>& path);


// Returns the shortest travel time path (vector of street segments) from 
// the start intersection to a point of interest with the specified name.
// If no such path exists, returns an empty (size == 0) vector.
std::vector<unsigned> find_path_to_point_of_interest(unsigned
        intersect_id_start, std::string point_of_interest_name);

std::vector<std::string> print_directions(std::vector<unsigned> path);

void singleSourceDijkstra(unsigned intersect_id_start, const std::vector<unsigned>& intersect_id_to,std::unordered_map<unsigned,std::unordered_map<unsigned,double>>& cost_map);