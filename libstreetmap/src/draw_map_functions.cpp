/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#define ARROW_ANGLE 0.5
#define PI 3.1415926
#include "draw_map_functions.h"
using namespace std;
//Global function declared in m1.h
extern StreetsGraph* streetsHelper;

//Given a segment id, then draw this segment on window

void draw_street_segment(unsigned streetSegmentID) {
    //set street property
    setlinestyle(SOLID);
    setlinewidth(3);
    t_bound_box current_view = get_visible_world();
    float total_area = streetsHelper->get_max_height() * streetsHelper->get_max_width();
    float current_area = current_view.area();
    float ratio = current_area / total_area;
    string highway_tag;

    setlinewidth(get_linewidth(current_view));

    //extract the highway tag for this street segment
    const std::vector<std::pair < std::string, std::string>> &segment_tag_vec = streetsHelper->get_segment_tags(streetSegmentID);
    for (unsigned i = 0; i < segment_tag_vec.size(); i++) {
        if (segment_tag_vec.at(i).first == "highway")
            highway_tag = segment_tag_vec.at(i).second;
    }

    //Always draw the highways/primary/secondary regardless of zoom level
    if (highway_tag == "motorway" || highway_tag == "motorway_link") {
        setcolor(ORANGE);
        draw_the_segment(streetSegmentID);
    } else if (highway_tag == "trunk" || highway_tag == "primary") {
        if (ratio < 0.05 || streetsHelper->isSegmentHighlighted(streetSegmentID))
            setcolor(t_color(254, 246, 149));
        else
            setcolor(WHITE);
        draw_the_segment(streetSegmentID);
    } else if (highway_tag == "secondary") {
        if (ratio < 0.05)
            setcolor(t_color(254, 246, 149));
        else
            setcolor(WHITE);
        draw_the_segment(streetSegmentID);
        //Draw the following roads when it's zoomed-in
    } else if (highway_tag == "tertiary" && ratio < 0.1) {
        setcolor(WHITE);
        draw_the_segment(streetSegmentID);
    } else if (highway_tag == "unclassified" && ratio < 0.1) {
        setcolor(WHITE);
        draw_the_segment(streetSegmentID);
    } else if ((ratio < 0.01) || streetsHelper->isSegmentHighlighted(streetSegmentID)) {
        setlinewidth(3);
        setcolor(t_color(216, 210, 200));
        draw_the_segment(streetSegmentID);
    }

    //call function to draw all street names
    draw_segment_text(streetSegmentID, 13);
}

//draw the names of all street segments

void draw_segment_text(unsigned streetSegmentID, int size) {
    t_bound_box current_view = get_visible_world();
    int width = get_linewidth(current_view);
    double current_height = current_view.get_height();
    double segment_length = find_street_segment_length(streetSegmentID);

    auto & curvePointsVector = streetsHelper->get_curve_points(streetSegmentID);
    StreetSegmentInfo segmentBasicInfo = streetsHelper->get_street_segment_information(streetSegmentID);
    auto curvePointsNum = curvePointsVector.size();

    //check zoom level to decide whether to draw names and arrows or not
    if ((segment_length / current_height) > 0.6) {

        std::pair<double, double> point1;
        std::pair<double, double> point2;
        std::pair<double, double> middle;
        double angle;

        //draw straight segments names
        if (curvePointsNum == 0) {
            string street_name = getStreetName(segmentBasicInfo.streetID);
            point1 = streetsHelper->get_intersection_position(segmentBasicInfo.from);
            point2 = streetsHelper->get_intersection_position(segmentBasicInfo.to);
            //draw arrow if it's one way
            if (segmentBasicInfo.oneWay)
                draw_single_arrow(point1, point2);
            //if the street name is "unknown", don't display the name
            if (street_name != "<unknown>") {

                middle.first = (point1.first + point2.first) / 2;
                middle.second = (point1.second + point2.second) / 2;
                setcolor(BLACK);
                angle = (atan((point2.second - point1.second) / (point2.first - point1.first)))*180 / 3.1415926;
                settextattrs(size, angle);

                drawtext(middle.first, middle.second, street_name, width * 20, width * 20);

            }
        }//draw curved segments names
            //draw name at every curve point
        else {
            //if curvepointsNum==1
            if (curvePointsNum == 1 && (segment_length / current_height) > 0.45) {
                std::pair<double, double> from = streetsHelper->get_intersection_position(segmentBasicInfo.from);
                std::pair<double, double> to = streetsHelper->get_intersection_position(segmentBasicInfo.to);
                string street_name = getStreetName(segmentBasicInfo.streetID);

                //handle first part from "from" to "curvepoint"
                if (segmentBasicInfo.oneWay)
                    draw_single_arrow(from, curvePointsVector.at(0));
                if (street_name != "<unknown>") {
                    middle.first = (from.first + curvePointsVector.at(0).first) / 2;
                    middle.second = (from.second + curvePointsVector.at(0).second) / 2;
                    double curve_length = sqrt(pow((curvePointsVector.at(0).first - from.first), 2) + pow((curvePointsVector.at(0).second - from.second), 2));
                    if (curve_length / current_height > 0.1) {
                        setcolor(BLACK);
                        angle = (atan((curvePointsVector.at(0).second - from.second) / (curvePointsVector.at(0).first - from.first)))*180 / 3.1415926;
                        settextattrs(size, angle);

                        drawtext(middle.first, middle.second, street_name, width * 20, width * 20);
                    }
                }

                //handle second part from "curvepoint" to "to"
                if (segmentBasicInfo.oneWay)
                    draw_single_arrow(curvePointsVector.at(0), to);
                //if the street name is "unknown", don't display the name
                if (street_name != "<unknown>") {
                    middle.first = (curvePointsVector.at(0).first + to.first) / 2;
                    middle.second = (curvePointsVector.at(0).second + to.second) / 2;
                    double curve_length = sqrt(pow((to.first - curvePointsVector.at(0).first), 2) + pow((to.second - curvePointsVector.at(0).second), 2));
                    if (curve_length / current_height > 0.1) {
                        setcolor(BLACK);
                        angle = (atan((to.second - curvePointsVector.at(0).second) / (to.first - curvePointsVector.at(0).first)))*180 / 3.1415926;
                        //cout<<angle<<"\n";
                        settextattrs(size, angle);

                        drawtext(middle.first, middle.second, street_name, width * 20, width * 20);
                    }
                }


            }//if curvePointsNum>=2
            else {
                for (unsigned i = 0; i < curvePointsNum - 1; i++) {
                    string street_name = getStreetName(segmentBasicInfo.streetID);
                    if ((i % 2 == 0)) {
                        point1 = curvePointsVector.at(i);
                        point2 = curvePointsVector.at(i + 1);
                        //draw_single_arrow
                        if (segmentBasicInfo.oneWay)
                            draw_single_arrow(point1, point2);

                        //if the street name is "unknown", don't display the name
                        if (street_name != "<unknown>") {
                            middle.first = (point1.first + point2.first) / 2;
                            middle.second = (point1.second + point2.second) / 2;
                            double curve_length = sqrt(pow((point2.first - point1.first), 2) + pow((point2.second - point1.second), 2));
                            if (curve_length / current_height > 0.1) {
                                setcolor(BLACK);
                                angle = (atan((point2.second - point1.second) / (point2.first - point1.first)))*180 / 3.1415926;
                                settextattrs(size, angle);

                                drawtext(middle.first, middle.second, street_name, width * 20, width * 20);
                            }
                        }
                    }

                }
            }

        }
    }

}

//draw arrow for one way streets

void draw_single_arrow(std::pair<double, double> point1, std::pair<double, double> point2) {
    std::pair<double, double> middle;
    std::pair<double, double>point3;
    std::pair<double, double>point4;
    std::pair<double, double>point5;
    setlinewidth(3);
    double angle;
    middle.first = (point1.first + point2.first) / 2;
    middle.second = (point1.second + point2.second) / 2;
    setcolor(t_color(89, 139, 226));
    double delta_x = point2.first - point1.first;
    double delta_y = point2.second - point1.second;

    //calculate angle
    if (delta_x == 0 && delta_y > 0)
        angle = PI / 2;
    else if (delta_x == 0 && delta_y < 0)
        angle = 3 * PI / 2;
    else if (delta_x == 0 && delta_y == 0)
        angle = 0;
    else {
        angle = atan((delta_y) / (delta_x));
        if (delta_x < 0 && delta_y > 0)
            angle = PI + angle;
        else if (delta_x < 0 && delta_y < 0)
            angle = PI + angle;
        else if (delta_x > 0 && delta_y < 0)
            angle = 2 * PI + angle;
    }
    //draw a line , 5 is the arrow leng
    int arrowLength = 4;
    int aroowSideLength = 2;
    point3.first = middle.first + arrowLength * cos(angle);
    point3.second = middle.second + arrowLength * sin(angle);

    drawline(middle.first, middle.second, point3.first, point3.second);

    point4.first = point3.first - aroowSideLength * cos(angle + ARROW_ANGLE);
    point4.second = point3.second - aroowSideLength * sin(angle + ARROW_ANGLE);

    point5.first = point3.first - aroowSideLength * cos(angle - ARROW_ANGLE);
    point5.second = point3.second - aroowSideLength * sin(angle - ARROW_ANGLE);

    //draw 2 other lines to form the arrow
    drawline(point3.first, point3.second, point4.first, point4.second);
    drawline(point3.first, point3.second, point5.first, point5.second);

}

//draw one street segment

void draw_the_segment(unsigned streetSegmentID) {
    if (streetsHelper->isSegmentHighlighted(streetSegmentID)) {
        setcolor(BLUE);
    }
    //read street segment basic information and all curve points on it
    StreetSegmentInfo segmentBasicInfo = streetsHelper->get_street_segment_information(streetSegmentID);
    auto & curvePointsVector = streetsHelper->get_curve_points(streetSegmentID);
    unsigned from_id = segmentBasicInfo.from;
    unsigned to_id = segmentBasicInfo.to;
    //convert two intersections to (x,y)
    auto from_point = streetsHelper->get_intersection_position(from_id);
    auto to_point = streetsHelper->get_intersection_position(to_id);
    auto curvePointsNum = curvePointsVector.size();
    if (curvePointsNum == 0) { // if no curve points on this segment
        //simply draw the street connecting two points
        drawline(from_point.first, from_point.second, to_point.first, to_point.second);
    } else {
        //first draw the line between from point and first curve point ,i.e index 0
        drawline(from_point.first, from_point.second, curvePointsVector[0].first, curvePointsVector[0].second);
        //connect all curve points to its next except first and last points
        for (unsigned i = 0; i < curvePointsNum - 1; i++) {

            drawline(curvePointsVector[i].first, curvePointsVector[i].second, curvePointsVector[i + 1].first, curvePointsVector[i + 1].second);
        }
        //connect last point to to_intersection
        drawline(curvePointsVector[curvePointsNum - 1].first, curvePointsVector[curvePointsNum - 1].second, to_point.first, to_point.second);
    }
}
void highlight_the_segment(unsigned streetSegmentID) {
    if(streetsHelper->isSegmentHighlighted(streetSegmentID)){
        setcolor(YELLOW);
    }
    //read street segment basic information and all curve points on it
    StreetSegmentInfo segmentBasicInfo = streetsHelper->get_street_segment_information(streetSegmentID);
    auto & curvePointsVector = streetsHelper->get_curve_points(streetSegmentID);
    unsigned from_id = segmentBasicInfo.from;
    unsigned to_id = segmentBasicInfo.to;
    //convert two intersections to (x,y)
    auto from_point = streetsHelper->get_intersection_position(from_id);
    auto to_point = streetsHelper->get_intersection_position(to_id);
    auto curvePointsNum = curvePointsVector.size();
    if (curvePointsNum == 0) { // if no curve points on this segment
        //simply draw the street connecting two points
        drawline(from_point.first, from_point.second, to_point.first, to_point.second);
    } else {
        //first draw the line between from point and first curve point ,i.e index 0
        drawline(from_point.first, from_point.second, curvePointsVector[0].first, curvePointsVector[0].second);
        //connect all curve points to its next except first and last points
        for (unsigned i = 0; i < curvePointsNum - 1; i++) {

            drawline(curvePointsVector[i].first, curvePointsVector[i].second, curvePointsVector[i + 1].first, curvePointsVector[i + 1].second);
        }
        //connect last point to to_intersection
        drawline(curvePointsVector[curvePointsNum - 1].first, curvePointsVector[curvePointsNum - 1].second, to_point.first, to_point.second);
    }
}  
void draw_feature(unsigned featureID) {
    const std::vector<std::pair<double, double>>&featurePoints_in_xy = streetsHelper -> get_feature_points(featureID);
    unsigned totalPointCount = getFeaturePointCount(featureID);
    t_point featurePoints[totalPointCount];
    //convert the pair type returned from get_feature_points to t_point type
    for (unsigned pointCount = 0; pointCount < totalPointCount; pointCount++) {
        featurePoints[pointCount].x = streetsHelper -> get_feature_points(featureID)[pointCount].first;
        featurePoints[pointCount].y = streetsHelper -> get_feature_points(featureID)[pointCount].second;
    }
    //Get current feature type
    FeatureType featureType = getFeatureType(featureID);
    //check the feature type current feature belongs to , then draw corresponding feature
    if (featureType == Unknown) {
        setcolor(t_color(233, 229, 220));
        fillpoly(featurePoints, totalPointCount);
    } else if (featureType == Park || featureType == Greenspace || featureType == Golfcourse) {
        //set color to lemon green
        setcolor(t_color(202, 223, 170));
        fillpoly(featurePoints, totalPointCount);
    } else if (featureType == Beach) {
        //color number FFCC99
        setcolor(t_color(250, 242, 199));
        fillpoly(featurePoints, totalPointCount);
    } else if (featureType == Lake) {
        //set color to sea blue
        setcolor(t_color(160, 195, 255));
        fillpoly(featurePoints, totalPointCount);
    } else if (featureType == River) {
        //set color to sea blue
        setcolor(t_color(160, 195, 255));
        setlinestyle(SOLID);
        setlinewidth(3);
        for (unsigned i = 0; i < totalPointCount - 1; i++) {
            drawline(featurePoints_in_xy[i].first, featurePoints_in_xy[i].second, featurePoints_in_xy[i + 1].first, featurePoints_in_xy[i + 1].second);
        }
    } else if (featureType == Island || featureType == Shoreline) {
        setcolor(t_color(233, 229, 220));
        fillpoly(featurePoints, totalPointCount);
    } else if (featureType == Building) {
        //set color to silver
        setcolor(t_color(244, 243, 236));
        fillpoly(featurePoints, totalPointCount);
    } else if (featureType == Stream) {
        setcolor(t_color(160, 195, 255));
        setlinestyle(SOLID);
        setlinewidth(2);
        for (unsigned i = 0; i < totalPointCount - 1; i++) {
            drawline(featurePoints_in_xy[i].first, featurePoints_in_xy[i].second, featurePoints_in_xy[i + 1].first, featurePoints_in_xy[i + 1].second);
        }
    }
}

void draw_POI(unsigned pointOfInterestID) {
    settextattrs(10, 0);
    std::pair<double, double> pointOfInterest = streetsHelper -> get_poi_position_in_xy(pointOfInterestID);
    std::string POItype = getPointOfInterestType(pointOfInterestID);
    std::string POIname = getPointOfInterestName(pointOfInterestID);
    t_bound_box currentView = get_visible_world();
    float currentScreenWidth = currentView.get_width();
    float currentScreenHeight = currentView.get_height();
    double maxScreenWidth = streetsHelper -> get_max_width();
    double maxScreenHeight = streetsHelper -> get_max_height();
    float aspectHeightRatio = 0.2 * (maxScreenHeight / currentScreenHeight);
    float aspectWidthRatio = 0.2 * (maxScreenWidth / currentScreenWidth);

    float x1 = pointOfInterest.first - 10;
    float x2 = pointOfInterest.first + 10;

    float y1 = pointOfInterest.second - 2;
    float y2 = pointOfInterest.second + 2;


    fillarc(10000, 10000, 100000, 0, 360);
    float bound_x = abs(x2 - x1) * aspectWidthRatio;
    float bound_y = abs(y2 - y1) * aspectHeightRatio;
    setcolor(BLACK);
    setfontsize(8);
    drawtext(pointOfInterest.first, pointOfInterest.second, POIname, bound_x, bound_y);
    t_point cross[14];

    cross[0].x = pointOfInterest.first - 0.5;
    cross[0].y = pointOfInterest.second - 3;
    cross[1].x = pointOfInterest.first - 0.5;
    cross[1].y = pointOfInterest.second - 4;
    cross[2].x = pointOfInterest.first - 1.5;
    cross[2].y = pointOfInterest.second - 4;
    cross[3].x = pointOfInterest.first - 1.5;
    cross[3].y = pointOfInterest.second - 5;
    cross[4].x = pointOfInterest.first - 0.5;
    cross[4].y = pointOfInterest.second - 5;
    cross[5].x = pointOfInterest.first - 0.5;
    cross[5].y = pointOfInterest.second - 6;
    cross[6].x = pointOfInterest.first - 0.5;
    cross[6].y = pointOfInterest.second - 7;
    cross[7].x = pointOfInterest.first + 0.5;
    cross[7].y = pointOfInterest.second - 7;
    cross[8].x = pointOfInterest.first + 0.5;
    cross[8].y = pointOfInterest.second - 6;
    cross[9].x = pointOfInterest.first + 0.5;
    cross[9].y = pointOfInterest.second - 5;
    cross[10].x = pointOfInterest.first + 1.5;
    cross[10].y = pointOfInterest.second - 5;
    cross[11].x = pointOfInterest.first + 1.5;
    cross[11].y = pointOfInterest.second - 4;
    cross[12].x = pointOfInterest.first + 0.5;
    cross[12].y = pointOfInterest.second - 4;
    cross[13].x = pointOfInterest.first + 0.5;
    cross[13].y = pointOfInterest.second - 3;

    t_point transportation[12];

    transportation[0].x = pointOfInterest.first - 0.5;
    transportation[0].y = pointOfInterest.second - 4;
    transportation[1].x = pointOfInterest.first - 1.5;
    transportation[1].y = pointOfInterest.second - 4;
    transportation[2].x = pointOfInterest.first - 1.5;
    transportation[2].y = pointOfInterest.second - 5;
    transportation[3].x = pointOfInterest.first - 0.5;
    transportation[3].y = pointOfInterest.second - 5;
    transportation[4].x = pointOfInterest.first - 0.5;
    transportation[4].y = pointOfInterest.second - 6;
    transportation[5].x = pointOfInterest.first - 0.5;
    transportation[5].y = pointOfInterest.second - 7;
    transportation[6].x = pointOfInterest.first + 0.5;
    transportation[6].y = pointOfInterest.second - 7;
    transportation[7].x = pointOfInterest.first + 0.5;
    transportation[7].y = pointOfInterest.second - 6;
    transportation[8].x = pointOfInterest.first + 0.5;
    transportation[8].y = pointOfInterest.second - 5;
    transportation[9].x = pointOfInterest.first + 1.5;
    transportation[9].y = pointOfInterest.second - 5;
    transportation[10].x = pointOfInterest.first + 1.5;
    transportation[10].y = pointOfInterest.second - 4;
    transportation[11].x = pointOfInterest.first + 0.5;
    transportation[11].y = pointOfInterest.second - 4;

    t_point restaurant[19];
    restaurant[0].x = pointOfInterest.first - 0.25;
    restaurant[0].y = pointOfInterest.second - 5;
    restaurant[1].x = pointOfInterest.first - 0.75;
    restaurant[1].y = pointOfInterest.second - 5;
    restaurant[2].x = pointOfInterest.first - 0.75;
    restaurant[2].y = pointOfInterest.second - 3;
    restaurant[3].x = pointOfInterest.first - 1;
    restaurant[3].y = pointOfInterest.second - 3;
    restaurant[4].x = pointOfInterest.first - 1.25;
    restaurant[4].y = pointOfInterest.second - 3;
    restaurant[5].x = pointOfInterest.first - 1.25;
    restaurant[5].y = pointOfInterest.second - 5;
    restaurant[6].x = pointOfInterest.first - 0.25;
    restaurant[6].y = pointOfInterest.second - 5.5;
    restaurant[7].x = pointOfInterest.first - 0.25;
    restaurant[7].y = pointOfInterest.second - 7.5;
    restaurant[8].x = pointOfInterest.first + 0.25;
    restaurant[8].y = pointOfInterest.second - 7.5;
    restaurant[9].x = pointOfInterest.first + 0.25;
    restaurant[9].y = pointOfInterest.second - 5.5;
    restaurant[10].x = pointOfInterest.first + 1.25;
    restaurant[10].y = pointOfInterest.second - 5;

    restaurant[11].x = pointOfInterest.first + 1.25;
    restaurant[11].y = pointOfInterest.second - 3;
    restaurant[12].x = pointOfInterest.first + 1;
    restaurant[12].y = pointOfInterest.second - 3;
    restaurant[13].x = pointOfInterest.first + 0.75;
    restaurant[13].y = pointOfInterest.second - 3;
    restaurant[14].x = pointOfInterest.first + 0.75;
    restaurant[14].y = pointOfInterest.second - 5;
    restaurant[15].x = pointOfInterest.first + 0.25;
    restaurant[15].y = pointOfInterest.second - 5;
    restaurant[16].x = pointOfInterest.first + 0.25;
    restaurant[16].y = pointOfInterest.second - 3;
    restaurant[17].x = pointOfInterest.first;
    restaurant[17].y = pointOfInterest.second - 3;
    restaurant[18].x = pointOfInterest.first - 0.25;
    restaurant[18].y = pointOfInterest.second - 3;

    if (POItype == "bar") {
        setcolor(BLACK);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(WHITE);
        fillpoly(restaurant, 19);
    } else if (POItype == "fast_food") {
        setcolor(BLACK);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(WHITE);
        fillpoly(restaurant, 19);
    } else if (POItype == "pharmacy") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "orthodonists") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "restaurant") {
        setcolor(BLACK);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(WHITE);
        fillpoly(restaurant, 19);
        ;
    } else if (POItype == "cafe") {
        setcolor(BLACK);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(WHITE);
        fillpoly(restaurant, 19);
    } else if (POItype == "fuel") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(LIGHTSKYBLUE);
        fillpoly(transportation, 12);
    } else if (POItype == "vending_machine") {
        setcolor(BLACK);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(WHITE);
        fillpoly(restaurant, 19);
    } else if (POItype == "dentist") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "pub") {
        setcolor(BLACK);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(WHITE);
        fillpoly(restaurant, 19);
    } else if (POItype == "bus_station") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(LIGHTSKYBLUE);
        fillpoly(transportation, 12);
    } else if (POItype == "ice_cream") {
        setcolor(BLACK);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(WHITE);
        fillpoly(restaurant, 19);
    } else if (POItype == "doctors") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "car_rental") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(LIGHTSKYBLUE);
        fillpoly(transportation, 12);
    } else if (POItype == "clinic") {
        setcolor(WHITE);
        float radius = 3;

        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "chiropodist") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "car_sharing") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(LIGHTSKYBLUE);
        fillpoly(transportation, 12);
    } else if (POItype == "bicycle_parking") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(LIGHTSKYBLUE);
        fillpoly(transportation, 12);
    } else if (POItype == "psychologist") {
        setcolor(WHITE);
        float radius = 3;

        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "pet_spa") {
        setcolor(PURPLE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
    } else if (POItype == "veterinary") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "ems_station") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "physiotherapist") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "drinking_water") {
        setcolor(BLACK);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(WHITE);
        fillpoly(restaurant, 19);
    } else if (POItype == "bicycle_rental") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(LIGHTSKYBLUE);
        fillpoly(transportation, 12);
    } else if (POItype == "hospital") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "ambulance_station") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "orthodontists") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "food_court") {
        setcolor(RED);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
    } else if (POItype == "optometrist") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "chiropractor") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "old_fast_food") {
        setcolor(BLACK);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(WHITE);
        fillpoly(restaurant, 19);
    } else if (POItype == "old_cafe") {
        setcolor(BLACK);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(WHITE);
        fillpoly(restaurant, 19);
    } else if (POItype == "office_supplies") {
        setcolor(GREEN);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
    } else if (POItype == "aromatherapy") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);

    } else if (POItype == "old_fuel") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(LIGHTSKYBLUE);
        fillpoly(transportation, 12);
    } else if (POItype == "pharmacy;restaurant") {
        setcolor(BLACK);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(WHITE);
        fillpoly(restaurant, 19);
    } else if (POItype == "cafe;bakery") {
        setcolor(BLACK);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(WHITE);
        fillpoly(restaurant, 19);
    } else if (POItype == "old_restaurant") {
        setcolor(BLACK);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(WHITE);
        fillpoly(restaurant, 19);
    } else if (POItype == "old_car_dealer") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(LIGHTSKYBLUE);
        fillpoly(transportation, 12);
    } else if (POItype == "taxi") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(LIGHTSKYBLUE);
        fillpoly(transportation, 12);
    } else if (POItype == "brewery") {
        setcolor(ORANGE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
    } else if (POItype == "truck_rental") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(LIGHTSKYBLUE);
        fillpoly(transportation, 12);
    } else if (POItype == "addiction_clinic") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "endodontics") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "doctors;dentist") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "natural_medicine") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "blood_donor_clinic") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "medical_labs") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "future_fast_food") {
        setcolor(BLACK);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(WHITE);
        fillpoly(restaurant, 19);
    } else if (POItype == "hair_loss") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "naturopathy") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "orthodontist") {
        setcolor(BLUE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
    } else if (POItype == "bakery") {
        setcolor(BLACK);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(WHITE);
        fillpoly(restaurant, 19);
    } else if (POItype == "reataurant") {
        setcolor(BLACK);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(WHITE);
        fillpoly(restaurant, 19);
    } else if (POItype == "health_specialty:chiropractic") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "deli") {
        setcolor(BLACK);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(WHITE);
        fillpoly(restaurant, 19);
    } else if (POItype == "optician") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "truck_rental") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(LIGHTSKYBLUE);
        fillpoly(transportation, 12);
    } else if (POItype == "enthodontics") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "doctors;dentist") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "sweets") {
        setcolor(BLACK);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(WHITE);
        fillpoly(restaurant, 19);
    } else if (POItype == "health_center") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "ferry_terminal") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(LIGHTSKYBLUE);
        fillpoly(transportation, 12);
    } else if (POItype == "cosmetic_surgery") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "natural_healing") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "restaurant;psychologist") {
        setcolor(BLACK);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(WHITE);
        fillpoly(restaurant, 19);
    } else if (POItype == "old_car_rental") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(LIGHTSKYBLUE);
        fillpoly(transportation, 12);
    } else if (POItype == "mechanic") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(LIGHTSKYBLUE);
        fillpoly(transportation, 12);
    } else if (POItype == "emergency_room") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "charging_station") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(LIGHTSKYBLUE);
        fillpoly(transportation, 12);
    } else if (POItype == "reflexology") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else if (POItype == "orthodonists") {
        setcolor(WHITE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
        setcolor(RED);
        fillpoly(cross, 14);
    } else {
        setcolor(ORANGE);
        float radius = 3;
        fillarc(pointOfInterest.first, (pointOfInterest.second - 5), radius, 0, 360);
    }

}

void draw_intersection(unsigned i) {
    float radius = 3;
    std::vector<intersectionInfo> &inters = streetsHelper->intersections();
    if (streetsHelper->is_intersection_highlighted(i)) {
        setcolor(CORAL);
        fillarc(inters.at(i).position_in_xy.first, inters.at(i).position_in_xy.second, radius, 0, 360);
    } else {
        setcolor(WHITE);
        fillarc(inters.at(i).position_in_xy.first, inters.at(i).position_in_xy.second, radius, 0, 360);

    }
}

//draw the symbol when any point on the map is clicked

void draw_clicked_symbol(float x, float y) {

    t_bound_box current_view = get_visible_world();
    float total_area = streetsHelper->get_max_height() * streetsHelper->get_max_width();
    float current_area = current_view.area();
    float ratio = current_area / total_area;
    float load_factor = 100 / pow(ratio, 0.5);
    float resizing_factor = ratio*load_factor;

    setcolor(CORAL);
    t_point polypts[3] = {
        {x, y},
        {x - 16 * resizing_factor, y + 36 * resizing_factor},
        {x + 16 * resizing_factor, y + 36 * resizing_factor}
    };
    fillpoly(polypts, 3);

    float circle_center_y = y + 45 * resizing_factor;
    float radius = 32 * resizing_factor / 3;

    fillarc(x, circle_center_y, radius, 0, 360);

}
void draw_point(float x, float y){
    setcolor(RED);
  
    float radius = 3;

    fillarc(x,y,radius,0,360);    
}
void highlight_route(vector<unsigned> path , bool turn_on) {
    for(unsigned i=0; i<path.size(); i++ ){
        streetsHelper->set_segment_highlight(path[i],turn_on);
    }

}
//print out the information of the closest intersection to the terminal
//if user clicked on the intersection, return intersection id 
//else, return -1

int display_intersection_info(float x, float y) {
    std::vector<unsigned> inters_in_range = streetsHelper->get_intersections_in_range(x, y, 2);
    std::vector<intersectionInfo> &inters = streetsHelper->intersections();
    //clicked on an intersection
    if (inters_in_range.size() == 1) {
        unsigned nearest_int_id = inters_in_range.at(0);
        if (streetsHelper->is_intersection_highlighted(nearest_int_id)) {
            //highlight_intersection(int_point.first, int_point.second, "WHITE");
            inters.at(nearest_int_id).isHighlighted = false;
          //  cout << "Deselecting:\n";
        } else {
            //highlight_intersection(int_point.first, int_point.second, "CORAL");
            inters.at(nearest_int_id).isHighlighted = true;
           // cout << "Selecting:\n";
        }
        auto point_in_LatLon = getIntersectionPosition(nearest_int_id);
        cout << "Intersection ID: " << nearest_int_id << "\n";
        cout << "Intersection Name: " << getIntersectionName(nearest_int_id) << "\n";
        cout << "Position: (" << point_in_LatLon.lat << "," << point_in_LatLon.lon << ")\n";
        cout << "====================================================================\n";


        return nearest_int_id;

    }

    return -1;
}

//print out the information of the closest intersection to the terminal
//if user clicked on the intersection, return intersection id 
//else, return -1

int display_poi_info(float x, float y) {
    std::vector<unsigned> poi_in_range = streetsHelper->get_poi_in_range(x, y, 2);
    //clicked on an intersection
    if (poi_in_range.size() == 1) {
        unsigned nearest_poi_id = poi_in_range.at(0);
        auto point_in_LatLon = getPointOfInterestPosition(nearest_poi_id);
        cout << "POI ID: " << nearest_poi_id << "\n";
        cout << getPointOfInterestType(nearest_poi_id) << " : " << getPointOfInterestName(nearest_poi_id) << "\n";
        cout << "Position: (" << point_in_LatLon.lat << "," << point_in_LatLon.lon << ")\n";
        cout << "====================================================================\n";

        return nearest_poi_id;
    }

    return -1;
}
//get a proper road width to draw depending on current view

int get_linewidth(t_bound_box current) {
    //Use fucntion w = 6640/x + 2.86 to determine the line width to draw
    //the coefficient of this function can be adjusted
    int linewidth = floor(6640 / current.get_width() + 2.86);

    return linewidth;
}

//return (x,y) of the intersection that should be highlighted from the mouse click

std::pair<float, float> get_last_clicked_inter(float x, float y) {
    unsigned nearest_int_id = streetsHelper->find_nearest_intersection(x, y);
    auto int_point = streetsHelper->get_intersection_position(nearest_int_id);
    return int_point;
}

//

void display_intersection_information_on_screen(unsigned intersection_id) {
    stringstream ss1, ss2;
    //four lines of information
    string line[4];
    ss1 << intersection_id;
    line[0] = "Intersection ID: " + ss1.str();
    line[1] = "Intersection Name: " + getIntersectionName(intersection_id);
    auto int_point = getIntersectionPosition(intersection_id);
    ss2 << int_point.lat << "," << int_point.lon;
    line[2] = "Position: ( " + ss2.str() + " )";
    line[3] = "====================================";
    t_bound_box current_view = get_visible_world();
    //set information box equals half of the screen
    double width = 0.5 * current_view.get_width();
    double height = 0.1 * current_view.get_height();
    t_point bottomLeft(current_view.get_width()*0.25, height * 0.01);
    bottomLeft = bottomLeft + current_view.bottom_left();
    t_bound_box Info_box(bottomLeft, width, height);
    setcolor(WHITE);
    fillrect(Info_box);
    setcolor(BLACK);
    //print three lines of information;
    for (int i = 0; i < 4; i++) {
        setcolor(BLACK);
        settextattrs(8, 0);
        t_point newBottomLeft(Info_box.left(), Info_box.top() - (i + 1) / 4.0 * Info_box.get_height());
        t_bound_box temp_box(newBottomLeft, Info_box.get_width(), Info_box.get_height() * 0.25);
        drawtext(temp_box.get_center(), line[i], temp_box);
        //drawtext(temp_box.get_center(),line[i],temp_box);
    }
}


//show information about a clicked point of interest 

void display_poi_information_on_screen(unsigned poi_id) {
    stringstream ss1, ss2;
    //four lines of information
    string line[4];
    ss1 << poi_id;
    line[0] = "POI ID: " + ss1.str();
    line[1] = getPointOfInterestType(poi_id) + " : " + getPointOfInterestName(poi_id);
    auto point_in_LatLon = getPointOfInterestPosition(poi_id);
    ss2 << point_in_LatLon.lat << "," << point_in_LatLon.lon;
    line[2] = "Position: ( " + ss2.str() + " )";
    line[3] = "====================================";
    t_bound_box current_view = get_visible_world();
    //set information box equals half of the screen
    double width = 0.5 * current_view.get_width();
    double height = 0.1 * current_view.get_height();
    t_point bottomLeft(current_view.get_width()*0.25, height * 0.01);
    bottomLeft = bottomLeft + current_view.bottom_left();
    t_bound_box Info_box(bottomLeft, width, height);
    setcolor(WHITE);
    fillrect(Info_box);
    setcolor(BLACK);
    //print three lines of information;
    for (int i = 0; i < 4; i++) {
        setcolor(BLACK);
        settextattrs(8, 0);
        t_point newBottomLeft(Info_box.left(), Info_box.top() - (i + 1) / 4.0 * Info_box.get_height());
        t_bound_box temp_box(newBottomLeft, Info_box.get_width(), Info_box.get_height() * 0.25);
        drawtext(temp_box.get_center(), line[i], temp_box);
        //drawtext(temp_box.get_center(),line[i],temp_box);
    }
}
