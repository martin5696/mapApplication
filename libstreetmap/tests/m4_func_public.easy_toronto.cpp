//#include <random>
//#include <iostream>
//#include <unittest++/UnitTest++.h>
//
//#include "StreetsDatabaseAPI.h"
//#include "m1.h"
//#include "m3.h"
//#include "m4.h"
//
//#include "unit_test_util.h"
//#include "courier_verify.h"
//
//using ece297test::relative_error;
//using ece297test::courier_path_is_legal;
//
//
//SUITE(easy_toronto_public) {
//    TEST(easy_toronto) {
//        std::vector<DeliveryInfo> deliveries;
//        std::vector<unsigned> depots;
//        std::vector<unsigned> result_path;
//        bool is_legal;
//        
//        deliveries = {DeliveryInfo(105225, 56051), DeliveryInfo(104672, 66348), DeliveryInfo(98141, 84950), DeliveryInfo(97060, 67133), DeliveryInfo(88129, 84132), DeliveryInfo(64436, 20885), DeliveryInfo(28619, 64489), DeliveryInfo(79055, 10192), DeliveryInfo(56060, 24341), DeliveryInfo(46379, 91978), DeliveryInfo(80938, 59249), DeliveryInfo(105371, 65267), DeliveryInfo(97919, 93400), DeliveryInfo(61004, 32007), DeliveryInfo(71085, 9140), DeliveryInfo(43327, 19741), DeliveryInfo(63381, 43046), DeliveryInfo(65463, 45138), DeliveryInfo(20649, 47055), DeliveryInfo(32342, 85976), DeliveryInfo(17930, 33578), DeliveryInfo(55705, 98719), DeliveryInfo(108325, 101919), DeliveryInfo(95430, 53600), DeliveryInfo(9748, 7801)};
//        depots = {2, 9256};
//        {
//        	ECE297_TIME_CONSTRAINT(30000);
//        	
//        	result_path = traveling_courier(deliveries, depots);
//        }
//        
//        is_legal = courier_path_is_legal(deliveries, depots, result_path);
//        CHECK(is_legal);
//        
//        if(is_legal) {
//        	double path_cost = compute_path_travel_time(result_path);
//        	std::cout << "QoR easy_toronto: " << path_cost << std::endl;
//        } else {
//        	std::cout << "QoR easy_toronto: INVALID" << std::endl;
//        }
//        
//    } //easy_toronto
//
//} //easy_toronto_public
//
