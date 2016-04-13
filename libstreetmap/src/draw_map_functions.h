
/* 
 * File:   draw_map_functions.h
 * Author: zhengz28
 *
 * Created on February 8, 2016, 11:10 AM
 */

#ifndef DRAW_MAP_FUNCTIONS_H
#define DRAW_MAP_FUNCTIONS_H
#include <sstream>
#include "StreetsGraph.h"
#include "graphics.h"
#include "m1.h"
#include "m3.h"

//Given a segment id, then draw this segment on window
void draw_street_segment(unsigned streetSegmentID);

//Highlight given street segment given segment ID
void highlight_the_segment(unsigned streetSegmentID);

//Given a point of interest id, then draw this on window
void draw_POI(unsigned POI_ID);

//draw two way symbol
void draw_double_arrow(std::pair<double, double> point1, std::pair<double, double> point2);

//draw the one way symbol
void draw_single_arrow(std::pair<double, double> point1, std::pair<double, double> point2);

//Given id ,draw features(ie. sea,forests)
void draw_feature(unsigned featuresID);

//hightlight given position
void hightlight_point(double xy[2]);

//get a proper road width to draw depending on current view
int get_linewidth(t_bound_box current);

//click on intersection and output info
//draw one intersection
void draw_intersection(unsigned i);

//draw the symbol when any point on the map is clicked
void draw_clicked_symbol(float x, float y);

//draw the points that the route connects 
void draw_point(float x, float y);

//highlights the route 
void highlight_route(vector<unsigned> path,bool turn_on);

//print out the information of the closest intersection to the terminal
//if user clicked on the intersection, return intersection id 
//else, return -1
int display_intersection_info(float x, float y);

//print out the information of the closest intersection to the terminal
//if user clicked on the intersection, return intersection id 
//else, return -1
int display_poi_info(float x, float y);

//return (x,y) of the intersection that should be highlighted from the mouse click
std::pair<float, float> get_last_clicked_inter(float x, float y);

//helper function to draw the street segment
void draw_the_segment(unsigned streetSegmentID);

//draw the text for main highways
void draw_motorway_text(unsigned streetSegmentID);

//draw the text for roads at every curve point
void draw_segment_text(unsigned streetSegmentID, int size);

//show information about clicked intersection
void display_intersection_information_on_screen(unsigned intersection_id);

//show information about a clicked point of interest 
void display_poi_information_on_screen(unsigned poi_id);
#endif /* DRAW_MAP_FUNCTIONS_H */

