/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   StreetGraph.cpp
 * Author: zhengz28
 * 
 * Created on January 18, 2016, 7:51 PM
 */

#include <boost/smart_ptr/shared_ptr.hpp>
#include "StreetsGraph.h"
#include "FeatureSort.h"
#include <thread>
#include <time.h>
//Must define the static member 
boost::shared_ptr<StreetsGraph> StreetsGraph::streetsGraphInstance = NULL;

void deleteHelper(StreetsGraph* streetsGraphPointer) {
    delete streetsGraphPointer;
}

StreetsGraph::StreetsGraph() : streetsVectorByID(getNumberOfStreets()), intersectionsByID(getNumberOfIntersections()),
streetSegmentsByID(getNumberOfStreetSegments()), featuresByID(getNumberOfFeatures()), POIByID(getNumberOfPointsOfInterest()) {
//    auto startTime = chrono::high_resolution_clock::now();

        //find the max and min latitude of this map
        bounds_helper();
    
        //add all intersections
        thread myThread1;
        myThread1 = thread(&StreetsGraph::addIntersections, this);
    
        //add all POI's
        thread myThread2;
        myThread2 = thread(&StreetsGraph::addPOI, this);
    
        //add all features
        thread myThread3;
        myThread3 = thread(&StreetsGraph::addFeatures, this);
    
    
        myThread1.join();
    
    
        //add streets and segments information
        addStreetsAndSegments();
    
        //add intersections information
        //addIntersections();
    
        myThread2.join();
        myThread3.join();


//---------------------Load without multithreading----------------------
//    //find the max and min latitude of this map
//    bounds_helper();
//    //add all intersections
//    addIntersections();
//    //add all POIs 
//    addPOI();
//    //add all features
//    addFeatures();
//    //add streets and segments information
//    addStreetsAndSegments();
        
//    auto currentTime = chrono::high_resolution_clock::now();
//    auto wallClock =chrono::duration_cast<chrono::duration<double>> (currentTime - startTime);
//    cout << "Load constructor: " << wallClock.count() << endl;
//}

}

//find maximum and minimum latitude used to convert coordinates

void StreetsGraph::bounds_helper() {
    auto bounds_in_latlon = get_osm_bounds();
    latitude.first = bounds_in_latlon.first.lat;
    latitude.second = bounds_in_latlon.second.lat;
    auto bottomLeft = convert_LatLon_to_xy(bounds_in_latlon.first);
    auto topRight = convert_LatLon_to_xy(bounds_in_latlon.second);
    bounds[0] = bottomLeft.first;
    bounds[1] = bottomLeft.second;
    bounds[2] = topRight.first;
    bounds[3] = topRight.second;

}


//Helper function to add all intersection to data structure

void StreetsGraph::addIntersections() {
    //go through every intersections and get information
    for (unsigned i = 0; i < getNumberOfIntersections(); i++) {
        // set all intersections not highlighted
        intersectionsByID.at(i).isHighlighted = false;
        auto position = get_intersection_xy_helper(i);
        intersectionsByID.at(i).position_in_xy = position;
        //add intersection coordinates to kdtree
        intersection_kdtree.insert(coordinates(position.first, position.second, i));
        //check if this intersection is on the boundary
        string name = getIntersectionName(i);
        intersectionsByName.insert(pair<string, unsigned>(name, i));
        intersectionNames.push_back(name);
    }
}



//Helper function to build data structure related to streets and segments

void StreetsGraph::addStreetsAndSegments() {
    for (int segmentID = 0; segmentID < int(getNumberOfStreetSegments()); segmentID++) {
        const auto segmentInfo = getStreetSegmentInfo(segmentID);
        //Store the segment information in certain place
        streetSegmentsByID.at(segmentID).basicInfo = segmentInfo;
        streetSegmentsByID.at(segmentID).isHightlighted = false;
        streetSegmentsByID.at(segmentID).travel_time = calulate_segment_length(segmentID) / 1000.0 / segmentInfo.speedLimit * 60.0;

        //add tags to corresponding segment
        addWayTags(segmentInfo.wayOSMID, segmentID);
        //Convert curve points to xy coordinates
        convert_curve_points(segmentID);
        unsigned thisStreetID = segmentInfo.streetID;
        //Add street segments id to the corresponding street id
        streetsVectorByID[thisStreetID].segments.push_back(segmentID);
        //get the start and end intersection id of one street segment
        unsigned from = segmentInfo.from;
        unsigned to = segmentInfo.to;
        //if the start intersection information has not been recorded yet
        if (intersectionsByID.at(from).segment_ids.empty()) {
            int fromSegmentsCount = int(getIntersectionStreetSegmentCount(from));
            for (int i = 0; i < fromSegmentsCount; i++) {
                intersectionsByID.at(from).segment_ids.push_back(getIntersectionStreetSegment(from, i));
            }
        }
        //if from intersection is not added in this streets
        if (streetsVectorByID[thisStreetID].intersections.find(from) ==
                streetsVectorByID[thisStreetID].intersections.end()) {
            streetsVectorByID[thisStreetID].intersections.insert(from);
        }

        //if the end intersection information has not been recorded yet
        if (intersectionsByID[to].segment_ids.empty()) {
            int toSegmentsCount = int(getIntersectionStreetSegmentCount(to));
            for (int i = 0; i < toSegmentsCount; i++) {
                intersectionsByID[to].segment_ids.push_back(getIntersectionStreetSegment(to, i));
            }
        }

        //if to intersection is not added in this streets
        if (streetsVectorByID[thisStreetID].intersections.find(to) ==
                streetsVectorByID[thisStreetID].intersections.end()) {
            streetsVectorByID[thisStreetID].intersections.insert(to);
        }

    }

    //This is used to store the street Id to map with street name
    const auto &streets = streetsDB.streets();
    for (int i = 0; i < int(getNumberOfStreets()); i++) {
        streetsMapByName.insert(std::make_pair(streets.at(i), i));
    }
}

//Helper function to build data structure related to point of interest

void StreetsGraph::addPOI() {
    for (unsigned i = 0; i < getNumberOfPointsOfInterest(); i++) {
        auto position = get_poi_xy_helper(i);
        poi_kdtree.insert(coordinates(position.first, position.second, i));
        POIByID.at(i).position = position;
        string name = getPointOfInterestName(i);
        POIByName.insert(std::make_pair(name, i));
        POINames.push_back(name);
    }

}

//Helper function to add OSMW Tags

void StreetsGraph::addWayTags(OSMID osmid, unsigned street_segment_id) {
    auto way = getWayByOSMID(osmid);
    for (unsigned tagsIdx = 0; tagsIdx < way.tags().size(); tagsIdx++) {
        streetSegmentsByID.at(street_segment_id).tags.push_back(getTagPair(&way, tagsIdx));
    }
}

//helper function to convert all points of a feature to xy coordinates

void StreetsGraph::addFeatures() {
    std::vector<std::pair<double, unsigned>> lakeAndGreenSpaceAndIslandAndShoreline_SizeAndID;

    for (unsigned i = 0; i < getNumberOfFeatures(); i++) {
        for (unsigned j = 0; j < getFeaturePointCount(i); j++) {
            LatLon pos = getFeaturePoint(i, j);
            featuresByID.at(i).points_in_xy.push_back((convert_LatLon_to_xy(pos)));
        }
        FeatureType type = getFeatureType(i);

        //combine the green space and lakes and islands and shorelines to avoid overlapping features 
        if (type == Lake || type == Park || type == Greenspace || type == Golfcourse || type == Island || type == Shoreline) {
            double area = find_feature_area(i);
            lakeAndGreenSpaceAndIslandAndShoreline_SizeAndID.push_back(std::pair<double, unsigned>(area, i));
        } else if (type == Building)
            featureSort.buildings.push_back(i);
        else if (type == Beach)
            featureSort.beach.push_back(i);
        else if (type == River || type == Stream)
            featureSort.freshWater.push_back(i);
        else
            featureSort.unknowns.push_back(i);
    }

    //sort based on ascending feature area 
    std::sort(lakeAndGreenSpaceAndIslandAndShoreline_SizeAndID.begin(), lakeAndGreenSpaceAndIslandAndShoreline_SizeAndID.end());

    for (unsigned j = 0; j < lakeAndGreenSpaceAndIslandAndShoreline_SizeAndID.size(); j++) {
        featureSort.lakeAndGreenSpaceAndIslandsAndShoreline.push_back(lakeAndGreenSpaceAndIslandAndShoreline_SizeAndID[j].second);
    }
}

StreetsGraph::StreetsGraph(const StreetsGraph& orig) {

}

StreetsGraph::~StreetsGraph() {
    streetsGraphInstance.reset();
}

StreetsGraph* StreetsGraph::GetStreetsGraphInstance() {
    if (streetsGraphInstance != NULL)
        streetsGraphInstance.reset();
    streetsGraphInstance.reset(new StreetsGraph, deleteHelper);
    return streetsGraphInstance.get();
}

std::vector<unsigned> StreetsGraph::get_street_segments(unsigned streetID) const {
    return streetsVectorByID.at(streetID).segments;
}

std::vector<unsigned> StreetsGraph::get_street_ids(std::string name) const {
    std::vector<unsigned> streetsWithSameName;
    auto key_range = streetsMapByName.equal_range(name);
    for (auto iter = key_range.first; iter != key_range.second; iter++) {
        streetsWithSameName.push_back(iter->second);
    }
    return streetsWithSameName;
}

std::vector<unsigned> StreetsGraph::get_intersection_street_segments(unsigned intersection_id) const {
    return intersectionsByID.at(intersection_id).segment_ids;
}

//get street segments of an intersection by reference

std::vector<unsigned>& StreetsGraph::get_intersection_street_segments_by_reference(unsigned intersection_id) {
    return intersectionsByID.at(intersection_id).segment_ids;
}

StreetSegmentInfo StreetsGraph::get_street_segment_information(unsigned streetSegment_id) const {
    return streetSegmentsByID.at(streetSegment_id).basicInfo;
}

//get the reference of street segment

StreetSegmentInfo& StreetsGraph::get_street_segment(unsigned streetSegment_id) {
    return streetSegmentsByID.at(streetSegment_id).basicInfo;
}

//get street information using its id

streetInfo StreetsGraph::get_street_information(unsigned street_id) const {
    return streetsVectorByID.at(street_id);
}

std::vector<intersectionInfo>& StreetsGraph::intersections() {
    return intersectionsByID;
}

//get certain street intersection information by its id

const boost::unordered_set<unsigned>& StreetsGraph::get_street_intersections(unsigned streetID) {
    return streetsVectorByID.at(streetID).intersections;
}

//A helper function that converts LatLostreetsVectorByIDn to xy coordinates

std::pair<double, double> StreetsGraph::convert_LatLon_to_xy(LatLon point) {
    double lat_average = ((DEG_TO_RAD * latitude.first + DEG_TO_RAD * latitude.second) / 2);
    double x = (DEG_TO_RAD * point.lon) * cos(lat_average) * EARTH_RADIUS_IN_METERS;
    double y = DEG_TO_RAD * point.lat * EARTH_RADIUS_IN_METERS;
    return std::pair<double, double>(x, y);
}

double StreetsGraph::find_distance_between_two_points(LatLon point1, LatLon point2) {
    double lat_average = ((DEG_TO_RAD * point2.lat + DEG_TO_RAD * point1.lat) / 2.0);
    double x1 = (DEG_TO_RAD * point1.lon) * cos(lat_average);
    double y1 = DEG_TO_RAD * point1.lat;
    double x2 = (DEG_TO_RAD * point2.lon) * cos(lat_average);
    double y2 = DEG_TO_RAD * point2.lat;

    double distance_between_two_points = EARTH_RADIUS_IN_METERS * sqrt((y2 - y1) * (y2 - y1) + (x2 - x1)* (x2 - x1));
    return distance_between_two_points;
}

double StreetsGraph::calulate_segment_length(unsigned streetSegmentID) {
    double street_segment_length;
    double first_segment_length;
    double last_segment_length;
    double curve_length = 0.0;
    //street segment is straight
    if (getStreetSegmentInfo(streetSegmentID).curvePointCount == 0) {
        LatLon initial_position = getIntersectionPosition(getStreetSegmentInfo(streetSegmentID).from);
        LatLon final_position = getIntersectionPosition(getStreetSegmentInfo(streetSegmentID).to);
        street_segment_length = find_distance_between_two_points(initial_position, final_position);
        return street_segment_length;
    }// street segment has curves
    else {
        //distance from first intersection to the first curve point
        LatLon first_intersection = getIntersectionPosition(getStreetSegmentInfo(streetSegmentID).from);
        LatLon first_curve_point = getStreetSegmentCurvePoint(streetSegmentID, 0);
        first_segment_length = find_distance_between_two_points(first_intersection, first_curve_point);

        //distance from last curve point to the end intersection 
        LatLon last_curve_point = getStreetSegmentCurvePoint(streetSegmentID, (getStreetSegmentInfo(streetSegmentID).curvePointCount) - 1);
        LatLon end_intersection = getIntersectionPosition(getStreetSegmentInfo(streetSegmentID).to);
        last_segment_length = find_distance_between_two_points(last_curve_point, end_intersection);

        //start from the first curve point and end at the second last curve point
        for (unsigned curve_point_count = 0; curve_point_count < ((getStreetSegmentInfo(streetSegmentID).curvePointCount) - 1); curve_point_count++) {
            //fetch the latlon of the idx'th curve point
            LatLon first_curve_point = getStreetSegmentCurvePoint(streetSegmentID, curve_point_count);
            LatLon second_curve_point = getStreetSegmentCurvePoint(streetSegmentID, curve_point_count + 1);
            curve_length += find_distance_between_two_points(first_curve_point, second_curve_point);
        }
        street_segment_length = curve_length + first_segment_length + last_segment_length;
    }
    return street_segment_length;
}


//A helper function that computers center of polygon for text on features 

t_point StreetsGraph::getCenterOfPoly(t_point *vertices, unsigned total_vertices) {

    t_point centerOfPolygon = {0, 0};
    double signedArea = 0;
    double x0 = 0;
    double x1 = 0;
    double y0 = 0;
    double y1 = 0;
    double partialSignedArea = 0;
    //computer all vertices except for the last 

    unsigned vertex_count = 0;
    for (vertex_count = 0; vertex_count < (total_vertices - 1); vertex_count++) {
        x0 = vertices[vertex_count].x;
        y0 = vertices[vertex_count].y;
        x1 = vertices[vertex_count + 1].x;
        y1 = vertices[vertex_count + 1].y;
        partialSignedArea = x0 * y1 - x1*y0;
        signedArea += partialSignedArea;
        centerOfPolygon.x += (x0 + x1) * partialSignedArea;
        centerOfPolygon.y += (y0 + y1) * partialSignedArea;
    }
    //Do last vertex separately to avoid perfomring an expensive modulous operation in each iteration
    x0 = vertices[total_vertices - 1].x;
    y0 = vertices[total_vertices - 1].y;
    x1 = vertices[0].x;
    y1 = vertices[0].y;
    partialSignedArea = (x0 * y1) - (x1 * y0);
    signedArea += partialSignedArea;

    centerOfPolygon.x += (x0 + x1) * partialSignedArea;
    centerOfPolygon.y += (y0 + y1) * partialSignedArea;

    signedArea *= 0.5;
    centerOfPolygon.x /= (6.0 * signedArea);
    centerOfPolygon.y /= (6.0 * signedArea);

    return centerOfPolygon;
}
//  Public-domain function by Darel Rex Finley, 2006.
// find the area of a polygon given the number of vertices and array of t_points 

double StreetsGraph::polygonArea(t_point *vertices, unsigned points) {
    double area = 0.;
    unsigned i, j = points - 1;

    for (i = 0; i < points; i++) {
        area += (vertices[j].x + vertices[i].x)*(vertices[j].y - vertices[i].y);
        j = i;
    }

    return area * .5;
}

//convert from latitude and longitude to x y coordinates

std::pair<double, double> StreetsGraph::get_intersection_xy_helper(unsigned intersection_id) {
    //get LatLon from intersection
    LatLon intersection_coordinate = getIntersectionPosition(intersection_id);
    //convert to xy and return it
    return convert_LatLon_to_xy(intersection_coordinate);
}


//A helper function to get (x,y) coordinates of a given point of interest

std::pair<double, double> StreetsGraph::get_poi_xy_helper(unsigned poi_id) {
    //get LatLon from a point of interest
    LatLon poi_coordinate = getPointOfInterestPosition(poi_id);
    //convert to xy and return it
    return convert_LatLon_to_xy(poi_coordinate);
}

//find the area of the feature polygon given featureID

double StreetsGraph::find_feature_area(unsigned featureID) {
    const std::vector<std::pair<double, double>>&featurePoints_in_xy = get_feature_points(featureID);
    unsigned totalPointCount = getFeaturePointCount(featureID);
    t_point featurePoints[totalPointCount];
    //convert the pair type returned from get_feature_points to t_point type
    for (unsigned pointCount = 0; pointCount < totalPointCount; pointCount++) {
        featurePoints[pointCount].x = get_feature_points(featureID)[pointCount].first;
        featurePoints[pointCount].y = get_feature_points(featureID)[pointCount].second;
    }
    double area = abs(polygonArea(featurePoints, totalPointCount));
    return area;
}

unsigned StreetsGraph::find_nearest_intersection(LatLon point) {
    //convert to xy coordinates and set id to 0 (by convention)
    auto point_in_xy = convert_LatLon_to_xy(point);
    coordinates pointLookingFor(point_in_xy.first, point_in_xy.second, 0);
    //search in kdtree use find function
    coordinates result = *(intersection_kdtree.find_nearest(pointLookingFor).first);
    return result.ID;
}

unsigned StreetsGraph::find_nearest_intersection(double x, double y) {
    coordinates pointLookingFor(x, y, 0);
    //search in kdtree use find function
    coordinates result = *(intersection_kdtree.find_nearest(pointLookingFor).first);
    return result.ID;
}

unsigned StreetsGraph::find_nearest_poi(LatLon point) {
    //convert to xy coordinates and set id to 0 (by convention)
    auto point_in_xy = convert_LatLon_to_xy(point);
    coordinates pointLookingFor(point_in_xy.first, point_in_xy.second, 0);
    //search in kdtree use find function 
    coordinates result = *(poi_kdtree.find_nearest(pointLookingFor).first);
    return result.ID;
}

//A helper function that transfer curve point of one intersection to xy coordinates

void StreetsGraph::convert_curve_points(unsigned streetSegment_id) {
    unsigned numCurvePoint = streetSegmentsByID.at(streetSegment_id).basicInfo.curvePointCount;
    for (unsigned i = 0; i < numCurvePoint; i++) {
        LatLon curve_point = getStreetSegmentCurvePoint(streetSegment_id, i);
        std::pair<double, double> curve_point_in_xy = convert_LatLon_to_xy(curve_point);
        //add converted curve points to corresponding segment
        streetSegmentsByID.at(streetSegment_id).curve_points_in_xy.push_back(curve_point_in_xy);
    }
}


//check if the given intersection is highlighted or not

bool StreetsGraph::is_intersection_highlighted(unsigned intersection_id) const {
    return intersectionsByID.at(intersection_id).isHighlighted;
}

//return the reference to all curve points of one segment

std::vector<std::pair<double, double>> &StreetsGraph::get_curve_points(unsigned street_segment_id) {
    return streetSegmentsByID.at(street_segment_id).curve_points_in_xy;
}

//return all points on one feature

const std::vector<std::pair<double, double>> &StreetsGraph::get_feature_points(unsigned feature_id) {
    return featuresByID.at(feature_id).points_in_xy;
}

//return the boundary of the map,Xmin,Ymin,Xmax,Ymax respectively

double* StreetsGraph::get_bounds() {
    return bounds;
}

//get the height of bound

double StreetsGraph::get_max_height() const {
    return (bounds[3] - bounds[1]);
}

//get the width of bound

double StreetsGraph::get_max_width() const {
    return (bounds[2] - bounds[0]);
}

//return the xy position fo a given intersection id

std::pair<double, double> StreetsGraph::get_intersection_position(unsigned intersection_id) const {
    return intersectionsByID.at(intersection_id).position_in_xy;
}


//return tags of a given street segment 

const std::vector<std::pair<std::string, std::string>> &StreetsGraph::get_segment_tags(unsigned street_segment_id) const {
    return streetSegmentsByID.at(street_segment_id).tags;
}

const KDTree::KDTree<2, coordinates>& StreetsGraph::get_intersection_KDTree() const {
    return intersection_kdtree;
}


//find all intersections around a given point within certain range

std::vector<unsigned> StreetsGraph::get_intersections_in_range(double x, double y, double range) {
    std::vector<coordinates> temp_vector;
    coordinates pointLookingFor(x, y, 0);
    intersection_kdtree.find_within_range(pointLookingFor, range, back_insert_iterator<vector < coordinates >> (temp_vector));
    std::vector<unsigned> intersections_in_range;
    for (unsigned i = 0; i < temp_vector.size(); i++) {
        intersections_in_range.push_back(temp_vector[i].ID);
    }
    return intersections_in_range;
}

//return the xy poisition of a given point of interset id

std::pair<double, double> StreetsGraph::get_poi_position_in_xy(unsigned poi_id) const {
    return POIByID.at(poi_id).position;
}


//pass in true if want certain intersection highlight or dis-highlight

void StreetsGraph::set_intersection_hightlight(unsigned intersection_id, bool highlight) {
    intersectionsByID.at(intersection_id).isHighlighted = highlight;
}

//return sorted features

const FeatureSort & StreetsGraph::get_featureSort() {
    return featureSort;
}

const std::unordered_multimap<std::string, unsigned> & StreetsGraph::get_POINyName() {
    return POIByName;
}

std::vector<unsigned> StreetsGraph::get_poi_in_range(double x, double y, double range) {
    std::vector<coordinates> temp_vector;
    coordinates pointLookingFor(x, y, 0);
    poi_kdtree.find_within_range(pointLookingFor, range, back_insert_iterator<vector < coordinates >> (temp_vector));
    std::vector<unsigned> poi_in_range;
    for (unsigned i = 0; i < temp_vector.size(); i++) {
        poi_in_range.push_back(temp_vector[i].ID);
    }
    return poi_in_range;
}

//set all segments on one street highlight

void StreetsGraph::highLightStreets(unsigned street_id, bool turn_on) {
    for (unsigned i = 0; i < streetsVectorByID[street_id].segments.size(); i++) {
        unsigned streetSegmentID = streetsVectorByID[street_id].segments.at(i);
        streetSegmentsByID[streetSegmentID].isHightlighted = turn_on;
    }
}

bool StreetsGraph::isSegmentHighlighted(unsigned streetSegment_id) {
    return streetSegmentsByID[streetSegment_id].isHightlighted;

}

void StreetsGraph::set_segment_highlight(unsigned streetSegmentID, bool turn_on) {
    streetSegmentsByID[streetSegmentID].isHightlighted = turn_on;
}

double StreetsGraph::get_segment_travel_time(unsigned streetSegment_id) const {
    return streetSegmentsByID[streetSegment_id].travel_time;
}

int StreetsGraph::get_intersection_by_name(std::string name) const {
    auto iter = intersectionsByName.find(name);
    if (iter != intersectionsByName.end()) {
        return iter->second;
    } else
        return -1;
}

std::vector<std::string> & StreetsGraph::get_intersectionNames() {
    return intersectionNames;
}

std::vector<std::string> & StreetsGraph::get_POINames() {
    return POINames;
}

