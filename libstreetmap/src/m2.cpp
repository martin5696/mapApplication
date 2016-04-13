/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */



#include "m2.h"
#include "DrawStatus.h"
#define RIGHT_CLICK 3
#define BUTTON_NUM 19
#define ON true
#define OFF false
#define EMPTY 0
#define LOAD_MAP 1
#define FIND_STREET 2
#define FIND_ROUTE_POINT1 3
#define FIND_ROUTE_POINT2 4
#define FIND_POI_POINT1 5
#define FIND_POI_POINT2 6

using namespace std;

//initialize draw_status
DrawStatus draw_status = {0.0, 0.0, 0, false, false, false, false, false, false, false, false};
extern StreetsGraph *streetsHelper;

//for M3 autocomplete
vector<string> potential_intersections;
vector<string>potential_POI;
string current_input;
int mode; //0 for intersections path, 1 for POI path. used in auto fill
//have a variable for mode during auto complete (POI/intersections). use in Tab key

/* -----------------Helper function to draw the map-------------*/
static void draw_screen();
//function draws all street segments depending on current zoom in rate 
static void draw_all_street_segments();
//function to draw all intersections
static void draw_all_intersections();
static void draw_all_features();
static void draw_all_POI();
static void draw_all_marks();
static bool highlight_street(string name);


/* -----------------Buttons Call Back Functions-------------*/
static void find_street_intersections_callback(void (*drawscreen_ptr) (void));
static void load_map_callback(void (*drawscreen_ptr) (void));
static void show_poi_callback(void (*drawscreen_ptr) (void));
static void show_features_callback(void (*drawscreen_ptr) (void));
static void clear_callback(void (*drawscreen_ptr) (void));
static void route_callback(void (*drawscreen_ptr) (void));
static void POI_route_callback(void (*drawscreen_ptr) (void));
static void show_help_callback(void (*drawscreen_ptr) (void));

/*-----------------Map Functions ------------------------*/
//a function used in key press call back function to determine what thing to do after
//enter key is pressed
static void enterPressedEvents();
//load the map depending on the name 
static bool load_map_func();
//search all data to find place with certain name
static bool find_street_intersections_func();
//clear all highlight places and marks
static void clear_map();
//function that actually call the find path function
static void route_func();
//Clear the path found
static void clear_route();

static void read_inter1_from_text_box();

static void read_poi_from_text_box();

static void read_inter2_from_text_box();

//Function used to wait disable all buttons except load button

void wait_for_load_map() {
    //disenable all buttons except load and exit
    for (int i = 0; i < BUTTON_NUM - 1; i++)
        enable_or_disable_button(i, OFF);
}

/*  Top draw map function called to create the window
 *  Layer : Feature < Streets < intersections < clicked_symbol < marks 
 */

void draw_map() {
    init_graphics("Hello Map", t_color(233, 229, 220));
    create_all_buttons();
    wait_for_load_map();
    //randomly set coordinates before loading map
    set_visible_world(0, 0, 1000, 1000);
    update_message("Please load a map first");
    event_loop(act_on_button_press, act_on_mousemove, act_on_keypress, draw_screen);
    close_graphics();
}


//The actual function to draw the map

static void draw_screen() {
    if (draw_status.successLoaded) {
        clearscreen();
        draw_all_features();
        //draw segments
        draw_all_street_segments();
        //draw intersection
        draw_all_intersections();
        //draw click symbol 
        draw_clicked_symbol(draw_status.last_clicked_x, draw_status.last_clicked_y);
        //draw intersection points along the path 

        //if pois are required, draw them 
        if (draw_status.showPOI)
            draw_all_POI();
        draw_all_marks();
    }
}

//void draw_all_legends(){
//    draw_legend();
//}


//function draws all street segments depending on current zoom in rate 

static void draw_all_street_segments() {
    for (unsigned segmentID = 0; segmentID < getNumberOfStreetSegments(); segmentID++) {
        draw_street_segment(segmentID);
    }
}

static void draw_all_features() {
    t_bound_box currentView = get_visible_world();
    double currentAspect = currentView.get_width() * currentView.get_height();
    double maxAspect = streetsHelper -> get_max_width() * streetsHelper -> get_max_height();
    auto & features = streetsHelper -> get_featureSort();

    if (getNumberOfFeatures() == 0)
        return;

    for (unsigned i = (features.lakeAndGreenSpaceAndIslandsAndShoreline.size() - 1); i > 0; i--) {
        draw_feature(features.lakeAndGreenSpaceAndIslandsAndShoreline[i]);
    }
    for (unsigned i = 0; i < features.beach.size(); i++) {
        draw_feature(features.beach[i]);
    }
    for (unsigned i = 0; i < features.unknowns.size(); i++) {
        draw_feature(features.unknowns[i]);
    }
    double aspectRatio = (maxAspect / currentAspect);
    // if zoom in rate is big enough, draw more details features
    if (aspectRatio >= 100 && draw_status.showFeatures) {
        for (unsigned i = 0; i < features.freshWater.size(); i++) {
            draw_feature(features.freshWater[i]);
        }
        for (unsigned i = 0; i < features.buildings.size(); i++) {
            draw_feature(features.buildings[i]);
        }
    }
}

static void draw_all_POI() {
    for (unsigned pointOfInterestID = 0; pointOfInterestID < getNumberOfPointsOfInterest(); pointOfInterestID++) {
        draw_POI(pointOfInterestID);
    }
}
//function that draws all intersections

static void draw_all_intersections() {
    for (unsigned i = 0; i < getNumberOfIntersections(); i++) {
        draw_intersection(i);
    }

}

//function reacting to button clicks

void act_on_button_press(float x, float y, t_event_buttonPressed event) {
    draw_status.last_clicked_x = x;
    draw_status.last_clicked_y = y;

    draw_screen();
    if (draw_status.successLoaded) {
        //if user clicked on a intersection
        draw_clicked_symbol(x, y);
        int intersectionClicked = display_intersection_info(x, y);
        int poiClicked = -1;

        //If POIs are shown, then start to search clicked poi
        if (draw_status.showPOI)
            poiClicked = display_poi_info(x, y);

        if (intersectionClicked != -1) {
            //button = 3 signifies a right click 
            if (event.button == RIGHT_CLICK && draw_status.showRoute) {
                //Add the intersection to find the path
                //Avoid repeat click on same intersection
                if (draw_status.pathLookingFor.size() != 0)
                    if (unsigned(intersectionClicked) == draw_status.pathLookingFor[0])
                        return;

                draw_status.pathLookingFor.push_back(intersectionClicked);
                if (draw_status.pathLookingFor.size() < 2) {
                    //if the user has not yet picked 2 points
                    //store the newly clicked point into the vector 

                    //close the input of intersection 1 and turn to intersection 2
                    show_text_box(ON);
                    draw_status.enterPressedEvent = FIND_ROUTE_POINT2;
                    update_message("Please input the second intersection name");

                    return;
                } else if (draw_status.pathLookingFor.size() == 2) {
                    route_func();
                    return;
                } else { // the intersection clicked on is more than 2 , restart
                    draw_status.pathLookingFor.clear();
                    draw_status.pathLookingFor.push_back(intersectionClicked);
                    update_message("Please input the second intersection name");

                    //clear the previous route
                    clear_route();
                    show_text_box(ON);
                    show_route_plane_window(OFF);
                    draw_status.enterPressedEvent = FIND_ROUTE_POINT2;
                }
            } else if (event.button == 1) {
                if (streetsHelper->is_intersection_highlighted(intersectionClicked)) {
                    draw_status.marks.push_back(std::pair<double, double> (x, y));
                    //If click on intersection
                    if (streetsHelper->is_intersection_highlighted(intersectionClicked)) {
                        draw_status.marks.push_back(std::pair<double, double> (x, y));
                        draw_status.highlighted_intersections.push_back(intersectionClicked);
                        draw_screen();
                    } else {
                        // if clicked position is already highlight , then dis-highlight it
                        auto position =
                                std::find(draw_status.marks.begin(), draw_status.marks.end(), streetsHelper->get_intersection_position(intersectionClicked));
                        draw_status.marks.erase(position);
                        draw_screen();
                    }
                    //Show information on a box at the bottom of screen
                    display_intersection_information_on_screen(intersectionClicked);
                }
            }
        } else if (poiClicked != -1) {
            display_poi_information_on_screen(poiClicked);
        }//if control is pressed,then mark on this place
        else if (event.ctrl_pressed) {
            draw_status.marks.push_back(std::pair<double, double> (x, y));
            draw_screen();
        }
    }
}
//things to do when any key is pressed

void act_on_keypress(char key_pressed, int keysym) {
    //if user input 'enter' on text box,turn off text box and 
    //disable keyboard input
    if (key_pressed == '\r') {

        enterPressedEvents();

    }//start autofill, output the 5 top results
    else if (key_pressed == '\t') {
        auto& intersectionNames = streetsHelper->get_intersectionNames();
        auto& POINames = streetsHelper->get_POINames();
        //clear the potential intersections vector from previous auto fills
        potential_intersections.clear();
        potential_POI.clear();
        //clear out the shift keys
        for (unsigned i = 0; i < current_input.length(); i++) {
            if ((current_input.at(i) != '.')&&(current_input.at(i) != '>')&&(current_input.at(i) != '<')&&(current_input.at(i) != '/')&&(current_input.at(i) != '&')&&(current_input.at(i) != ' ')&&(current_input.at(i) != 'a')&&(current_input.at(i) != 'b')&&(current_input.at(i) != 'c')&&(current_input.at(i) != 'd')&&(current_input.at(i) != 'e')&&(current_input.at(i) != 'f')&&(current_input.at(i) != 'g')&&(current_input.at(i) != 'h')&&(current_input.at(i) != 'i')&&(current_input.at(i) != 'j')&&(current_input.at(i) != 'k')&&(current_input.at(i) != 'l')&&(current_input.at(i) != 'm')&&(current_input.at(i) != 'n')&&(current_input.at(i) != 'o')&&(current_input.at(i) != 'p')&&(current_input.at(i) != 'q')&&(current_input.at(i) != 'r')&&(current_input.at(i) != 's')&&(current_input.at(i) != 't')&&(current_input.at(i) != 'u')&&(current_input.at(i) != 'v')&&(current_input.at(i) != 'w')&&(current_input.at(i) != 'x')&&(current_input.at(i) != 'y')&&(current_input.at(i) != 'z')&&(current_input.at(i) != 'A')&&(current_input.at(i) != 'B')&&(current_input.at(i) != 'C')&&(current_input.at(i) != 'D')&&(current_input.at(i) != 'E')&&(current_input.at(i) != 'F')&&(current_input.at(i) != 'G')&&(current_input.at(i) != 'H')&&(current_input.at(i) != 'I')&&(current_input.at(i) != 'J')&&(current_input.at(i) != 'K')&&(current_input.at(i) != 'L')&&(current_input.at(i) != 'M')&&(current_input.at(i) != 'N')&&(current_input.at(i) != 'O')&&(current_input.at(i) != 'P')&&(current_input.at(i) != 'Q')&&(current_input.at(i) != 'R')&&(current_input.at(i) != 'S')&&(current_input.at(i) != 'T')&&(current_input.at(i) != 'U')&&(current_input.at(i) != 'V')&&(current_input.at(i) != 'W')&&(current_input.at(i) != 'X')&&(current_input.at(i) != 'Y')&&(current_input.at(i) != 'Z')&&(current_input.at(i) != '0')&&(current_input.at(i) != '1')&&(current_input.at(i) != '2')&&(current_input.at(i) != '3')&&(current_input.at(i) != '4')&&(current_input.at(i) != '5')&&(current_input.at(i) != '6')&&(current_input.at(i) != '7')&&(current_input.at(i) != '8')&&(current_input.at(i) != '9')) {
                current_input.erase(i, 1);
            }
        }
        //        for (unsigned i = 0; i < current_input.size(); i++)
        //            cout << i << ": " << current_input.at(i) << endl;
        //        cout << "searching for: " << current_input << " with size: " << current_input.size() << endl;
        //intersection mode
        if (mode == 0) {
            //look for matches and put them in a vector
            for (unsigned i = 0; i < intersectionNames.size(); i++) {
                size_t found = intersectionNames.at(i).find(current_input);
                //if it's a match
                if (found != string::npos) {
                    potential_intersections.push_back(intersectionNames.at(i));
                }
            }
            //put the first one in the matches vector on the box if there is one
            if (potential_intersections.size() > 0) {
                string current = read_text_box();
                for (unsigned i = 0; i < current.size(); i++)
                    write_in_text_box('\b');
                string inter_to_fill = potential_intersections.at(0);
                for (unsigned i = 0; i < inter_to_fill.size(); i++)
                    write_in_text_box(inter_to_fill.at(i));
                //put the newly filled box into current_input
                current_input = read_text_box();
            }

            //draw 5 potential intersections below the search box
            settextattrs(12, 0);
            t_bound_box current_view = get_visible_world();
            double width = 0.4 * current_view.get_width();
            double height = 0.5 * current_view.get_height();
            t_point bottomLeft(0, height * 0.8);
            bottomLeft = bottomLeft + current_view.bottom_left();
            t_bound_box Info_box(bottomLeft, width, height);
            setcolor(WHITE);
            fillrect(Info_box);
            setcolor(BLACK);

            //            //draw box's boundary
            t_point middle = Info_box.get_center();
            setlinewidth(5);
            t_point bottom_left = Info_box.bottom_left();
            t_point top_right = Info_box.top_right();

            t_point bottom_right;
            bottom_right.x = bottom_left.x + Info_box.get_width();
            bottom_right.y = bottom_left.y;

            t_point top_left;
            top_left.x = top_right.x - Info_box.get_width();
            top_left.y = top_right.y;

            drawline(top_left, top_right);
            drawline(top_left, bottom_left);
            drawline(top_right, bottom_right);
            drawline(bottom_left, bottom_right);


            double box_height = Info_box.get_height();
            double matches_size = potential_intersections.size();
            //display the names of the potential matches
            if (matches_size > 0)
                drawtext(middle.x, middle.y + box_height * 0.4, potential_intersections.at(0), width * 2, width * 2);
            if (matches_size > 1)
                drawtext(middle.x, middle.y + box_height * 0.3, potential_intersections.at(1), width * 2, width * 2);
            if (matches_size > 2)
                drawtext(middle.x, middle.y + box_height * 0.2, potential_intersections.at(2), width * 2, width * 2);
            if (matches_size > 3)
                drawtext(middle.x, middle.y + box_height * 0.1, potential_intersections.at(3), width * 2, width * 2);
            if (matches_size > 4)
                drawtext(middle.x, middle.y, potential_intersections.at(4), width * 2, width * 2);
        }//POI mode
        else if (mode == 1) {
            for (unsigned i = 0; i < POINames.size(); i++) {
                size_t found = POINames.at(i).find(current_input);
                //if it's a match
                if (found != string::npos) {
                    potential_POI.push_back(POINames.at(i));
                    //cout << current_input << " found at index: " << i << " in " << intersectionNames.at(i) << endl;
                }
            }
            //put the first one in the matches vector on the box if there is one
            if (potential_POI.size() > 0) {
                string current = read_text_box();
                for (unsigned i = 0; i < current.size(); i++)
                    write_in_text_box('\b');
                string inter_to_fill = potential_POI.at(0);
                for (unsigned i = 0; i < inter_to_fill.size(); i++)
                    write_in_text_box(inter_to_fill.at(i));
                //put the newly filled box into current_input
                current_input = read_text_box();
            }

            //draw 5 potential POI below the search box
            settextattrs(12, 0);
            t_bound_box current_view = get_visible_world();
            double width = 0.5 * current_view.get_width();
            double height = 0.5 * current_view.get_height();
            t_point bottomLeft(0, height * 0.8);
            bottomLeft = bottomLeft + current_view.bottom_left();
            t_bound_box Info_box(bottomLeft, width, height);
            setcolor(WHITE);
            fillrect(Info_box);
            setcolor(BLACK);
            t_point middle = Info_box.get_center();
            double box_height = Info_box.get_height();
            double matches_size = potential_POI.size();

            //draw box's boundary
            setlinewidth(5);
            t_point bottom_left = Info_box.bottom_left();
            t_point top_right = Info_box.top_right();

            t_point bottom_right;
            bottom_right.x = bottom_left.x + Info_box.get_width();
            bottom_right.y = bottom_left.y;

            t_point top_left;
            top_left.x = top_right.x - Info_box.get_width();
            top_left.y = top_right.y;

            drawline(top_left, top_right);
            drawline(top_left, bottom_left);
            drawline(top_right, bottom_right);
            drawline(bottom_left, bottom_right);
            
//            box_height = Info_box.get_height();
//            matches_size = potential_intersections.size();
            //display the names of the potential matches
            if (matches_size > 0)
                drawtext(middle.x, middle.y + box_height * 0.4, potential_POI.at(0), width * 2, width * 2);
            if (matches_size > 1)
                drawtext(middle.x, middle.y + box_height * 0.3, potential_POI.at(1), width * 2, width * 2);
            if (matches_size > 2)
                drawtext(middle.x, middle.y + box_height * 0.2, potential_POI.at(2), width * 2, width * 2);
            if (matches_size > 3)
                drawtext(middle.x, middle.y + box_height * 0.1, potential_POI.at(3), width * 2, width * 2);
            if (matches_size > 4)
                drawtext(middle.x, middle.y, potential_POI.at(4), width * 2, width * 2);
        }

    } else if (key_pressed == '\b') {
        write_in_text_box(key_pressed);
        if (current_input.size() != 0)
            current_input.pop_back();

    } else {
        write_in_text_box(key_pressed);
        current_input += key_pressed;
        update_message("");
    }
}

//create all buttons we need in GUI

void create_all_buttons() {
    create_button("Window", "Find", find_street_intersections_callback);
    create_button("Exit", "Load", load_map_callback);
    create_button("Find", "POI", show_poi_callback);
    create_button("POI", "Features", show_features_callback);
    create_button("Features", "Clear", clear_callback);
    create_button("Clear", "Route", route_callback);
    create_button("Route", "To POI", POI_route_callback);
    create_button("To POI", "Help", show_help_callback);
}

//helper function of search button

static void find_street_intersections_callback(void (*drawscreen_ptr) (void)) {
    mode=0;
    current_input="";
    update_message("Please input two street names on textbox , use '&' to separate");
    set_keypress_input(true);
    show_text_box(true);
    draw_status.enterPressedEvent = FIND_STREET;
}

//things to do when mouse move

void act_on_mousemove(float x, float y) {
    t_bound_box current_view = get_visible_world();
    if (x >= current_view.top_right().x)
        show_menu(true);
    else
        show_menu(false);
}

//Call back function of load button

static void load_map_callback(void (*drawscreen_ptr) (void)) {
    update_message("Please input map name on text box");
    set_keypress_input(true);
    show_text_box(true);
    draw_status.enterPressedEvent = LOAD_MAP;
}

//a function used in key press call back function to determine what thing to do after
//enter key is pressed

static void enterPressedEvents() {
    switch (draw_status.enterPressedEvent) {
        case LOAD_MAP:
            if (load_map_func()) {
                update_message("Map is successfully loaded");
                current_input = "";
                set_keypress_input(false);
                show_text_box(false);
                //current_input.erase(0,1);
            } else
                update_message("Failed to load the map, pleas try other map name");
            break;
        case FIND_STREET:
            find_street_intersections_func();
            set_keypress_input(false);
            show_text_box(false);
            break;
        case FIND_ROUTE_POINT1:
            read_inter1_from_text_box();
            if (draw_status.enterPressedEvent == FIND_ROUTE_POINT2)
                update_message("Please input the second intersection name");
            show_text_box(ON);
            current_input = "";
            break;
        case FIND_ROUTE_POINT2:
            read_inter2_from_text_box();
            current_input = "";
            break;
        case FIND_POI_POINT1:
            read_inter1_from_text_box();
            if (draw_status.enterPressedEvent == FIND_POI_POINT2) {
                update_message("Please input a point of interest name(i.e Tim Hortons)");
                show_text_box(ON);
                mode = 1;
            } else {
                update_message("Invalid intersection name");
                show_text_box(ON);
            }
            current_input = "";
            break;
        case FIND_POI_POINT2:
            read_poi_from_text_box();
            current_input = "";
            break;
        default:
            update_message("");
    }
}

//load the map depending on the name 

static bool load_map_func() {
    bool success;
    string map_name = read_text_box();
    update_message("Loading map...");
    //convert name to small letters and remove all spaces
    boost::algorithm::to_lower(map_name);
    boost::algorithm::erase_all(map_name, " ");
    string dir = "/cad2/ece297s/public/maps/";
    //check which map to read
    if (map_name == "toronto")
        success = load_map(dir + "toronto.streets.bin");
    else if (map_name == "cairo")
        success = load_map(dir + "cairo_egypt.streets.bin");
    else if (map_name == "hamilton")
        success = load_map(dir + "hamilton_canada.streets.bin");
    else if (map_name == "moscow")
        success = load_map(dir + "moscow.streets.bin");
    else if (map_name == "newyork")
        success = load_map(dir + "newyork.streets.bin");
    else if (map_name == "sainthelena")
        success = load_map(dir + "saint_helena.streets.bin");
    else if (map_name == "torontocanada")
        success = load_map(dir + "toronto_canada.streets.bin");
    else if (map_name == "london")
        success = load_map(dir + "london_england.streets.bin");
    else
        return false;
    if (success) {
        draw_status.successLoaded = true;
        //reset bounds depending on new map bounds
        double * bounds = streetsHelper->get_bounds();
        const t_bound_box initial_coords = t_bound_box(bounds[0], bounds[1], bounds[2], bounds[3]);
        set_visible_world(initial_coords);
        enable_all_buttons();
        clearscreen();
        draw_screen();
        //enable mouse move call back
        set_mouse_move_input(ON);
    }
    return success;
}


//enable all buttons except load button

void enable_all_buttons() {
    for (int i = 0; i < BUTTON_NUM; i++) {
        enable_or_disable_button(i, ON);
    }
    enable_or_disable_button(BUTTON_NUM, OFF);
}

//search all data to find place with certain name
//return true if find any consistent place

static bool find_street_intersections_func() {
    string two_street_name = read_text_box();
    vector<string> street_names;
    //split up two street names by & 
    boost::split(street_names, two_street_name, boost::is_any_of("&"));
    //if number of street name is not 2
    //if only one street name is input, then search for street
    if (street_names.size() == 1) {
        if (highlight_street(street_names[0])) {
            boost::trim_left(street_names[0]);
            boost::trim_right(street_names[0]);
            update_message("All streets required are highlighted");
        } else {
            update_message("No street is found");
            draw_screen();
        }
        return true;
    }
    if (street_names.size() != 2) {
        update_message("Invalid number of inputs");
        return false;
    }
    //remove the spaces on the two sides of string
    boost::trim_left(street_names[0]);
    boost::trim_right(street_names[0]);
    boost::trim_right(street_names[1]);
    boost::trim_left(street_names[1]);

    auto all_intersections = find_intersection_ids_from_street_names(street_names[0], street_names[1]);
    //if no intersection if found;
    if (all_intersections.size() == 0) {
        update_message("No intersection is found");
        return false;
    }
    for (unsigned i = 0; i < all_intersections.size(); i++) {
        auto position = streetsHelper->get_intersection_position(all_intersections[i]);
        streetsHelper->set_intersection_hightlight(all_intersections[i], ON);
        //keep these highlight intersection to a vector , easier to clear afterwards
        draw_status.highlighted_intersections.push_back(all_intersections[i]);
        //also set a mark on this intersection then user can easily see it
        draw_status.marks.push_back(pair<double, double>(position.first, position.second));
        //print out information of intersection(s)
        cout << "Intersection ID: " << all_intersections[i] << "\n";
        cout << "Intersection Name: " << getIntersectionName(all_intersections[i]) << "\n";
        cout << "Coordinates: (" << position.first << "," << position.second << ")\n";
        cout << "====================================================================\n";
    }
    //zoom in to highlighted position , window is a box with center of this position and 
    //height and width are both 100
    auto position = streetsHelper->get_intersection_position(all_intersections[0]);
    set_visible_world(position.first - 100, position.second - 100, position.first + 100, position.second + 100);
    clearscreen();
    draw_screen();
    update_message("All required intersections are highlighted");
    return true;
}

static void show_poi_callback(void (*drawscreen_ptr) (void)) {
    //if already show POIs, disable it
    draw_status.showPOI = !draw_status.showPOI;
    draw_screen();
}

static void show_help_callback(void (*drawscreen_ptr) (void)) {
    //if already showing help, disable it 
    draw_status.helpOn = !draw_status.helpOn;
    draw_status.showPathResult = !draw_status.showPathResult;
    //if help is on, display info about how to use the interface 
    if (draw_status.helpOn) {
        show_route_plane_window(ON);
        std::vector<string> instructions;
        instructions.push_back("Enable the guidance system by");
        instructions.push_back("Pressing the 'Route' button");
        instructions.push_back("After you have clicked the");
        instructions.push_back("'Route' button, select the");
        instructions.push_back("starting point as well");
        instructions.push_back("As well as your destination");
        instructions.push_back("When two intersections have been");
        instructions.push_back("Chosen, the guidance system will");
        instructions.push_back("highlight the path in blue");
        instructions.push_back("Along with the directions displayed on");
        instructions.push_back("The text box on the left side of the window");
        for (unsigned instruction_count = 0; instruction_count < instructions.size(); instruction_count++) {
            write_in_route_plane_window(instructions[instruction_count]);
        }
    } else
        show_route_plane_window(OFF);
}

static void show_features_callback(void (*drawscreen_ptr) (void)) {
    //if already show features, disable it
    draw_status.showFeatures = !draw_status.showFeatures;
    draw_screen();
}

/* draw all marks recorded on draw status marks vector*/
static void draw_all_marks() {
    //here we just use the same symbol as clicked to mark
    for (unsigned i = 0; i < draw_status.marks.size(); i++)
        draw_clicked_symbol(draw_status.marks[i].first, draw_status.marks[i].second);
}

static void clear_callback(void (*drawscreen_ptr) (void)) {
    clear_map();
    draw_screen();
}

//clear all highlight places and marks

static void clear_map() {
    //first set all intersection to un-highlighted
    for (unsigned i = 0; i < draw_status.highlighted_intersections.size(); i++) {
        streetsHelper->set_intersection_hightlight(draw_status.highlighted_intersections[i], OFF);
    }
    draw_status.highlighted_intersections.clear();
    for (unsigned i = 0; i < draw_status.highlighted_streets.size(); i++) {
        streetsHelper->highLightStreets(draw_status.highlighted_streets[i], OFF);
    }
    draw_status.highlighted_streets.clear();
    draw_status.marks.clear();
    double * ini_bounds = streetsHelper->get_bounds();
    set_visible_world(ini_bounds[0], ini_bounds[1], ini_bounds[2], ini_bounds[3]);

}

static bool highlight_street(string streetName) {
    auto streetIDs = find_street_ids_from_name(streetName);
    //if no street is found, return false
    if (streetIDs.size() == 0)
        return false;
    for (unsigned i = 0; i < streetIDs.size(); i++) {
        streetsHelper->highLightStreets(streetIDs[i], ON);
        draw_status.highlighted_streets.push_back(streetIDs[i]);
    }
    return true;
}

static void route_callback(void (*drawscreen_ptr) (void)) {
    current_input="";
    //set auto-complete mode to intersection
    mode = 0;

    //First clear the buffer on path
    draw_status.pathLookingFor.clear();

    draw_status.showRoute = !draw_status.showRoute;
    draw_status.showPathResult = OFF;
    //Close route plane window first to avoid conflics
    show_route_plane_window(OFF);

    if (!draw_status.showRoute) {
        clear_route_plane_window();
        clear_route();
    } else {
        update_message("Please input first intersection name on text box");
        set_keypress_input(true);
        set_keypress_input(true);
        show_text_box(true);
        draw_status.enterPressedEvent = FIND_ROUTE_POINT1;
    }
}

//function that actually call the find path function

static void route_func() {
    draw_status.enterPressedEvent = EMPTY;
    vector<unsigned> path = find_path_between_intersections(draw_status.pathLookingFor[0], draw_status.pathLookingFor[1]);
    vector<string> directions = print_directions(path);
    highlight_route(path, ON);
    //Show the result on a separate window
    draw_status.showPathResult = ON;
    show_route_plane_window(ON);

    for (unsigned i = 0; i < path.size(); i++) {
        draw_status.highlighted_streets.push_back(path[i]);
    }

    for (unsigned i = 0; i < directions.size(); i++) {
        write_in_route_plane_window(directions[i]);
    }
    update_message("Path is successfully found");

    //close the text box
    show_text_box(OFF);

}

static void clear_route() {
    for (unsigned i = 0; i < draw_status.highlighted_streets.size(); i++) {
        streetsHelper->set_segment_highlight(draw_status.highlighted_streets[i], OFF);
    }
    draw_status.highlighted_streets.clear();
    draw_screen();
}

static void POI_route_callback(void (*drawscreen_ptr) (void)) {
    current_input="";
    //set auto-complete mode to intersection
    mode = 0;
    //Close route plane window first to avoid conflicts
    show_route_plane_window(OFF);
    draw_status.showPathResult = OFF;

    if (draw_status.showPOI) {
        draw_status.showPOIRoute = !draw_status.showPOIRoute;
        if (draw_status.showPOIRoute) {
            set_keypress_input(true);
            draw_status.enterPressedEvent = FIND_POI_POINT1;
            show_text_box(ON);
            update_message("Please input a intersection name");
        } else {
            clear_route_plane_window();
            show_route_plane_window(OFF);
            clear_route();
        }
    } else
        update_message("Please click on POI button first");
}

static void read_inter1_from_text_box() {
    string name = read_text_box();
    //trim the empty spaces from both side
    boost::trim_left(name);
    boost::trim_right(name);
    int intersection = streetsHelper->get_intersection_by_name(name);
    if (intersection == -1) { // name if invalid
        update_message("Invalid intersection name");
    } else {
        if (draw_status.enterPressedEvent == FIND_POI_POINT1) {
            draw_status.from_inter_to_poi = intersection;
            draw_status.enterPressedEvent = FIND_POI_POINT2;
        } else if (draw_status.enterPressedEvent == FIND_ROUTE_POINT1) {
            draw_status.pathLookingFor.push_back(intersection);
            draw_status.enterPressedEvent = FIND_ROUTE_POINT2;
        }
    }

    return;
}

static void read_poi_from_text_box() {
    string poi_name = read_text_box();
    //trim the empty spaces from both side
    boost::trim_left(poi_name);
    boost::trim_right(poi_name);

    vector<unsigned> path;
    vector<string> directions;
    auto& poisByName = streetsHelper->get_POINyName();
    auto iter = poisByName.find(poi_name);

    if (iter == poisByName.end()) {
        update_message("Invalid point of interest name");
        show_text_box(ON);
        return;
    } else {
        draw_status.enterPressedEvent = EMPTY;
        draw_status.showPathResult = ON;
        path = find_path_to_point_of_interest(draw_status.from_inter_to_poi, poi_name);
        if (path.size() == 0) {
            update_message("Intersection and Point of Interest are too close");
            return;
        }

        directions = print_directions(path);
        highlight_route(path, ON);

        //Show the result on a separate window
        show_route_plane_window(ON);

        for (unsigned i = 0; i < path.size(); i++) {
            draw_status.highlighted_streets.push_back(path[i]);
        }

        for (unsigned i = 0; i < directions.size(); i++) {
            write_in_route_plane_window(directions[i]);
        }

        update_message("Path is successfully found");

        //close the text box
        show_text_box(OFF);
    }
}

static void read_inter2_from_text_box() {
    string name = read_text_box();
    //trim the empty spaces from both side
    boost::trim_left(name);
    boost::trim_right(name);
    int intersection = streetsHelper->get_intersection_by_name(name);
    if (intersection == -1) { // name if invalid
        update_message("Invalid intersection name");
        //Clear the text box and restart to input
        show_text_box(ON);
    } else if (unsigned(intersection) == draw_status.pathLookingFor[0]) {
        update_message("Repeated intersection");
        show_text_box(ON);
    } else {
        draw_status.pathLookingFor.push_back(intersection);
        route_func();
    }
}
