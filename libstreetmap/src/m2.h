#pragma once
#include <string>
//#include <boost/algorithm/string/classification.hpp>
//#include <boost/algorithm/string/case_conv.hpp>
//#include <boost/algorithm/string/erase.hpp>
//#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include "draw_map_functions.h"
#include "m3.h"
#include <algorithm> //for the sort function used to sort lakes and green space based on ascending area 
#include <vector>
#include <iostream>
#include <cstdlib>
#include <readline/readline.h>
#include <readline/history.h>


// Draws the map. You can assume your load_map (string map_name)
// function is called before this function in the unit tests.
// Your main () program should do the same.
void draw_map();

//handle button clicks
void act_on_button_press(float x, float y, t_event_buttonPressed event);

//things to do when any key is pressed
void act_on_keypress(char key_pressed, int keysym);

//create all buttons we need in GUI
void create_all_buttons();

//things to do when mouse move
void act_on_mousemove(float x, float y);

//Function used to wait disable all buttons except load button
void wait_for_load_map();

//enable all buttons except load button
void enable_all_buttons();
