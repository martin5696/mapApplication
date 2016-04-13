#include "StreetsDatabaseAPI.h"
#include "m1.h"
#include <vector> //include STL vector
#include <math.h>
//Global variable used to speed up
StreetsGraph* streetsHelper;

//load the map

bool load_map(std::string map_name) {
    std::string dir = "/cad2/ece297s/public/maps/";
    bool load_success1 = loadStreetsDatabaseBIN(map_name);
    string osmPath;  
    
    // Fetch out the city name
    map_name.erase(0,dir.size());
    map_name.erase(map_name.size() - 12, map_name.size()-1);
    
    //transfer to osm path
    osmPath = dir + map_name + ".osm.bin";
    
    bool load_success2 = loadOSMDatabaseBIN(osmPath); 
    if (load_success1 && load_success2)
        streetsHelper = StreetsGraph::GetStreetsGraphInstance();
    return load_success1 && load_success2;
}

//close the map

void close_map() {
    closeStreetDatabase();
    closeOSMDatabase();
}

//function to return the street segments for a given intersection 

std::vector <unsigned> find_intersection_street_segments(unsigned intersection_id) {
    return streetsHelper->intersections().at(intersection_id).segment_ids;
}

//function to return all intersection ids for two intersecting streets
//this function will typically return one intersection id between two street names
//but duplicate street names are allowed, so more than 1 intersection id may exist for 2 street names

std::vector<unsigned> find_intersection_ids_from_street_names(std::string street_name1, std::string street_name2) {
    //put all of street_name1's intersection IDs in one vector
    std::vector<unsigned> streetIDs_from_name1 = streetsHelper -> get_street_ids(street_name1);
    std::vector<unsigned> streetIDs_from_name2 = streetsHelper -> get_street_ids(street_name2);
    std::vector<unsigned> intersectingIDs;
    //go through every element in each of streets,and check whether the other set has same element
    for (unsigned i = 0; i < streetIDs_from_name1.size(); i++) {
        auto & intersectionSet1 = streetsHelper->get_street_intersections(streetIDs_from_name1[i]);
        for (unsigned j = 0; j < streetIDs_from_name2.size(); j++) {
            auto & intersectionSet2 = streetsHelper->get_street_intersections(streetIDs_from_name2[j]);
            for (auto iter1 = intersectionSet1.begin(); iter1 != intersectionSet1.end(); iter1++) {
                if (intersectionSet2.find(*iter1) != intersectionSet2.end()) {
                    intersectingIDs.push_back(*iter1);
                }
            }
        }
    }

    return intersectingIDs;
}


//find distance between two coordinates

double find_distance_between_two_points(LatLon point1, LatLon point2) {
    double lat_average = ((DEG_TO_RAD * point2.lat + DEG_TO_RAD * point1.lat) / 2);
    double x1 = (DEG_TO_RAD * point1.lon) * cos(lat_average);
    double y1 = DEG_TO_RAD * point1.lat;
    double x2 = (DEG_TO_RAD * point2.lon) * cos(lat_average);
    double y2 = DEG_TO_RAD * point2.lat;

    double distance_between_two_points = EARTH_RADIUS_IN_METERS * (sqrt(pow((y2 - y1), 2) + pow((x2 - x1), 2)));
    return distance_between_two_points;
}


//find the length of a given street segment

double find_street_segment_length(unsigned street_segment_id) {
    double street_segment_length;
    double first_segment_length;
    double last_segment_length;
    double curve_length = 0;
    //street segment is straight
    if (getStreetSegmentInfo(street_segment_id).curvePointCount == 0) {
        LatLon initial_position = getIntersectionPosition(getStreetSegmentInfo(street_segment_id).from);
        LatLon final_position = getIntersectionPosition(getStreetSegmentInfo(street_segment_id).to);
        street_segment_length = find_distance_between_two_points(initial_position, final_position);
        return street_segment_length;
    }// street segment has curves
    else {
        //distance from first intersection to the first curve point
        LatLon first_intersection = getIntersectionPosition(getStreetSegmentInfo(street_segment_id).from);
        LatLon first_curve_point = getStreetSegmentCurvePoint(street_segment_id, 0);
        first_segment_length = find_distance_between_two_points(first_intersection, first_curve_point);

        //distance from last curve point to the end intersection 
        LatLon last_curve_point = getStreetSegmentCurvePoint(street_segment_id, (getStreetSegmentInfo(street_segment_id).curvePointCount) - 1);
        LatLon end_intersection = getIntersectionPosition(getStreetSegmentInfo(street_segment_id).to);
        last_segment_length = find_distance_between_two_points(last_curve_point, end_intersection);

        //start from the first curve point and end at the second last curve point
        for (unsigned curve_point_count = 0; curve_point_count < ((getStreetSegmentInfo(street_segment_id).curvePointCount) - 1); curve_point_count++) {
            //fetch the latlon of the idx'th curve point
            LatLon first_curve_point = getStreetSegmentCurvePoint(street_segment_id, curve_point_count);
            LatLon second_curve_point = getStreetSegmentCurvePoint(street_segment_id, curve_point_count + 1);
            curve_length += find_distance_between_two_points(first_curve_point, second_curve_point);
        }
        street_segment_length = curve_length + first_segment_length + last_segment_length;


        return street_segment_length;
    }
}


//find the length of a whole street

double find_street_length(unsigned street_id) {
    double streetLength = 0;
    std::vector<unsigned> allStreetSegments;
    allStreetSegments = streetsHelper -> get_street_segments(street_id);
    unsigned numOfSegments = allStreetSegments.size();
    for (unsigned i = 0; i < numOfSegments; i++) {
        streetLength += find_street_segment_length(allStreetSegments[i]);
    }
    return streetLength;
}


//find the travel time to drive a street segment (time(minutes) = distance(km)/speed_limit(km/hr) * 60

double find_street_segment_travel_time(unsigned street_segment_id) {
//    double travel_time;
//    double travel_distance; //in km
//    travel_distance = (find_street_segment_length(street_segment_id) / 1000.0);
//    travel_time = travel_distance / (getStreetSegmentInfo(street_segment_id).speedLimit) * 60.0;
//    return travel_time;
    
    return streetsHelper->get_segment_travel_time(street_segment_id);
}


//for a given street, return all the street segments

std::vector<unsigned> find_street_street_segments(unsigned street_id) {
    return streetsHelper->get_street_segments(street_id);
}


//function to return street id(s) for a street name
//return a 0-length vector if no street with this name exists.

std::vector<unsigned> find_street_ids_from_name(std::string street_name) {
    return streetsHelper->get_street_ids(street_name);
}


//for a given street, find all the intersections

std::vector<unsigned> find_all_street_intersections(unsigned street_id) {
    auto & allIntersections = streetsHelper->get_street_intersections(street_id);
    std::vector<unsigned> allIntersectionsVec;
    //transfer from unordered set to vector
    for (auto setIter = allIntersections.begin(); setIter != allIntersections.end(); setIter++) {
        allIntersectionsVec.push_back(*setIter);
    }
    return allIntersectionsVec;
}


//function to return street names at an intersection (include duplicate street names in returned vector)

std::vector<std::string> find_intersection_street_names(unsigned intersection_id) {
    std::vector<std::string> street_names_vec;
    //get number of street segments at the intersection
    int numStreetSegments = streetsHelper->get_intersection_street_segments(intersection_id).size();
    //iterate through each segment to obtain the segment ID, segmentInfo struct, streetID, and finally street name from the street ID
    for (int i = 0; i < numStreetSegments; i++) {
        StreetSegmentInfo segmentInfo = streetsHelper->get_street_segment_information((streetsHelper->get_intersection_street_segments(intersection_id).at(i)));
        //StreetSegmentInfo segmentInfo= streetsHelper->get_street_segment_information(i);//change to O(1)segmentInfo=segmentVector[seg id]
        street_names_vec.push_back(getStreetName(segmentInfo.streetID));
    }
    return street_names_vec;
}



//can you get from intersection1 to intersection2 using a single street segment (hint: check for 1-way streets too)
//corner case: an intersection is considered to be connected to itself

bool are_directly_connected(unsigned intersection_id1, unsigned intersection_id2) {
    //check the corner case
    if (intersection_id1 == intersection_id2)
        return true;

    int common_segment_id = -1;
    bool common_seg_found = false;

    std::vector<unsigned> street_segments_ids_intersection1
            = streetsHelper->get_intersection_street_segments(intersection_id1);
    std::vector<unsigned> street_segments_ids_intersection2
            = streetsHelper->get_intersection_street_segments(intersection_id2);

    int numStreetSegments_id1 = street_segments_ids_intersection1.size();
    int numStreetSegments_id2 = street_segments_ids_intersection2.size();

    //check if there's a common street segment
    for (int i = 0; i < numStreetSegments_id1; i++)
        for (int j = 0; j < numStreetSegments_id2; j++)
            if (street_segments_ids_intersection1[i] == street_segments_ids_intersection2[j]) {
                common_segment_id = street_segments_ids_intersection1[i];
                common_seg_found = true;
            }
    //if there's no common street segment, not connected
    if (!common_seg_found)
        return false;

    //there's a common street segment:

    //if it's not one way, connected
    if (!streetsHelper->get_street_segment_information(common_segment_id).oneWay)
        return true;

        //if it is one way, check if intersection1=from. if yes, then connected
    else {
        if (intersection_id1 == streetsHelper->get_street_segment_information(common_segment_id).from)
            return true;
        else
            return false;
    }
}


//find all intersections reachable by traveling down one street segment 
//from given intersection (hint: you can't travel the wrong way on a 1-way street)
//the returned vector should NOT contain duplicate intersections

std::vector<unsigned> find_adjacent_intersections(unsigned intersection_id) {
    std::vector<unsigned> reachable_intersection_ids;
    bool repeated;
    bool push;
    unsigned id_to_push;
    //int numStreetSegments = streetsHelper->get_intersection_street_segments(intersection_id).size();
    //iterate through all segments connected to this intersection
    for (unsigned i = 0; i < getIntersectionStreetSegmentCount(intersection_id); i++) {
        repeated = false;
        push = false;
        //int street_seg_id = getIntersectionStreetSegment(intersection_id, i);
        // StreetSegmentInfo test_segment = getStreetSegmentInfo(street_seg_id);
        StreetSegmentInfo test_segment = streetsHelper->get_street_segment_information((streetsHelper->get_intersection_street_segments(intersection_id).at(i)));

        //if not one way, it can be reached
        //compare from && to with intersection_id, the one that's different is the one i need to put in. the other one is the same as intersection_id

        if (!test_segment.oneWay) {

            if (intersection_id != test_segment.to) {
                push = true;
                id_to_push = test_segment.to;

            } else {
                push = true;
                id_to_push = test_segment.from;
            }
        }//if it's one way, then it can be reached if from = intersection_id
        else {

            if (test_segment.from == intersection_id) {
                push = true;
                id_to_push = test_segment.to;

            }
        }
        //check if it already exists
        for (unsigned i = 0; i < reachable_intersection_ids.size(); i++) {
            if (reachable_intersection_ids[i] == id_to_push)
                repeated = true;
        }
        //if it doesn't exist and the value of id_to_push was actually reassigned, then push to the vector
        if (!repeated && push)
            reachable_intersection_ids.push_back(id_to_push);
    }

    return reachable_intersection_ids;
}


//find the nearest intersection (by ID) to a given position

unsigned find_closest_intersection(LatLon my_position) {
    return streetsHelper->find_nearest_intersection(my_position);
}


//find the nearest point of interest to a given position

unsigned find_closest_point_of_interest(LatLon my_position) {
    return streetsHelper->find_nearest_poi(my_position);
}