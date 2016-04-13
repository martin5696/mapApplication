#include "m3.h"
#define INF 0x7FFFFFFF
//#define INF 1.0e12
#define TURN_TIME 0.25
//const double TURN_TIME = 0.25;
using namespace std;

extern StreetsGraph* streetsHelper;

struct InterNode {
    unsigned intersectionID;

    //Record the shortest times from different segment
    vector<pair<unsigned, double>> shortestTimes;

    //Shortest time among all shortest times
    double shortest_time;

    //Estimated_cost = shortest_time + heuristic
    double estimated_cost;
    
    bool isTarget;

    InterNode(double shortest_time_, double estimated_cost_) : shortest_time(shortest_time_),
    estimated_cost(estimated_cost_) {
        isTarget = false;
    }
};

struct waveElem {
    unsigned intersectionID;

    double estimated_cost;

    waveElem(unsigned intersectionID_, double estimated_cost_) : intersectionID(intersectionID_),
    estimated_cost(estimated_cost_) {
    }

};

bool operator<(const waveElem& lhs, const waveElem & rhs) {
    if (lhs.estimated_cost > rhs.estimated_cost)
        return true;
    else
        return false;
}
/*-----------------Helper function--------------*/
static std::vector<unsigned> find_path_between_intersections_helper(unsigned intersect_id_start, unsigned intersect_id_end, priority_queue<waveElem>& wavefront,
        vector<InterNode>& intersections);

//If two segments belong to same street ,return 0. Otherwise return 1
static int isSameStreet(unsigned segment1, unsigned segment2);

// Used to find the other end of a given segment id and intersection id 
// if it exists, return that id.Otherwise, return -1
static int find_the_other_end(unsigned segmentID, unsigned from_intersection);

static void get_path(unsigned intersect_id_start, unsigned intersect_id_end, vector<unsigned> & path, vector<InterNode>& intersections);

static double heuristic(unsigned end_id, unsigned intersectionID, vector<InterNode>& intersections);

static void singleDijkstra_helper(unsigned intersect_id_start, const vector<unsigned>& intersect_id_to, priority_queue<waveElem>& wavefront,
        vector<InterNode>& intersections,std::unordered_map<unsigned,std::unordered_map<unsigned,double>>& cost_map);

//Return poi(s) with given name that is the closest to given intersection.
//If the poi(s)
static priority_queue<waveElem> find_nearest_poi_given_name(unsigned intersectionID, string poi_name);

std::vector<unsigned> find_path_between_intersections(unsigned intersect_id_start, unsigned intersect_id_end) {
    vector<InterNode> intersections;
    intersections.assign(getNumberOfIntersections(), InterNode(INF, INF));
    vector<unsigned> path;
    if (intersect_id_start == intersect_id_end) {
        return path;
    }
    //Initialize the start intersection , set time and cost to zero
    intersections[intersect_id_start].intersectionID = intersect_id_start;
    intersections[intersect_id_start].shortest_time = 0.0;
    intersections[intersect_id_start].estimated_cost = 0.0;
    vector<unsigned> & start_segments = streetsHelper->get_intersection_street_segments_by_reference(intersect_id_start);

    for (unsigned i = 0; i < start_segments.size(); i++)
        intersections[intersect_id_start].shortestTimes.push_back(pair<unsigned, double>(start_segments[i], 0.0));

    priority_queue<waveElem> wavefront;
    wavefront.push(waveElem(intersections[intersect_id_start].intersectionID, intersections[intersect_id_start].estimated_cost));

    path = find_path_between_intersections_helper(intersect_id_start, intersect_id_end, wavefront, intersections);

    return path;

}

std::vector<string> print_directions(vector<unsigned> street_segment_IDs) {

    std::vector<std::string> directions;
    string next_street_name;
    string current_street_name;
    double travel_time = compute_path_travel_time(street_segment_IDs);

    directions.push_back("Search results: ");


    int hours = travel_time / 60;
    int minutes = travel_time;
    int seconds = (travel_time - minutes) * 60;

    string string_1 = "Will arrive at destination in ";
    string string_hours = " hours ";
    string string_minutes = " minutes ";
    string string_seconds = " seconds ";

    string hours_str;
    ostringstream convert_hours;
    convert_hours << hours;
    hours_str = convert_hours.str();

    string minutes_str;
    ostringstream convert_minutes;
    convert_minutes << minutes;
    minutes_str = convert_minutes.str();

    string seconds_str;
    ostringstream convert_seconds;
    convert_seconds << seconds;
    seconds_str = convert_seconds.str();

    string total_time = string_1;
    string total_time_2 = hours_str + string_hours + minutes_str + string_minutes + seconds_str + string_seconds;

    if (street_segment_IDs.size() == 1) {
        StreetSegmentInfo current_segment_info = streetsHelper -> get_street_segment(street_segment_IDs[0]);
        current_street_name = getStreetName(current_segment_info.streetID);

        if (current_street_name.size() > 15) {
            string straight = "Continue on ";
            directions.push_back(straight);
            directions.push_back(current_street_name);
        } else {
            std::string straight = "Continue on ";
            std::string result = straight + current_street_name;
            directions.push_back(result);
        }
    }

    for (unsigned segment_index = 0; segment_index < street_segment_IDs.size() - 1; segment_index++) {

        // cout << segment_index << endl;

        StreetSegmentInfo current_segment_info = streetsHelper -> get_street_segment(street_segment_IDs[segment_index]);
        StreetSegmentInfo next_segment_info = streetsHelper -> get_street_segment(street_segment_IDs[segment_index + 1]);
        //find the street name that the segment belongs to 
        current_street_name = getStreetName(current_segment_info.streetID);
        next_street_name = getStreetName(next_segment_info.streetID);


        if (current_street_name != next_street_name) {

            //if turning left 
            //the next segments intersection position should be greater in the y-direction
            //or lesser in the x-direction
            std::pair<double, double> current_segment_from = streetsHelper -> get_intersection_position(current_segment_info.from);
            std::pair<double, double> current_segment_to = streetsHelper -> get_intersection_position(current_segment_info.to);
            std::pair<double, double> next_segment_from = streetsHelper -> get_intersection_position(next_segment_info.from);
            std::pair<double, double> next_segment_to = streetsHelper -> get_intersection_position(next_segment_info.to);

            //determien the orientations of the from's and to's of the segments
            if (current_segment_to == next_segment_from) {
                //the orientation we want 
            } else if (current_segment_from == next_segment_from) {
                std::pair<double, double> temp_1 = current_segment_to;
                current_segment_to = current_segment_from;
                current_segment_from = temp_1;
            } else if (current_segment_from == next_segment_to) {
                std::pair<double, double> temp_2 = current_segment_to;
                current_segment_to = current_segment_from;
                current_segment_from = temp_2;

                std::pair<double, double> temp_3 = next_segment_to;
                next_segment_to = next_segment_from;
                next_segment_from = temp_3;
            } else if (current_segment_to == next_segment_to) {
                std::pair<double, double> temp_4 = next_segment_to;
                next_segment_to = next_segment_from;
                next_segment_from = temp_4;
            }
            //project the segment from-and-to points onto the first plane (the all positive plane)
            //from point of current street segment
            float x_current_from = current_segment_from.first;
            float y_current_from = current_segment_from.second;
            //to point of current street segment
            float x_current_to = current_segment_to.first;
            float y_current_to = current_segment_to.second;

            //to point of next street segment 
            float x_next_to = next_segment_to.first;
            float y_next_to = next_segment_to.second;

            //angle the current segment makes with the x-axis 
            float y_diff_current = (y_current_to) - (y_current_from);
            float x_diff_current = (x_current_to) - (x_current_from);
            // take absoulte of the difference 
            y_diff_current = (y_diff_current);
            x_diff_current = (x_diff_current);
            //atan converts angle from -180 to 180 degrees
            float angle_theta = atan2(y_diff_current, x_diff_current);
            angle_theta = angle_theta * 180 / 3.14159;

            //angle made by the next segment witeh the x-axis 
            float y_diff_next = (y_next_to) - (y_current_from);
            float x_diff_next = (x_next_to) - (x_current_from);
            // take absoulte of the difference 
            x_diff_next = (x_diff_next);
            y_diff_next = (y_diff_next);

            float angle_phi = atan2(y_diff_next, x_diff_next);
            angle_phi = angle_phi * 180 / 3.14159;

            if (angle_phi > angle_theta) {
                if (next_street_name.size() > 15) {
                    string left = "Turn left onto ";
                    directions.push_back(left);
                    directions.push_back(next_street_name);
                } else {
                    string left = "Turn left onto ";
                    string result = left + next_street_name;
                    directions.push_back(result);
                }
            }
            if (angle_phi < angle_theta) {
                if (next_street_name.size() > 15) {
                    string right = "Turn right onto ";
                    directions.push_back(right);
                    directions.push_back(next_street_name);
                } else {
                    string right = "Turn right onto ";
                    string result = right + next_street_name;
                    directions.push_back(result);
                }
            }
            if (angle_phi == angle_theta) {
                if (next_street_name.size() > 15) {
                    string continue_onto = "Continue onto ";
                    directions.push_back(continue_onto);
                    directions.push_back(next_street_name);
                } else {
                    string continue_onto = "Continue onto ";
                    string result = continue_onto + next_street_name;
                    directions.push_back(result);
                }
            }

        } else {
            std::string straight = "Continue on ";
            std::string result = straight + current_street_name;
            directions.push_back(result);

        }
    }
    //add the total time it takes to travel the path
    //at the end of the string vector 
    directions.push_back(total_time);
    directions.push_back(total_time_2);


    directions.erase(unique(directions.begin(), directions.end()), directions.end());
    for (unsigned i = 0; i < directions.size(); i++) {
        cout << directions[i] << endl;
    }
    return directions;
}

static std::vector<unsigned> find_path_between_intersections_helper(unsigned intersect_id_start, unsigned intersect_id_end, priority_queue<waveElem>& wavefront,
        vector<InterNode>& intersections) {

    double shortest_path_time = INF;

    while (!wavefront.empty()) {

        //get the first element intersection id and pop it 
        unsigned cur_intersection = wavefront.top().intersectionID;
        wavefront.pop();

        if (intersections[cur_intersection].estimated_cost < shortest_path_time) {
            if (cur_intersection == intersect_id_end) {
                // If current intersection is end intersection, update the shortest time
                if (intersections[cur_intersection].estimated_cost < shortest_path_time)
                    shortest_path_time = intersections[cur_intersection].estimated_cost;
            } else {
                // Get the adjacent segments of intersection
                vector<unsigned> & reachable_segments
                        = streetsHelper->get_intersection_street_segments_by_reference(cur_intersection);

                //Go to check every segment of current intersection
                for (unsigned i = 0; i < reachable_segments.size(); i++) {

                    int to_intersection
                            = find_the_other_end(reachable_segments[i], cur_intersection);

                    if (to_intersection != -1) {

                        intersections[to_intersection].intersectionID = to_intersection;

                        double segment_travel_time
                                = streetsHelper->get_segment_travel_time(reachable_segments[i]);
                        double newTime = INF;

                        // Get the shortest time to reach the next intersection
                        for (unsigned j = 0; j < intersections[cur_intersection].shortestTimes.size(); j++) {
                            double tempTime = intersections[cur_intersection].shortestTimes[j].second + segment_travel_time
                                    + TURN_TIME * isSameStreet(intersections[cur_intersection].shortestTimes[j].first, reachable_segments[i]);
                            if (tempTime < newTime)
                                newTime = tempTime;
                        }

                        bool segmentExist = false;
                        //Update the segment shortest time if available
                        for (unsigned k = 0; k < intersections[to_intersection].shortestTimes.size(); k++) {
                            if (intersections[to_intersection].shortestTimes[k].first == reachable_segments[i]) {
                                segmentExist = true;
                                if (newTime < intersections[to_intersection].shortestTimes[k].second) {
                                    intersections[to_intersection].shortestTimes[k].second = newTime;
                                    intersections[to_intersection].estimated_cost
                                            = newTime + heuristic(intersect_id_end, to_intersection, intersections);
                                    wavefront.push(waveElem(intersections[to_intersection].intersectionID,
                                            intersections[to_intersection].estimated_cost));
                                }
                                break;
                            }
                        }

                        if (newTime < intersections[to_intersection].shortest_time) {
                            intersections[to_intersection].shortest_time = newTime;
                            intersections[to_intersection].estimated_cost
                                    = newTime + heuristic(intersect_id_end, to_intersection, intersections);
                        }

                        //If segment does not exist, add this segment to next intersection
                        if (!segmentExist) {
                            if (newTime < intersections[to_intersection].shortest_time + TURN_TIME) {
                                intersections[to_intersection].shortestTimes.push_back(pair<unsigned, double>(reachable_segments[i], newTime));
                                wavefront.push(waveElem(intersections[to_intersection].intersectionID,
                                        intersections[to_intersection].estimated_cost));
                            }
                        }
                    }
                }
            }
        }
    }

    vector<unsigned> path;
    if (shortest_path_time < INF)
        get_path(intersect_id_start, intersect_id_end, path, intersections);

    return path;
}

static int isSameStreet(unsigned segment1, unsigned segment2) {
    unsigned streetID1 = streetsHelper->get_street_segment_information(segment1).streetID;
    unsigned streetID2 = streetsHelper->get_street_segment_information(segment2).streetID;

    if (streetID1 == streetID2)
        return 0;
    else
        return 1;
}

/*
 * Used to find the other end of a given segment id and intersection id 
 * if it exists, return that id.Otherwise, return -1
 */
static int find_the_other_end(unsigned segmentID, unsigned from_intersection) {
    auto streetSegment = streetsHelper->get_street_segment_information(segmentID);
    int other_end;
    if (from_intersection == streetSegment.from)
        other_end = streetSegment.to;
    else if (from_intersection == streetSegment.to) {
        if (streetSegment.oneWay)
            other_end = -1;
        else
            other_end = streetSegment.from;
    } else
        other_end = -1;


    return other_end;
}

static void get_path(unsigned intersect_id_start, unsigned intersect_id_end, vector<unsigned> & path, vector<InterNode>& intersections) {

    unsigned cur_intersection = intersect_id_end;
    double time = intersections[intersect_id_end].shortest_time;
    while (cur_intersection != intersect_id_start) {
        bool turnFound = false;
        for (unsigned i = 0; i < intersections[cur_intersection].shortestTimes.size(); i++) {
            double tempTime = time - TURN_TIME;
            //                cout <<i <<":" << intersections[cur_intersection].shortestTimes[i].second <<endl;
            //                cout << intersections[cur_intersection].shortestTimes[i].second - time <<endl;
            if (abs(intersections[cur_intersection].shortestTimes[i].second - tempTime) <= 1.0e-4) {
                turnFound = true;
                unsigned segmentID = intersections[cur_intersection].shortestTimes[i].first;
                StreetSegmentInfo segmentInfo = streetsHelper->get_street_segment_information(segmentID);
                if (cur_intersection == segmentInfo.from)
                    cur_intersection = segmentInfo.to;
                else
                    cur_intersection = segmentInfo.from;

                time -= TURN_TIME;
                time -= streetsHelper->get_segment_travel_time(segmentID);

                path.push_back(segmentID);

                //                    cout << segmentID <<endl;

                break;
            }
        }

        if (!turnFound) {
            for (unsigned i = 0; i < intersections[cur_intersection].shortestTimes.size(); i++) {
                //                cout <<i <<":" << intersections[cur_intersection].shortestTimes[i].second <<endl;
                //                cout << intersections[cur_intersection].shortestTimes[i].second - time <<endl;
                if (abs(intersections[cur_intersection].shortestTimes[i].second - time) <= 1.0e-4) {
                    unsigned segmentID = intersections[cur_intersection].shortestTimes[i].first;
                    StreetSegmentInfo segmentInfo = streetsHelper->get_street_segment_information(segmentID);
                    if (cur_intersection == segmentInfo.from)
                        cur_intersection = segmentInfo.to;
                    else
                        cur_intersection = segmentInfo.from;

                    time -= streetsHelper->get_segment_travel_time(segmentID);

                    path.push_back(segmentID);

                    //                    cout << segmentID <<endl;

                    break;
                }
            }
        }

    }
    reverse(path.begin(), path.end());
}

double compute_path_travel_time(const std::vector<unsigned>& path) {
    double time = 0.0;
    if (path.size() != 0) {
        time = streetsHelper->get_segment_travel_time(path[0]);
        for (unsigned i = 1; i < path.size(); i++) {
            time += find_street_segment_travel_time(path[i]) +
                    TURN_TIME * isSameStreet(path[i], path[i - 1]);
            //            cout <<time <<endl;
        }
    }

    return time;
}

static double heuristic(unsigned end_id, unsigned intersectionID, vector<InterNode>& intersections) {
    double estimated_distance;
    //get this intersection position and end point position
    auto cur_pos = streetsHelper->get_intersection_position(intersectionID);
    auto end_pos = streetsHelper->get_intersection_position(end_id);

    estimated_distance = abs(cur_pos.first - end_pos.first) + abs(cur_pos.second - end_pos.second);

    double h = 0.000555 * estimated_distance;
    return h;
}

std::vector<unsigned> find_path_to_point_of_interest(unsigned intersect_id_start, string point_of_interest_name) {
    vector<unsigned> path;
    auto nearest_pois = find_nearest_poi_given_name(intersect_id_start, point_of_interest_name);

    if (nearest_pois.empty())
        return path;

    unsigned cur_inter = nearest_pois.top().intersectionID;
    //    double minDistance = abs(target_pos.first - cur_inter_pos.first) + abs(target_pos.second - cur_inter_pos.second);
    path = find_path_between_intersections(intersect_id_start, cur_inter);
    double minDistance = nearest_pois.top().estimated_cost;
    nearest_pois.pop();
    double shortest_time = compute_path_travel_time(path);

    while (!nearest_pois.empty()) {
        cur_inter = nearest_pois.top().intersectionID;
        double tempDistance = nearest_pois.top().estimated_cost;
        nearest_pois.pop();
        //This condition is to ensure not going to far away to search
        //This limit is gotten by testing ... 
        if ((tempDistance / minDistance > 1.63 && tempDistance - minDistance > 1500) || tempDistance - minDistance > 12800)
            break;

        vector<unsigned> tempPath = find_path_between_intersections(intersect_id_start, cur_inter);
        double temp_time = compute_path_travel_time(tempPath);
        if (!path.empty()) {
            if (temp_time < shortest_time) {
                shortest_time = temp_time;
                path = tempPath;
            }
            //This condition is to ensure not going to far away to search
            //Smaller the ratio , faster the speed
            if (temp_time / shortest_time > 1.2425 && tempDistance - minDistance > 1500)
                break;
        }
    }


    return path;
}

static priority_queue<waveElem> find_nearest_poi_given_name(unsigned intersectionID, string poi_name) {
    //    vector<unsigned> nearest_pois;
    //    vector<pair<double,unsigned>> pois_with_same_name;
    priority_queue <waveElem> pois_with_same_name;
    auto& POIMap = streetsHelper->get_POINyName();
    auto range = POIMap.equal_range(poi_name);
    auto inter_pos = streetsHelper->get_intersection_position(intersectionID);

    //No point of interest is found
    if (range.first == POIMap.end()) {
        return pois_with_same_name;
    }

    for (auto iter = range.first; iter != range.second; iter++) {
        auto poi_pos = streetsHelper->get_poi_position_in_xy(iter->second);
        //get distance to target intersection
        //        double distance = abs(poi_pos.first - inter_pos.first) + abs(poi_pos.second - inter_pos.second);
        double distance = abs(poi_pos.first - inter_pos.first) + abs(poi_pos.second - inter_pos.second);
        pois_with_same_name.push(waveElem(iter->second, distance));
    }

    auto poi_pos = streetsHelper->get_poi_position_in_xy(pois_with_same_name.top().intersectionID);
    auto cur_inter = streetsHelper->find_nearest_intersection(poi_pos.first, poi_pos.second);
    auto cur_inter_pos = streetsHelper->get_intersection_position(cur_inter);
    double shortest_distance = abs(cur_inter_pos.first - inter_pos.first) + abs(cur_inter_pos.second - inter_pos.second);
    double temp_shortest_distance = shortest_distance;

    pois_with_same_name.pop();
    priority_queue<waveElem> nearest_pois;
    nearest_pois.push(waveElem(cur_inter, temp_shortest_distance));

    while ((temp_shortest_distance / shortest_distance < 1.8 || temp_shortest_distance - shortest_distance < 1500)
            && !pois_with_same_name.empty()) {
        poi_pos = streetsHelper->get_poi_position_in_xy(pois_with_same_name.top().intersectionID);
        cur_inter = streetsHelper->find_nearest_intersection(poi_pos.first, poi_pos.second);
        cur_inter_pos = streetsHelper->get_intersection_position(cur_inter);
        temp_shortest_distance = abs(cur_inter_pos.first - inter_pos.first) + abs(cur_inter_pos.second - inter_pos.second);

        pois_with_same_name.pop();
        nearest_pois.push(waveElem(cur_inter, temp_shortest_distance));
    }


    return nearest_pois;
}







void singleSourceDijkstra(unsigned intersect_id_start, const std::vector<unsigned>& intersect_id_to,
        std::unordered_map<unsigned,std::unordered_map<unsigned,double>>& cost_map){
    vector<InterNode> intersections;
    intersections.assign(getNumberOfIntersections(), InterNode(INF, INF));


    //Initialize the start intersection , set time and cost to zero
    intersections[intersect_id_start].intersectionID = intersect_id_start;
    intersections[intersect_id_start].shortest_time = 0.0;
    intersections[intersect_id_start].estimated_cost = 0.0;
    vector<unsigned> & start_segments = streetsHelper->get_intersection_street_segments_by_reference(intersect_id_start);

    for (unsigned i = 0; i < start_segments.size(); i++)
        intersections[intersect_id_start].shortestTimes.push_back(pair<unsigned, double>(start_segments[i], 0.0));

    priority_queue<waveElem> wavefront;
    wavefront.push(waveElem(intersections[intersect_id_start].intersectionID, intersections[intersect_id_start].estimated_cost));

    //Flag the targets
    for(unsigned i = 0 ; i < intersect_id_to.size(); i++){
        intersections[intersect_id_to[i]].isTarget = true;
        //cout << intersect_id_to[i]<< "is true"<<endl;
    }
    
    singleDijkstra_helper(intersect_id_start, intersect_id_to, wavefront,intersections,cost_map);
    
}


static void singleDijkstra_helper(unsigned intersect_id_start, const vector<unsigned>& intersect_id_to, priority_queue<waveElem>& wavefront,
        vector<InterNode>& intersections,std::unordered_map<unsigned,std::unordered_map<unsigned,double>>& cost_map){
    
    double shortest_path_time = INF;

    while (!wavefront.empty()) {

        //get the first element intersection id and pop it 
        unsigned cur_intersection = wavefront.top().intersectionID;
        wavefront.pop();

//        double temp_shortest_path_time = 0 ;
//        //Chose the largest cost among the target intersection as the upper bound
//        for(unsigned target = 0; target < intersect_id_to.size() ; target++){
//            double tempCost = cost_map[intersect_id_start][intersect_id_to[target]];
//            if(tempCost > temp_shortest_path_time)
//                temp_shortest_path_time = tempCost;
//        }
//        shortest_path_time = temp_shortest_path_time;
        
//        if (intersections[cur_intersection].estimated_cost < shortest_path_time) {
            if (intersections[cur_intersection].isTarget) {
                // If current intersection is end intersection, update the shortest time
                if (intersections[cur_intersection].estimated_cost < cost_map[intersect_id_start][cur_intersection]){
//                    cout <<"update : "<<intersect_id_start<<" ->" << cur_intersection<< " to " 
//                            <<intersections[cur_intersection].estimated_cost  <<endl;
                    cost_map[intersect_id_start][cur_intersection] = intersections[cur_intersection].estimated_cost; 
                }
            } 
            
            // Get the adjacent segments of intersection
            vector<unsigned> & reachable_segments
                    = streetsHelper->get_intersection_street_segments_by_reference(cur_intersection);

            //Go to check every segment of current intersection
            for (unsigned i = 0; i < reachable_segments.size(); i++) {

                int to_intersection
                        = find_the_other_end(reachable_segments[i], cur_intersection);

                if (to_intersection != -1) {

                    intersections[to_intersection].intersectionID = to_intersection;

                    double segment_travel_time
                            = streetsHelper->get_segment_travel_time(reachable_segments[i]);
                    double newTime = INF;

                    // Get the shortest time to reach the next intersection
                    for (unsigned j = 0; j < intersections[cur_intersection].shortestTimes.size(); j++) {
                        double tempTime = intersections[cur_intersection].shortestTimes[j].second + segment_travel_time
                                + TURN_TIME * isSameStreet(intersections[cur_intersection].shortestTimes[j].first, reachable_segments[i]);
                        if (tempTime < newTime)
                            newTime = tempTime;
                    }

                    bool segmentExist = false;
                    //Update the segment shortest time if available
                    for (unsigned k = 0; k < intersections[to_intersection].shortestTimes.size(); k++) {
                        if (intersections[to_intersection].shortestTimes[k].first == reachable_segments[i]) {
                            segmentExist = true;
                            if (newTime < intersections[to_intersection].shortestTimes[k].second) {
                                intersections[to_intersection].shortestTimes[k].second = newTime;
                                intersections[to_intersection].estimated_cost = newTime;
                                wavefront.push(waveElem(intersections[to_intersection].intersectionID,
                                        intersections[to_intersection].estimated_cost));
                            }
                            break;
                        }
                    }

                    if (newTime < intersections[to_intersection].shortest_time) {
                        intersections[to_intersection].shortest_time = newTime;
                        intersections[to_intersection].estimated_cost = newTime;
                    }

                    //If segment does not exist, add this segment to next intersection
                    if (!segmentExist) {
                        if (newTime < intersections[to_intersection].shortest_time + TURN_TIME) {
                            intersections[to_intersection].shortestTimes.push_back(pair<unsigned, double>(reachable_segments[i], newTime));
                            wavefront.push(waveElem(intersections[to_intersection].intersectionID,
                                    intersections[to_intersection].estimated_cost));
                        }
                    }
                }
            }

        }
//    }
}