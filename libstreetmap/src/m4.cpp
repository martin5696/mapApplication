#include <algorithm>

#include "m4.h"
#define INF 0x7FFFFFFF
#define TIME_LIMIT 30
#define NUM_OF_THREADS 4
using namespace std;
extern StreetsGraph* streetsHelper;

/*-------------Some helper functions ------------*/

static void buildCostMap(std::unordered_map<unsigned,std::unordered_map<unsigned,double>>& cost_map,
        const std::vector<DeliveryInfo>& deliveries,const std::vector<unsigned>& depots);

static void buildCostMap_helper(unsigned start,unsigned end,vector<unsigned>& all_targets,std::unordered_map<unsigned,std::unordered_map<unsigned,double>>& cost_map);

//Build two kdtrees for pickUp and drop off places
static void buildDeliveriesTree(KDTree::KDTree<2, coordinates>& pickUpTree, KDTree::KDTree<2, coordinates>& dropOffTree,
        KDTree::KDTree<2, coordinates>& depotTree, const std::vector<DeliveryInfo>& deliveries, const std::vector<unsigned>& depots);

static void generateFirstPath(KDTree::KDTree<2, coordinates>& pickUpTree, KDTree::KDTree<2, coordinates>& dropOffTree, KDTree::KDTree<2, coordinates>& depotTree,
        const std::vector<DeliveryInfo>& deliveries, const std::vector<unsigned>& depots, vector<unsigned> & path_of_inter, vector<unsigned> & final_path,
        std::unordered_map<unsigned,std::unordered_map<unsigned,double>>& cost_map);

//get Manhattan distance between two intersections
static double get_manhattan_distance(unsigned intersection1, unsigned intersection2);

//
static double calculate_actual_path_cost(std::unordered_map<unsigned,std::unordered_map<unsigned,double>>& cost_map,const vector<unsigned> & path_of_inter);

//get the total geographic distance of given intersections
static double calculate_path_geo_distance(const vector<unsigned> & path_of_inter);

//Update the closest first and last depots of given path of intersections
static void update_depot_place(vector<unsigned> & path_of_inter ,const std::vector<unsigned>& depots,
         std::unordered_map<unsigned,std::unordered_map<unsigned,double>>& cost_map);

//Return true if the given path is valid.It means it starts from one depot and goes to every
//pick-up and drop-off intersections.Every pick-up is before corresponding drop-off
static bool isPathValid(vector<unsigned>& path, const vector<DeliveryInfo>& deliveries, const vector<unsigned>& depots);

static void generateRandomLegalPath(vector<unsigned>& path, const vector<DeliveryInfo>& deliveries, const vector<unsigned>& depots);

static void randomSearch(vector<unsigned>& path_of_inter,const vector<DeliveryInfo>& deliveries,
        const vector<unsigned>& depots, KDTree::KDTree<2, coordinates>& depotTree,chrono::high_resolution_clock::time_point startTime,
        std::unordered_map<unsigned,std::unordered_map<unsigned,double>>& cost_map);

//Given a vector of intersections,convert it to a path of segments
//If two adjacent intersection don't have a legal path,return false
static bool convert_inter_to_segments_path(vector<unsigned>& path_of_inter,vector<unsigned>& path);

//Simulated annealing , return true if accept move
static bool SA(double path_difference,double time);

static void twoOptSwap(vector<unsigned>& path_of_inter,unsigned i , unsigned k);

static void threeOptSwap(vector<unsigned>& path_of_inter,unsigned i , unsigned k);

static void randomTwoOpt(vector<unsigned>& path_of_inter,unsigned i , unsigned k);

static void randomThreeOpt(vector<unsigned>& path_of_inter,unsigned i , unsigned j, unsigned k);

static void randomTwoOptAndCheck(vector<unsigned>& path_of_inter,unsigned i , unsigned k , double& time,
         const vector<DeliveryInfo>& deliveries, const vector<unsigned>& depots,std::unordered_map<unsigned,std::unordered_map<unsigned,double>>& cost_map);

static void twoOptSwapAndCheck(vector<unsigned>& path_of_inter,unsigned i , unsigned k , double& time,
         const vector<DeliveryInfo>& deliveries, const vector<unsigned>& depots,std::unordered_map<unsigned,std::unordered_map<unsigned,double>>& cost_map);



std::vector<unsigned> traveling_courier(const std::vector<DeliveryInfo>& deliveries, const std::vector<unsigned>& depots) {
    chrono::high_resolution_clock::time_point startTime = chrono::high_resolution_clock::now();
    vector <unsigned> path_of_inter;
    vector <unsigned> final_path;
    KDTree::KDTree<2, coordinates>pickUpTree,dropOffTree, depotTree;
    unordered_map<unsigned,unordered_map<unsigned,double>> cost_map;
    
    buildDeliveriesTree(pickUpTree,dropOffTree,depotTree,deliveries,depots);
    
    buildCostMap(cost_map,deliveries,depots);
    
    generateFirstPath(pickUpTree,dropOffTree,depotTree,deliveries,depots, path_of_inter,final_path,cost_map);
    

    if(path_of_inter.empty()){
        cout << "empty" <<endl;
        generateRandomLegalPath(path_of_inter,deliveries,depots);
    }
//    generateRandomLegalPath(path_of_inter,deliveries,depots);
//    randomSearch(final_path,path_of_inter,deliveries,depots,depotTree,startTime);
    
//    auto currentTime = chrono::high_resolution_clock::now();
//    auto wallClock =
//                    chrono::duration_cast<chrono::duration<double>>(currentTime - startTime);
//    cout << wallClock.count()<<endl;
    
    if(deliveries.size() > 5){
        randomSearch(path_of_inter,deliveries,depots,depotTree,startTime,cost_map);
    }
    
    convert_inter_to_segments_path(path_of_inter,final_path);
    
    return final_path;
}

/*-------------Some helper functions ------------*/
//build kdtree to increase the speed of helping find the closest point
//Note: id in each kdtree corresponds to the number in delivery vector and depot vector
static void buildDeliveriesTree(KDTree::KDTree<2, coordinates>& pickUpTree,KDTree::KDTree<2, coordinates>& dropOffTree,
        KDTree::KDTree<2, coordinates>& depotTree,const std::vector<DeliveryInfo>& deliveries,const std::vector<unsigned>& depots){

    for(unsigned i = 0 ; i < deliveries.size(); i ++){
        auto deliveryInfo = deliveries[i];
        auto pickUp_pos = streetsHelper->get_intersection_position(deliveryInfo.pickUp);
        pickUpTree.insert(coordinates(pickUp_pos.first,pickUp_pos.second,i));

        auto dropOff_pos = streetsHelper->get_intersection_position(deliveryInfo.dropOff);
        dropOffTree.insert(coordinates(dropOff_pos.first,dropOff_pos.second,i));
    }

    for(unsigned i = 0 ; i < depots.size(); i ++){
        auto depot_pos = streetsHelper->get_intersection_position(depots[i]);
        depotTree.insert(coordinates(depot_pos.first, depot_pos.second, i));
    }
}


static void buildCostMap(std::unordered_map<unsigned,std::unordered_map<unsigned,double>>& cost_map,
        const std::vector<DeliveryInfo>& deliveries,const std::vector<unsigned>& depots){
    
    vector<unsigned> tempVector;
    
    for(unsigned  i = 0; i< deliveries.size();i++){
        tempVector.push_back(deliveries[i].pickUp);
        tempVector.push_back(deliveries[i].dropOff);
    }
    
    tempVector.insert(tempVector.end(),depots.begin(),depots.end());
    
    //Initialize the two dimension hash table
    for(unsigned i = 0; i < tempVector.size(); i ++){
        for(unsigned j = 0 ; j < tempVector.size(); j++){
            cost_map[tempVector[i]][tempVector[j]] = INF;
        }
    }
    
    unsigned size = tempVector.size();
    thread myThread[NUM_OF_THREADS];
    
    for(unsigned ithr = 0 ; ithr < NUM_OF_THREADS; ithr++){
        myThread[ithr] = thread(buildCostMap_helper,size * ithr/NUM_OF_THREADS,size * (ithr+1)/NUM_OF_THREADS
                ,ref(tempVector),ref(cost_map));
    }
    
    for(unsigned i = 0 ; i < NUM_OF_THREADS; i++)
        myThread[i].join();
    
    return ;
}

static void buildCostMap_helper(unsigned start,unsigned end,vector<unsigned>& all_targets,
        std::unordered_map<unsigned,std::unordered_map<unsigned,double>>& cost_map){
    
        for(unsigned i = start ; i < end; i++){
            //cout <<"------------" << i << "--------------"<<endl;
            singleSourceDijkstra(all_targets[i],all_targets,cost_map);
        }
}

//Use greedy method to generate the first path

static void generateFirstPath(KDTree::KDTree<2, coordinates>& pickUpTree, KDTree::KDTree<2, coordinates>& dropOffTree, KDTree::KDTree<2, coordinates>& depotTree,
        const std::vector<DeliveryInfo>& deliveries, const std::vector<unsigned>& depots, vector<unsigned> & path_of_inter, vector<unsigned> & final_path,
        std::unordered_map<unsigned,std::unordered_map<unsigned,double>>& cost_map) {
    
    double shortest_path_length = INF;
    double shortest_path_travel_time = INF;
    //start from different depot
    for (unsigned depotNum = 0; depotNum < depots.size(); depotNum++) {
        double tempLength = 0;
        vector<unsigned> temp_path;
        vector<unsigned> temp_final_path;
        //Pick this depot as the start point
        temp_path.push_back(depots[depotNum]);

        //create a temporary set to mark the availbability of drop off intersection
        vector <int> dropOffFlag;
        dropOffFlag.assign(pickUpTree.size(),0); // 0 means corresponding pickup is not reached yet

        auto pickUpTree_copy = pickUpTree;
        auto dropOffTree_copy = dropOffTree;
        auto depotTree_copy = depotTree;
        for (unsigned i = 0; i < pickUpTree.size() * 2; i++) { // total insertion time should be 2 time pick-up number (= drop-off number)
            auto cur_pos = streetsHelper->get_intersection_position(temp_path[i]);
            coordinates cur_coor(cur_pos.first, cur_pos.second, 0);

            int closest_pickup = -1;
            if (!pickUpTree_copy.empty())
                closest_pickup = pickUpTree_copy.find_nearest(cur_coor).first->ID;

            int closest_dropOff = -1;
            if (!dropOffTree_copy.empty())
                closest_dropOff = dropOffTree_copy.find_nearest(cur_coor).first->ID;

            if(closest_pickup == -1){
                //If no pick up intersection exists
                //add closest drop off position to the path
                double distanceToDropOff = get_manhattan_distance(temp_path[i], deliveries[closest_dropOff].dropOff);
                tempLength += distanceToDropOff;
                temp_path.push_back(deliveries[closest_dropOff].dropOff);
                auto dropOffDeleted = streetsHelper->get_intersection_position(deliveries[closest_dropOff].dropOff);
                dropOffTree_copy.erase_exact(coordinates(dropOffDeleted.first, dropOffDeleted.second, closest_dropOff));
            } else {
                double distanceToPickUp = get_manhattan_distance(temp_path[i], deliveries[closest_pickup].pickUp);
                double distanceToDropOff = get_manhattan_distance(temp_path[i], deliveries[closest_dropOff].dropOff);
                if (distanceToDropOff < distanceToPickUp && dropOffFlag[closest_dropOff] == 1) {
                    //Add this drop off to path , and delete this drop off in kdtree
                    tempLength += distanceToDropOff;
                    temp_path.push_back(deliveries[closest_dropOff].dropOff);
                    dropOffFlag[closest_dropOff] = 0;
                    auto dropOffDeleted = streetsHelper->get_intersection_position(deliveries[closest_dropOff].dropOff);
                    dropOffTree_copy.erase_exact(coordinates(dropOffDeleted.first, dropOffDeleted.second, closest_dropOff));
                } else {
                    tempLength += distanceToPickUp;
                    temp_path.push_back(deliveries[closest_pickup].pickUp);
                    dropOffFlag[closest_pickup] = 1;
                    auto pickupDeleted = streetsHelper->get_intersection_position(deliveries[closest_pickup].pickUp);
                    pickUpTree_copy.erase_exact(coordinates(pickupDeleted.first,pickupDeleted.second,closest_pickup));
                }
            }
        }
        
        //add the final depot intersection which is closest to the last drop off
        auto last_dropOff_pos = streetsHelper->get_intersection_position(temp_path[pickUpTree.size()*2]);
        coordinates last_drop_coor(last_dropOff_pos.first, last_dropOff_pos.second, 0);
        int finalDepot = depotTree_copy.find_nearest(last_drop_coor).first->ID;
        temp_path.push_back(depots[finalDepot]);
        
        update_depot_place(temp_path,depots,cost_map);
        
        double temp_time = calculate_actual_path_cost(cost_map,temp_path);
        if(temp_time < shortest_path_travel_time){
            shortest_path_travel_time = temp_time;
            path_of_inter = temp_path;
        }
            
//        //add the final depot intersection which is closest to the last drop off
//        auto last_dropOff_pos = streetsHelper->get_intersection_position(temp_path[pickUpTree.size()*2]);
//        coordinates last_drop_coor(last_dropOff_pos.first, last_dropOff_pos.second, 0);
//        int finalDepot = depotTree_copy.find_nearest(last_drop_coor).first->ID;
//        temp_path.push_back(depots[finalDepot]);
//
//        //hard coding here . Remember to change !!!!!!
//        vector<unsigned> check_legal = find_path_between_intersections(depots[finalDepot], temp_path[pickUpTree.size()*2]);
//        if (check_legal.empty()) {
//            while (1) {
//                auto final_depot_pos = streetsHelper->get_intersection_position(depots[finalDepot]);
//                depotTree_copy.erase_exact(coordinates(final_depot_pos.first, final_depot_pos.second, finalDepot));
//                finalDepot = depotTree_copy.find_nearest(last_drop_coor).first->ID;
////                check_legal = find_path_between_intersections(depots[finalDepot], temp_path[pickUpTree.size()*2]);
////                if (!check_legal.empty()) {
////                    temp_path.push_back(depots[finalDepot]);
////                    break;
////                }
//                if(cost_map[temp_path[pickUpTree.size()*2]][depots[finalDepot]] != INF){
//                    
//                    break;
//                }
//            }
//        } else {
//            temp_path.push_back(depots[finalDepot]);
//        }
//
////        if(check_legal.empty()){
////            temp_path.push_back(depots[1]);
////        }else{
////            temp_path.push_back(depots[finalDepot]);
////        }
//
//        //Add the last distance
//        tempLength += get_manhattan_distance(depots[finalDepot],temp_path[pickUpTree.size()*2]);
////        if(tempLength < shortest_path_length){
////            shortest_path_length = tempLength;
////            path_of_inter = temp_path;
////        }
//
//        bool validPath = true;
//
//        for (unsigned i = 0; i < temp_path.size() - 1; i++) {
//            vector<unsigned> oneSegment = find_path_between_intersections(temp_path[i], temp_path[i + 1]);
//            if (oneSegment.empty() && temp_path[i] != temp_path[i + 1]) {
//                validPath = false;
////              cout <<"invalid" <<endl;
//                break;
//            }else{
//                temp_final_path.insert(temp_final_path.end(),oneSegment.begin(),oneSegment.end());
//            }
//        }
//
//        if(validPath){
//            double tempTravelTime = compute_path_travel_time(temp_final_path);
//            if (tempTravelTime < shortest_path_travel_time) {
//                shortest_path_travel_time = tempTravelTime;
//                final_path = temp_final_path;
//                path_of_inter = temp_path;
//            }
//        }

    }
}

static bool isPathValid(vector<unsigned>& path, const vector<DeliveryInfo>& deliveries, const vector<unsigned>& depots) {
    //if either the start or end elements of <path> is not part of depots vector, legal=false
    if ((find(depots.begin(), depots.end(), path[0]) == depots.end()) || (find(depots.begin(), depots.end(), path[path.size() - 1]) == depots.end())) {
        return false;
    }

    //during each iteration, check if the ID of pick up intersection is before the ID of drop-off intersection in the path vector
    for (unsigned i = 0; i < deliveries.size(); i++) {
        unsigned pickup = deliveries[i].pickUp;
        unsigned dropoff = deliveries[i].dropOff;

        unsigned first_pickup_index = path.size();
        unsigned last_dropoff_index = -1;

        //get last_dropoff
        for (unsigned j = 0; j < path.size(); j++) {
            if (path[j] == dropoff){
                last_dropoff_index = j;
            }
        }

        //get first pickup
        vector <unsigned>::iterator first_pickup_iter = find(path.begin(), path.end(), pickup);
        if (first_pickup_iter != path.end())
            first_pickup_index = first_pickup_iter - path.begin();

        //if this condition is true, then the delivery was legal. I used last drop off and first pick up because
        //each intersection may be visited multiple times.
        if (last_dropoff_index < first_pickup_index){
            return false;
        }

    }
    return true;

}

static void generateRandomLegalPath(vector<unsigned>& path, const vector<DeliveryInfo>& deliveries, const vector<unsigned>& depots) {
    unsigned depots_size = depots.size();
    unsigned deliveries_size = deliveries.size();
    //first: deliveries index
    //second: true: pick_up
    //        false: drop_off
    vector < pair<unsigned, bool>>available_intersections;
    //vector < pair<unsigned, bool>>::iterator erase_it;
    //bool legal = false;
    unsigned random_repots_index;
    unsigned random_index;
    srand(time(NULL));
    //int count = 0;
    //push starting depot in
    random_repots_index = rand() % depots_size;
    path.push_back(depots[random_repots_index]);

    //store all the pick_ups in available intersections first
    for (unsigned i = 0; i < deliveries_size; i++) {
        pair<unsigned, bool> temp;
        temp.first = i;
        temp.second = true;

        available_intersections.push_back(temp);
    }

    while (available_intersections.size() != 0) {
        random_index = rand() % available_intersections.size();
        //it's a pick_up
        //put itself in path
        //put its dropoff in available_intersections
        //delete itself from available_intersections
        if (available_intersections[random_index].second) {
            pair<unsigned, bool> temp;
            temp.first = available_intersections[random_index].first;
            temp.second = false;
            available_intersections.push_back(temp);

            unsigned delivery_ID = available_intersections[random_index].first;
            path.push_back(deliveries[delivery_ID].pickUp);
            //delete it from available_intersections
            available_intersections.erase(available_intersections.begin() + random_index);
        }
        //it's a drop_off
            //put itself in path
            //delete itself from available_intersections
        else {
            unsigned delivery_ID = available_intersections[random_index].first;
            path.push_back(deliveries[delivery_ID].dropOff);
            //delete it from available_intersections
            available_intersections.erase(available_intersections.begin() + random_index);
        }
    }
    //put a depot at the end
    random_repots_index = rand() % depots_size;
    path.push_back(depots[random_repots_index]);

    
    
}


//get Manhattan distance between two intersections

static double get_manhattan_distance(unsigned intersection1, unsigned intersection2) {
    auto pos1 = streetsHelper->get_intersection_position(intersection1);

    auto pos2 = streetsHelper->get_intersection_position(intersection2);

    return abs(pos1.first - pos2.first) + abs(pos1.second - pos2.second);
}


static double calculate_path_geo_distance(const vector<unsigned> & path_of_inter){
    double distance = 0.0;
    for(unsigned i = 0 ; i < path_of_inter.size() - 1; i ++){
        distance += get_manhattan_distance(path_of_inter[i],path_of_inter[i+1]);
    }
    return distance;
}


static double calculate_actual_path_cost(std::unordered_map<unsigned,std::unordered_map<unsigned,double>>& cost_map,
    const vector<unsigned> & path_of_inter){
    double time = 0 ;
    for(unsigned i = 0; i < path_of_inter.size() - 1 ; i++){
        double temp_cost = cost_map[path_of_inter[i]][path_of_inter[i+1]];
        if(temp_cost == INF){
            time = INF;
            break;
        }else
            time += temp_cost;
    }
    
    return time;
}


//Update the closest first and last depots of given path of intersections
static void update_depot_place(vector<unsigned> & path_of_inter ,const std::vector<unsigned>& depots,
         std::unordered_map<unsigned,std::unordered_map<unsigned,double>>& cost_map){
//    auto depotTree_copy = depotTree;
//    auto first_pickup_pos = streetsHelper->get_intersection_position(path_of_inter[1]);
//    auto last_dropOff_pos = streetsHelper->get_intersection_position(path_of_inter[path_of_inter.size()-2]);
//    coordinates first_pickup_coor(first_pickup_pos.first, first_pickup_pos.second, 0);
//    coordinates last_drop_coor(last_dropOff_pos.first, last_dropOff_pos.second, 0);
//
//    int firstDepot = depotTree.find_nearest(first_pickup_coor).first->ID;
//    int finalDepot = depotTree.find_nearest(last_drop_coor).first->ID;
//        
//    path_of_inter[0] = depots[firstDepot];
//    path_of_inter[path_of_inter.size() - 1] = depots[finalDepot];
    
    unsigned closestFirstDepot = depots[0], closetLastDepot = depots[0];
    double toFirst = INF,toLast = INF;
    for(unsigned i = 0 ; i < depots.size(); i++){
        double tempFirst = cost_map[depots[i]][path_of_inter[1]];
        if(tempFirst < toFirst){
            toFirst = tempFirst;
            closestFirstDepot = depots[i];
        }
        double tempLast = cost_map[path_of_inter[path_of_inter.size()-2]][depots[i]];
        if(tempLast < toLast){
            toLast = tempLast; 
            closetLastDepot = depots[i];
        }
           
    }
    
    path_of_inter[0] = closestFirstDepot;
    path_of_inter[path_of_inter.size() - 1] = closetLastDepot;
    
}


static void randomSearch(vector<unsigned>& path_of_inter,const vector<DeliveryInfo>& deliveries,
        const vector<unsigned>& depots,KDTree::KDTree<2, coordinates>& depotTree,chrono::high_resolution_clock::time_point startTime,
        std::unordered_map<unsigned,std::unordered_map<unsigned,double>>& cost_map){
    chrono::duration<double> wallClock;

    //vector<unsigned> cur_path_of_inter = path_of_inter;
    //double shortest_distance = calculate_path_geo_distance(path_of_inter);
    //double shortest_travel_time = compute_path_travel_time(path);
    double shortest_travel_time = calculate_actual_path_cost(cost_map,path_of_inter);
    vector<unsigned> current_path_of_inter = path_of_inter;
    srand(time(NULL));
    auto updateTime = chrono::high_resolution_clock::now();
    bool stuck = false;
    bool startSA = false;
    do{

        for (unsigned i = 1; i < path_of_inter.size() - 1; i++) { // start with the second element,end with second last element
            if (i == (path_of_inter.size() - 1)) {
                cout << "at the end" << endl;
            }
            bool findShorter = false;
            bool timeOut = false;
            bool SAout = false;
            
            for (unsigned j = 1; j < i; j++) {
                auto currentTime = chrono::high_resolution_clock::now();
                wallClock =
                        chrono::duration_cast<chrono::duration<double>>(currentTime - startTime);
                if (wallClock.count() >= 0.9 * TIME_LIMIT) {
                    timeOut = true;
                    break;
                }

                vector<unsigned> temp_path_of_inter = current_path_of_inter;
                
                
                auto nowTime = chrono::high_resolution_clock::now();
                auto duration_update =
                        chrono::duration_cast<chrono::duration<double>> (nowTime - updateTime);

                if (duration_update.count() > 0.5 && stuck){
                    startSA = true;
                }
                
                if (duration_update.count() > 0.5 && !stuck){
                    updateTime = chrono::high_resolution_clock::now();
                    stuck  = true;
                }
                
                
                
                
                if (stuck ) 
                {
                    if(startSA){
                    //swap two elements
                    unsigned temp = temp_path_of_inter[i];
                    temp_path_of_inter[i] = temp_path_of_inter[j];
                    temp_path_of_inter[j] = temp;                    
                    } else
                        twoOptSwap(temp_path_of_inter,j,i);
//                    if (i != path_of_inter.size() - 2)
//                        randomTwoOpt(temp_path_of_inter,j,i);

                } 
//                else 
//                    if( j + 4 == i){
//                        twoOptSwap(temp_path_of_inter,j,i);
//                    }
                    else
                    if (j + 3 == i ){
                    twoOptSwap(temp_path_of_inter,j,i);
//                    threeOptSwap(temp_path_of_inter,j,i);
                } else {
                    //twoOptSwap(temp_path_of_inter, j, i);
                     if (i != path_of_inter.size() - 2)
                        randomTwoOpt(temp_path_of_inter,j,i);
                }

                
                

//                if (i == 1 || j == 1 || i == path_of_inter.size() - 2 || j == path_of_inter.size() - 2)
                    update_depot_place(temp_path_of_inter, depots, cost_map);

                if (isPathValid(temp_path_of_inter, deliveries, depots)) {
                    //double temp_distance = calculate_path_geo_distance(temp_path_of_inter);
                    double temp_time = calculate_actual_path_cost(cost_map, temp_path_of_inter);
                    //vector<unsigned> temp_final_path;
//                    cout << "cost now : " << temp_time << endl;

                    if (temp_time < shortest_travel_time) {
                        cout << shortest_travel_time << "Find a shorter !!  " << temp_time << " " << endl;
                        shortest_travel_time = temp_time;
                        path_of_inter = temp_path_of_inter;
                        current_path_of_inter = temp_path_of_inter;
                        findShorter = true;
                        updateTime = chrono::high_resolution_clock::now();
                        break;
                    } 
//                    else {
//                        auto nowTime = chrono::high_resolution_clock::now();
//                        auto duration_update =
//                                chrono::duration_cast<chrono::duration<double>> (nowTime - updateTime);
//                        //                cout <<duration_update.count() <<endl;
//                        if (duration_update.count() > 0.7) {
//                            if (SA(temp_time - shortest_travel_time, wallClock.count())) {
//                                current_path_of_inter = temp_path_of_inter;
////                                cout << "accept " << endl;
////                                updateTime = chrono::high_resolution_clock::now();
//                                startSA = true;
//                            }
//                        }
//
//                    }
                  
                }
                
                if (findShorter || timeOut || SAout) break;
            }
        }
        


    }while(wallClock.count() < 0.9 * TIME_LIMIT);


}


//Given a vector of intersections,convert it to a path of segments
//If two adjacent intersection don't have a legal path,return false
static bool convert_inter_to_segments_path(vector<unsigned>& path_of_inter,vector<unsigned>& path) {
    path.clear();
    bool validPath = true;
    for (unsigned i = 0; i < path_of_inter.size() - 1; i++) {
        vector<unsigned> oneSegment = find_path_between_intersections(path_of_inter[i], path_of_inter[i + 1]);
        if (oneSegment.empty() && path_of_inter[i] != path_of_inter[i + 1]) {
            validPath = false;
            cout <<"invalid" <<endl;
            break;
        } else {
            path.insert(path.end(), oneSegment.begin(), oneSegment.end());
        }
    }

    return validPath;
}

//Simulated annealing , return true if accept move
static bool SA(double difference,double cur_time){
    const double T0 = 10000;
//    double currentT = T0 /  (cur_time * cur_time);
    const double coolRate = 0.85;
    double currentT = T0 * pow(coolRate,cur_time);
    double p = exp(- difference* difference / currentT);
    
    //srand(time(NULL));
    int randomNum = rand()%100 + 10;
//    cout <<randomNum << "  difference " << difference <<" T " <<currentT << "current time " << cur_time << " possiblities : " << p <<endl;
    return (randomNum < (p * 100));
}

static void twoOptSwap(vector<unsigned>& path_of_inter,unsigned i, unsigned k){
    vector<unsigned> firstPart, secondPart, lastPart;
    for (unsigned index = 1; index < i; index++)
        firstPart.push_back(path_of_inter[index]);

    for (unsigned index = i; index <= k; index++)
        secondPart.push_back(path_of_inter[index]);

    reverse(secondPart.begin(),secondPart.end());

    for (unsigned index = k + 1; index < path_of_inter.size() - 1; index++)
        lastPart.push_back(path_of_inter[index]);

    unsigned firstDepot = path_of_inter[0];
    unsigned lastDepot = path_of_inter[path_of_inter.size() -1];

    path_of_inter.clear();
    path_of_inter.push_back(firstDepot);
    path_of_inter.insert(path_of_inter.end(),firstPart.begin(),firstPart.end());
    path_of_inter.insert(path_of_inter.end(),secondPart.begin(),secondPart.end());
    path_of_inter.insert(path_of_inter.end(),lastPart.begin(),lastPart.end());
    path_of_inter.push_back(lastDepot);

}

static void threeOptSwap(vector<unsigned>& path_of_inter,unsigned i , unsigned k){
    unsigned temp = path_of_inter[i+2];
    path_of_inter[i + 2] = path_of_inter [i+3];
    path_of_inter[i + 3] = temp;
    return;
}

static void randomTwoOpt(vector<unsigned>& path_of_inter,unsigned i , unsigned k){
    unsigned temp1 = path_of_inter[i + 1];
    unsigned temp2 = path_of_inter[k + 1];
    
    path_of_inter[i + 1] = temp2;
    path_of_inter[k + 1] = temp1;
    

}
static void randomThreeOpt(vector<unsigned>& path_of_inter,unsigned i , unsigned j, unsigned k){
    unsigned temp1 = path_of_inter[i + 1];
    unsigned temp2 = path_of_inter[j + 1];
    unsigned temp3 = path_of_inter[k + 1];
    
    
    path_of_inter[i + 1] = temp3;
    path_of_inter[j + 1] = temp1;
    path_of_inter[k + 1] = temp2;
}

static void randomTwoOptAndCheck(vector<unsigned>& path_of_inter, unsigned i , unsigned k , double& time,
         const vector<DeliveryInfo>& deliveries, const vector<unsigned>& depots,std::unordered_map<unsigned,std::unordered_map<unsigned,double>>& cost_map){
    
    if( i != path_of_inter.size() -2 ) {       
        unsigned temp1 = path_of_inter[i + 1];
        unsigned temp2 = path_of_inter[k + 1];

        path_of_inter[i + 1] = temp2;
        path_of_inter[k + 1] = temp1;

        update_depot_place(path_of_inter, depots, cost_map);

        if (isPathValid(path_of_inter, deliveries, depots)) {
            //double temp_distance = calculate_path_geo_distance(temp_path_of_inter);
            time = calculate_actual_path_cost(cost_map, path_of_inter);
        } else {
            time = -1;
        }
    }else {
        time = -1;
    }
    return ;
}

static void twoOptSwapAndCheck(vector<unsigned>& path_of_inter,unsigned i , unsigned k , double& time,
        const vector<DeliveryInfo>& deliveries, const vector<unsigned>& depots,std::unordered_map<unsigned,std::unordered_map<unsigned,double>>& cost_map){
    vector<unsigned> firstPart, secondPart, lastPart;
    for (unsigned index = 1; index < i; index++)
        firstPart.push_back(path_of_inter[index]);

    for (unsigned index = i; index <= k; index++)
        secondPart.push_back(path_of_inter[index]);

    reverse(secondPart.begin(),secondPart.end());

    for (unsigned index = k + 1; index < path_of_inter.size() - 1; index++)
        lastPart.push_back(path_of_inter[index]);

    unsigned firstDepot = path_of_inter[0];
    unsigned lastDepot = path_of_inter[path_of_inter.size() -1];

    path_of_inter.clear();
    path_of_inter.push_back(firstDepot);
    path_of_inter.insert(path_of_inter.end(),firstPart.begin(),firstPart.end());
    path_of_inter.insert(path_of_inter.end(),secondPart.begin(),secondPart.end());
    path_of_inter.insert(path_of_inter.end(),lastPart.begin(),lastPart.end());
    path_of_inter.push_back(lastDepot);
    
    update_depot_place(path_of_inter, depots, cost_map);
    
    if (isPathValid(path_of_inter, deliveries, depots)) {
        //double temp_distance = calculate_path_geo_distance(temp_path_of_inter);
        time = calculate_actual_path_cost(cost_map, path_of_inter);
    }else {
        time = -1;
    }

}