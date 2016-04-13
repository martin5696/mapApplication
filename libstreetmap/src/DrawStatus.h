/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DrawStatus.h
 * Author: zhengz28
 *
 * Created on February 21, 2016, 8:53 AM
 */

#ifndef DRAWSTATUS_H
#define DRAWSTATUS_H
/* A helper structure to record the draw status of current map
 *
 */
struct DrawStatus{
    
    float last_clicked_x;
    float last_clicked_y;
    //things to do after enter key is pressed
    int enterPressedEvent;
    //whether map is successfully loaded
    bool successLoaded;
    bool showPOI;
    bool showFeatures;
    bool showRoute; 
    bool windowCreated;
    bool showPathResult;
    bool helpOn;
    bool showPOIRoute;
    std::vector<pair<double,double>> marks;
    std::vector<unsigned> highlighted_intersections;
    std::vector<unsigned> highlighted_streets;
    std::vector<unsigned> pathLookingFor;
    unsigned from_inter_to_poi;
  
};


#endif /* DRAWSTATUS_H */

