/* 
 * File:   StreetGraph.h
 * Author: zhengz28
 *
 * Created on January 18, 2016, 7:51 PM
 */

#ifndef STREETSGRAPH_H
#define STREETSGRAPH_H

#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include "kdTreeNode.h"
#include "PathNetwork.hpp"
#include "StreetsDatabase.h"
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "graphics.h"
#include "FeatureSort.h"

#define DEG_TO_RAD 0.017453292519943295769236907684886
#define EARTH_RADIUS_IN_METERS 6372797.560856
using namespace std;

//extern the global variable of streets DB
extern StreetsDatabase streetsDB;

//new type to stor the information about street

struct streetInfo {
    std::vector<unsigned> segments;
    boost::unordered_set<unsigned> intersections;
};
//new type to store the information about intersection

struct intersectionInfo {
    std::vector<unsigned> segment_ids;
    std::pair<double, double> position_in_xy;
    bool isHighlighted;
};
//new type to store the information about street segment

struct segmentInfo {
    StreetSegmentInfo basicInfo;
    std::vector<std::pair<double, double>> curve_points_in_xy;
    std::vector<std::pair<std::string, std::string>> tags;
    bool isHightlighted;
    double travel_time;
};

struct featureInfo {
    std::vector<pair<double, double>> points_in_xy;
};

struct poiInfo {
    std::pair<double, double> position;
};

class StreetsGraph {
private:
    StreetsGraph();
    StreetsGraph(const StreetsGraph& orig);
    ~StreetsGraph();

    //The smart pointer to the instance of the class
    static boost::shared_ptr<StreetsGraph> streetsGraphInstance;

    /*-------------Container related to street---------------*/
    //A vector container storing street information including intersections and segments
    std::vector<streetInfo> streetsVectorByID;
    //A map container storing  the streetInfo using street name
    std::unordered_multimap<std::string, unsigned> streetsMapByName;

    /*-------------Container related to intersections---------------*/
    //A vector container storing intersection information
    std::vector<intersectionInfo> intersectionsByID;
    //A kd-tree storing the intersections information
    KDTree::KDTree<2, coordinates> intersection_kdtree;
    //A hash table storing the intersections by name
    std::unordered_map<std::string, unsigned> intersectionsByName;
    std::vector<std::string> intersectionNames;
    
    /*-------------Container related to segments---------------*/
    //A vector container storing street segment information
    std::vector<segmentInfo> streetSegmentsByID;

    /*-------------Container related to features---------------*/
    //A vector container storing street segment information
    std::vector<featureInfo> featuresByID;
    //A special vector to store sorted features
    FeatureSort featureSort;
    
    /*-------------Container related to poi---------------*/
    //A kd-tree storing the point of interest information
    KDTree::KDTree<2, coordinates> poi_kdtree;
    std::vector<poiInfo> POIByID;
    std::unordered_multimap<std::string,unsigned> POIByName;
    std::vector<std::string> POINames;
    
    //A map container stors the OSW
    //the bound of the map,Xmin,Ymin,Xmax,Ymax respectively
    double bounds[4];

    //min and max latitudes respectively 
    std::pair<double, double> latitude;

    //find maximum and minimum latitude used to convert coordinates
    void bounds_helper();

    //A helper function that converts lonla to xy coordinates
    std::pair<double, double> convert_LatLon_to_xy(LatLon point);
    
    double calulate_segment_length(unsigned streetSegmentID);
    
    double find_distance_between_two_points(LatLon point1, LatLon point2);
    
    //A helper function that transfer curve point of one intersection to xy coordinates
    //this function must be called after basic info has been inserted in streetSegmentsByID
    void convert_curve_points(unsigned streetSegment_id);

    //Helper function to build data structure related to intersections
    //and get the bounds of the map
    void addIntersections();

    //Helper function to build data structure related to streets and segments
    void addStreetsAndSegments();

    //Helper function to build data structure related to point of interest
    void addPOI();

    //helper function to convert all points of a feature to xy coordinates
    void addFeatures();

    //Helper function to add OSMW Tags
    void addWayTags(OSMID osmid, unsigned street_segment_id);

    //helper function finds the longitude and latitude of one intersection
    std::pair<double, double> get_intersection_xy_helper(unsigned intersection_id);

    //helper function finds the longitude and latitude of one intersection
    std::pair<double, double> get_poi_xy_helper(unsigned poi_id);
    
    double find_feature_area(unsigned feature_id);

public:
    
    //return the Intersection KDTree structure
    const KDTree::KDTree<2, coordinates> & get_intersection_KDTree() const;

    //return the instance of this class
    static StreetsGraph* GetStreetsGraphInstance();

    //return features vector
    const std::vector<featureInfo>& features();
    
    //return the reference of intersections data base
    std::vector<intersectionInfo>& intersections();

    //get all street segments belonging to one street with a given id
    std::vector<unsigned> get_street_segments(unsigned streetId) const;

    //get all street ids with the same name
    std::vector<unsigned> get_street_ids(std::string name) const;

    //get all street segments on same intersection
    std::vector<unsigned> get_intersection_street_segments(unsigned intersection_id) const;

    //get street segments of an intersection by reference
    std::vector<unsigned>& get_intersection_street_segments_by_reference(unsigned intersection_id) ;
    
    //get certain street segment information using its id
    StreetSegmentInfo get_street_segment_information(unsigned streetSegment_id) const;
    
    //get the reference of street segment
    StreetSegmentInfo& get_street_segment(unsigned streetSegment_id);
    
    //get street information using its id
    streetInfo get_street_information(unsigned street_id) const;

    //get certain street intersection information by its id
    const boost::unordered_set<unsigned>& get_street_intersections(unsigned streetID);

    //get nearest intersection of a certain latlon
    unsigned find_nearest_intersection(LatLon point);

    //get nearest intersection of a certain (x,y)
    unsigned find_nearest_intersection(double x, double y);

    //find the nearest point of interest
    unsigned find_nearest_poi(LatLon point);

    //check if the given intersection is highlighted or not
    bool is_intersection_highlighted(unsigned intersection_id) const;

    //return the reference to all curve points of one segment
    std::vector<std::pair<double, double>> &get_curve_points(unsigned street_segment_id);

    //return all points on one feature
    const std::vector<std::pair<double, double>> & get_feature_points(unsigned feature_id);

    //return the xy poisition of a given point of interset id
    std::pair<double, double> get_poi_position_in_xy(unsigned poi_id) const;

    //return the boundary of the map,Xmin,Ymin,Xmax,Ymax respectively
    double* get_bounds();

    //get the height of bound
    double get_max_height() const;

    //get the width of bound
    double get_max_width() const;

    //return the xy position fo a given intersection id
    std::pair<double, double> get_intersection_position(unsigned intersection_id) const;

    //get tags of a given segment id
    const std::vector<std::pair<std::string, std::string>> &get_segment_tags(unsigned street_segment_id) const;
  
    //pass in true if want certain intersection highlight or dis-highlight
    void set_intersection_hightlight(unsigned intersection_id,bool highlight);
    
    std::vector<unsigned> get_intersections_in_range(double x, double y, double range);
    
    std::vector<unsigned> get_poi_in_range(double x, double y, double range);

    //A helper function that returns the centroid of a polygon 
    t_point getCenterOfPoly(t_point *vertices, unsigned total_vertices);
    
    double polygonArea(t_point *vertices, unsigned points);
    //return sorted features
    const FeatureSort & get_featureSort();
    
    const std::unordered_multimap<std::string,unsigned> & get_POINyName();
    
    //set all segments on one street highlight
    void highLightStreets(unsigned street_id,bool turn_on);
    
    void set_segment_highlight(unsigned streetSegmentID ,bool turn_on);
    
    bool isSegmentHighlighted (unsigned streetSegment_id);
    
    double get_segment_travel_time (unsigned streetSegment_id) const;
     
    //If not found, return -1
    int get_intersection_by_name(std::string name) const;
    
    std::vector<std::string> & get_intersectionNames();
    
    std::vector<std::string> & get_POINames();
    
    friend void deleteHelper(StreetsGraph *);
    

};

//This is a helper function passed into the shared pointer so that we can make
//destructor private
void deleteHelper(StreetsGraph *);


#endif /* STREETGRAPH_H */

