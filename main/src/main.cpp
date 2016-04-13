#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "m4.h"
#include <vector>
#include <fstream>
#include <string>
#include <iostream>

#include "courier_verify.h"
#include "path_verify.h"
//#include "unit_test_util.h"
//#include <unittest++/UnitTest++.h>

//using ece297test::relative_error;
using ece297test::courier_path_is_legal;

using namespace std;
extern StreetsGraph* streetsHelper;

int main() {
//    string map_name = "/cad2/ece297s/public/maps/toronto.streets.bin";
//    load_map(map_name);
//
//
//    std::vector<DeliveryInfo> deliveries;
//    std::vector<unsigned> depots;
//    std::vector<unsigned> result_path;
//    std::vector<double> all_path_cost;
//    double path_cost_sum=1;
//    //bool is_legal;
//
//    deliveries = {DeliveryInfo(108031, 6145), DeliveryInfo(60559, 68385), DeliveryInfo(9586, 44723), DeliveryInfo(20806, 4738), DeliveryInfo(62657, 103362), DeliveryInfo(86324, 95265), DeliveryInfo(74264, 39741), DeliveryInfo(99932, 54939), DeliveryInfo(39444, 45923), DeliveryInfo(54844, 51018), DeliveryInfo(989, 83418), DeliveryInfo(36870, 94398), DeliveryInfo(20433, 26525), DeliveryInfo(32640, 88341), DeliveryInfo(53709, 47068), DeliveryInfo(96318, 20466), DeliveryInfo(103884, 68300), DeliveryInfo(71511, 57406), DeliveryInfo(61012, 36713), DeliveryInfo(9727, 94683), DeliveryInfo(76221, 22727), DeliveryInfo(24362, 5404), DeliveryInfo(83735, 21261), DeliveryInfo(26541, 13982), DeliveryInfo(2719, 7906), DeliveryInfo(60037, 44689), DeliveryInfo(98765, 82273), DeliveryInfo(44690, 93630), DeliveryInfo(51692, 46690), DeliveryInfo(83186, 66262), DeliveryInfo(64614, 108770), DeliveryInfo(74490, 89383), DeliveryInfo(79849, 2266), DeliveryInfo(24366, 50531), DeliveryInfo(60773, 74323), DeliveryInfo(79978, 74333), DeliveryInfo(87051, 55738), DeliveryInfo(58034, 16936), DeliveryInfo(80415, 69739), DeliveryInfo(107067, 41567), DeliveryInfo(98564, 99530), DeliveryInfo(98805, 30252), DeliveryInfo(86395, 105662), DeliveryInfo(70831, 36280), DeliveryInfo(27827, 97964), DeliveryInfo(16454, 2725), DeliveryInfo(57705, 39219), DeliveryInfo(7572, 13838), DeliveryInfo(108448, 56452), DeliveryInfo(37191, 98608), DeliveryInfo(57286, 77791), DeliveryInfo(49350, 41462), DeliveryInfo(87576, 21235), DeliveryInfo(92611, 9007), DeliveryInfo(16103, 53986), DeliveryInfo(81784, 11113), DeliveryInfo(68915, 44826), DeliveryInfo(41357, 36493), DeliveryInfo(28917, 85643), DeliveryInfo(102853, 75119), DeliveryInfo(103834, 107566), DeliveryInfo(69352, 91850), DeliveryInfo(86580, 96376), DeliveryInfo(93336, 96967), DeliveryInfo(4863, 6566), DeliveryInfo(14679, 32310), DeliveryInfo(75459, 51728), DeliveryInfo(48014, 77817), DeliveryInfo(96722, 23835), DeliveryInfo(74175, 106669), DeliveryInfo(67807, 74292), DeliveryInfo(67216, 47770), DeliveryInfo(53109, 61805), DeliveryInfo(41171, 99880), DeliveryInfo(46806, 4562), DeliveryInfo(43567, 67285), DeliveryInfo(43215, 77756), DeliveryInfo(51610, 23822), DeliveryInfo(44546, 18710), DeliveryInfo(38892, 39136), DeliveryInfo(50331, 22340), DeliveryInfo(15804, 49666), DeliveryInfo(7650, 47704), DeliveryInfo(68616, 75607), DeliveryInfo(85195, 47378), DeliveryInfo(3909, 33352), DeliveryInfo(28282, 55406), DeliveryInfo(84125, 97481), DeliveryInfo(61155, 75263), DeliveryInfo(47448, 98287), DeliveryInfo(58489, 78564), DeliveryInfo(105649, 97894), DeliveryInfo(107584, 107059), DeliveryInfo(51371, 27010), DeliveryInfo(60356, 25809), DeliveryInfo(40558, 82828), DeliveryInfo(49782, 18735), DeliveryInfo(81702, 1224), DeliveryInfo(65021, 76398), DeliveryInfo(5132, 84210)};
//    depots = {14, 55539, 66199, 38064, 87930, 15037, 9781, 42243, 62859, 50192};
//
//    {
//        //ECE297_TIME_CONSTRAINT(30000);
//
//        result_path = traveling_courier(deliveries, depots);
//    }
//
//    double path_cost = compute_path_travel_time(result_path);
//    std::cout << "QoR hard_toronto: " << path_cost << std::endl;
//    all_path_cost.push_back(path_cost);
//    
//    
//    //-------------------------------------------------------------------------------------------------------
//    
//    
//    deliveries = {DeliveryInfo(38727, 59698), DeliveryInfo(65778, 85395), DeliveryInfo(72606, 62090), DeliveryInfo(7827, 86986), DeliveryInfo(39431, 91987), DeliveryInfo(15613, 84303), DeliveryInfo(37441, 72613), DeliveryInfo(10284, 60470), DeliveryInfo(16654, 92145), DeliveryInfo(60276, 3579), DeliveryInfo(50829, 60719), DeliveryInfo(85570, 845), DeliveryInfo(6333, 22876), DeliveryInfo(21428, 79783), DeliveryInfo(16472, 42652), DeliveryInfo(98146, 17150), DeliveryInfo(41982, 8788), DeliveryInfo(68237, 58412), DeliveryInfo(29599, 48254), DeliveryInfo(19070, 28138), DeliveryInfo(5781, 41378), DeliveryInfo(105114, 45952), DeliveryInfo(35687, 25254), DeliveryInfo(85570, 7169), DeliveryInfo(98247, 87639), DeliveryInfo(77271, 52297), DeliveryInfo(34495, 71063), DeliveryInfo(102996, 92999), DeliveryInfo(100448, 85395), DeliveryInfo(14852, 10598), DeliveryInfo(643, 74177), DeliveryInfo(58079, 38910), DeliveryInfo(84320, 96524), DeliveryInfo(37873, 73515), DeliveryInfo(85570, 61062), DeliveryInfo(35687, 37695), DeliveryInfo(83605, 92332), DeliveryInfo(48625, 18518), DeliveryInfo(44624, 70990), DeliveryInfo(85570, 96451), DeliveryInfo(17177, 32292), DeliveryInfo(5877, 32281), DeliveryInfo(28423, 102803), DeliveryInfo(97252, 7666), DeliveryInfo(57531, 15850), DeliveryInfo(86538, 45936), DeliveryInfo(54607, 82043), DeliveryInfo(79302, 23466), DeliveryInfo(17330, 13563), DeliveryInfo(20735, 77097), DeliveryInfo(71347, 57609), DeliveryInfo(62158, 58144), DeliveryInfo(99551, 101225), DeliveryInfo(64507, 25401), DeliveryInfo(21654, 70065), DeliveryInfo(30106, 26063), DeliveryInfo(50355, 27368), DeliveryInfo(101852, 92510), DeliveryInfo(71181, 31512), DeliveryInfo(26265, 80945), DeliveryInfo(37340, 81774), DeliveryInfo(96847, 15852), DeliveryInfo(61533, 29574), DeliveryInfo(6131, 70742), DeliveryInfo(9087, 61034), DeliveryInfo(77134, 15234), DeliveryInfo(63984, 85395), DeliveryInfo(8230, 42), DeliveryInfo(106939, 42206), DeliveryInfo(22271, 95320), DeliveryInfo(24130, 24326), DeliveryInfo(32474, 42831), DeliveryInfo(2810, 94984), DeliveryInfo(29327, 46365), DeliveryInfo(79547, 85395), DeliveryInfo(91479, 7528), DeliveryInfo(28999, 97021), DeliveryInfo(85466, 482), DeliveryInfo(90236, 32465), DeliveryInfo(8677, 50387), DeliveryInfo(35687, 35819), DeliveryInfo(73100, 67706), DeliveryInfo(15986, 48494), DeliveryInfo(89699, 94252), DeliveryInfo(4289, 95320), DeliveryInfo(16052, 33731), DeliveryInfo(36581, 66778), DeliveryInfo(33827, 28353), DeliveryInfo(63355, 5926), DeliveryInfo(98196, 60346), DeliveryInfo(10063, 67706), DeliveryInfo(35687, 73875), DeliveryInfo(78247, 52844), DeliveryInfo(88166, 95320), DeliveryInfo(12403, 103519), DeliveryInfo(83417, 95584), DeliveryInfo(35687, 20788), DeliveryInfo(96220, 5528), DeliveryInfo(57602, 100089), DeliveryInfo(32275, 90786)};
//        depots = {17, 64796, 22802};
//
//    {
//        //ECE297_TIME_CONSTRAINT(30000);
//
//        result_path = traveling_courier(deliveries, depots);
//    }
//
//    path_cost = compute_path_travel_time(result_path);
//    std::cout << "QoR hard_multi_toronto: " << path_cost << std::endl;
//    all_path_cost.push_back(path_cost);
//    
//    
//    //-------------------------------------------------------------------------------------------------------
//    
//    
//   deliveries = {DeliveryInfo(97737, 22123), DeliveryInfo(86706, 107106), DeliveryInfo(70446, 89550), DeliveryInfo(92035, 91504), DeliveryInfo(91406, 12979), DeliveryInfo(100483, 100489), DeliveryInfo(65227, 44951), DeliveryInfo(71300, 24499), DeliveryInfo(39569, 26541), DeliveryInfo(10943, 8868), DeliveryInfo(86202, 49245), DeliveryInfo(95476, 6933), DeliveryInfo(87487, 5098), DeliveryInfo(19200, 37655), DeliveryInfo(69586, 11855), DeliveryInfo(38195, 9266), DeliveryInfo(76897, 55076), DeliveryInfo(1577, 23420), DeliveryInfo(101760, 36138), DeliveryInfo(18992, 1358), DeliveryInfo(20737, 4199), DeliveryInfo(3825, 19895), DeliveryInfo(108313, 38772), DeliveryInfo(52866, 99918), DeliveryInfo(58367, 38278), DeliveryInfo(55815, 62594), DeliveryInfo(10903, 8852), DeliveryInfo(101687, 63100), DeliveryInfo(101653, 1995), DeliveryInfo(32635, 84959), DeliveryInfo(4532, 60665), DeliveryInfo(7451, 6732), DeliveryInfo(66721, 51546), DeliveryInfo(100800, 85757), DeliveryInfo(55413, 42396), DeliveryInfo(57606, 63772), DeliveryInfo(67512, 19810), DeliveryInfo(101289, 67660), DeliveryInfo(84756, 90646), DeliveryInfo(57800, 49935), DeliveryInfo(81126, 41379), DeliveryInfo(83885, 4922), DeliveryInfo(44236, 9832), DeliveryInfo(80177, 80066), DeliveryInfo(100835, 33285), DeliveryInfo(53191, 28183), DeliveryInfo(79300, 107511), DeliveryInfo(24536, 21543), DeliveryInfo(27124, 82725), DeliveryInfo(43658, 54195), DeliveryInfo(69278, 100017), DeliveryInfo(97039, 12154), DeliveryInfo(107250, 18010), DeliveryInfo(42285, 75488), DeliveryInfo(104906, 45561), DeliveryInfo(35117, 46371), DeliveryInfo(18176, 7464), DeliveryInfo(66985, 337), DeliveryInfo(78011, 84803), DeliveryInfo(39454, 9971), DeliveryInfo(38327, 100624), DeliveryInfo(30957, 95459), DeliveryInfo(86153, 59390), DeliveryInfo(99997, 52329), DeliveryInfo(78446, 43073), DeliveryInfo(108369, 30452), DeliveryInfo(82971, 15070), DeliveryInfo(86016, 66831), DeliveryInfo(16338, 69732), DeliveryInfo(24181, 7956), DeliveryInfo(53851, 38252), DeliveryInfo(70416, 82078), DeliveryInfo(38198, 73571), DeliveryInfo(51298, 47356), DeliveryInfo(97739, 59795), DeliveryInfo(10324, 81125), DeliveryInfo(78074, 35161), DeliveryInfo(43330, 3965), DeliveryInfo(63125, 37747), DeliveryInfo(102356, 43166), DeliveryInfo(80679, 81011), DeliveryInfo(15942, 108701), DeliveryInfo(13046, 70271), DeliveryInfo(101925, 43762), DeliveryInfo(45017, 85103), DeliveryInfo(98913, 97060), DeliveryInfo(72613, 74346), DeliveryInfo(4542, 16876), DeliveryInfo(54400, 97553), DeliveryInfo(42794, 30678), DeliveryInfo(35600, 7936), DeliveryInfo(47933, 17911), DeliveryInfo(89038, 90700), DeliveryInfo(37935, 48448), DeliveryInfo(92897, 50555), DeliveryInfo(9676, 20734), DeliveryInfo(66236, 98229), DeliveryInfo(55586, 96313), DeliveryInfo(25699, 102948), DeliveryInfo(79089, 72423), DeliveryInfo(12457, 71101), DeliveryInfo(106629, 19927), DeliveryInfo(105215, 28113), DeliveryInfo(104429, 90122), DeliveryInfo(12658, 6342), DeliveryInfo(104063, 104785), DeliveryInfo(102743, 79598), DeliveryInfo(31348, 95638), DeliveryInfo(5115, 77756), DeliveryInfo(88550, 85554), DeliveryInfo(16212, 39365), DeliveryInfo(39208, 40477), DeliveryInfo(76351, 78144), DeliveryInfo(60724, 107865), DeliveryInfo(105705, 39838), DeliveryInfo(13641, 75528), DeliveryInfo(35440, 788), DeliveryInfo(52343, 57495), DeliveryInfo(45478, 38816), DeliveryInfo(78964, 24686), DeliveryInfo(57079, 107516), DeliveryInfo(51767, 73285), DeliveryInfo(9206, 52303), DeliveryInfo(96723, 22897), DeliveryInfo(91238, 51539), DeliveryInfo(71680, 18540), DeliveryInfo(23303, 64794), DeliveryInfo(7898, 11483), DeliveryInfo(4974, 41556), DeliveryInfo(6728, 59088), DeliveryInfo(15029, 82548), DeliveryInfo(20835, 101981), DeliveryInfo(37879, 101434), DeliveryInfo(107705, 102304), DeliveryInfo(7921, 62502), DeliveryInfo(100594, 43313), DeliveryInfo(27311, 34325), DeliveryInfo(15012, 65199), DeliveryInfo(48195, 53593), DeliveryInfo(61188, 72699), DeliveryInfo(15383, 29521), DeliveryInfo(98849, 17404), DeliveryInfo(28358, 64354), DeliveryInfo(77817, 4401), DeliveryInfo(87533, 37041), DeliveryInfo(91044, 86037), DeliveryInfo(59888, 18522), DeliveryInfo(70166, 82462), DeliveryInfo(37234, 102756), DeliveryInfo(2089, 39725), DeliveryInfo(89895, 7133), DeliveryInfo(57694, 2500), DeliveryInfo(53439, 49695), DeliveryInfo(69104, 91785), DeliveryInfo(63733, 46490), DeliveryInfo(76725, 87583), DeliveryInfo(40604, 67188), DeliveryInfo(106684, 56115), DeliveryInfo(15289, 82773), DeliveryInfo(56889, 105054), DeliveryInfo(85751, 46224), DeliveryInfo(14766, 74171), DeliveryInfo(35435, 25214), DeliveryInfo(10241, 20733), DeliveryInfo(46379, 78497), DeliveryInfo(103884, 48768), DeliveryInfo(71193, 97900), DeliveryInfo(73387, 36400), DeliveryInfo(48908, 43693), DeliveryInfo(3417, 91647), DeliveryInfo(1670, 54596), DeliveryInfo(24222, 105503), DeliveryInfo(74710, 17038), DeliveryInfo(56057, 67471), DeliveryInfo(100567, 60848)};
//        depots = {24, 92566, 1472, 99728, 73976, 97636, 16302, 106692, 68479, 47367, 97481, 65740, 89515, 10414, 100062, 12750, 83423, 103509, 28534, 72513};
//
//    {
//        //ECE297_TIME_CONSTRAINT(30000);
//
//        result_path = traveling_courier(deliveries, depots);
//    }
//
//    path_cost = compute_path_travel_time(result_path);
//    std::cout << "QoR extreme_toronto: " << path_cost << std::endl;
//    all_path_cost.push_back(path_cost);
//
//    
//    
//    //-------------------------------------------------------------------------------------------------------
//    deliveries = {DeliveryInfo(35616, 105526), DeliveryInfo(63629, 29982), DeliveryInfo(57895, 42832), DeliveryInfo(25504, 42414), DeliveryInfo(17388, 50521), DeliveryInfo(72263, 8484), DeliveryInfo(13114, 11870), DeliveryInfo(62797, 25487), DeliveryInfo(47665, 3132), DeliveryInfo(19446, 10643), DeliveryInfo(47665, 20216), DeliveryInfo(19919, 88504), DeliveryInfo(17884, 102336), DeliveryInfo(17320, 69630), DeliveryInfo(25504, 101648), DeliveryInfo(40957, 101012), DeliveryInfo(68878, 60332), DeliveryInfo(79874, 8198), DeliveryInfo(49579, 39492), DeliveryInfo(56988, 52677), DeliveryInfo(20388, 75461), DeliveryInfo(47665, 89283), DeliveryInfo(64901, 74546), DeliveryInfo(79278, 11982), DeliveryInfo(61080, 106565), DeliveryInfo(19919, 74718), DeliveryInfo(74498, 58420), DeliveryInfo(67830, 83936), DeliveryInfo(62797, 37040), DeliveryInfo(40338, 531), DeliveryInfo(69435, 52369), DeliveryInfo(50594, 43698), DeliveryInfo(53778, 72780), DeliveryInfo(72263, 106061), DeliveryInfo(40928, 74718), DeliveryInfo(48762, 77948), DeliveryInfo(2996, 66404), DeliveryInfo(85310, 72860), DeliveryInfo(28858, 32671), DeliveryInfo(5410, 34612), DeliveryInfo(61145, 20216), DeliveryInfo(62797, 1609), DeliveryInfo(76206, 66404), DeliveryInfo(87649, 11982), DeliveryInfo(68455, 29016), DeliveryInfo(89198, 52696), DeliveryInfo(11266, 54359), DeliveryInfo(68878, 34825), DeliveryInfo(19919, 101012), DeliveryInfo(17884, 60387), DeliveryInfo(107445, 104557), DeliveryInfo(72263, 37252), DeliveryInfo(31169, 9991), DeliveryInfo(94872, 60332), DeliveryInfo(16513, 11982), DeliveryInfo(67051, 17253), DeliveryInfo(6012, 51171), DeliveryInfo(9356, 18856), DeliveryInfo(66467, 88504), DeliveryInfo(34656, 76700), DeliveryInfo(81848, 101012), DeliveryInfo(68455, 89283), DeliveryInfo(15410, 77948), DeliveryInfo(99081, 1736), DeliveryInfo(80064, 46224), DeliveryInfo(19919, 39183), DeliveryInfo(44665, 8259), DeliveryInfo(40188, 20216), DeliveryInfo(12281, 14841), DeliveryInfo(10831, 61773), DeliveryInfo(30922, 89283), DeliveryInfo(39195, 45037), DeliveryInfo(74253, 81289), DeliveryInfo(31861, 32431), DeliveryInfo(43454, 46741), DeliveryInfo(105212, 25539), DeliveryInfo(29004, 31106), DeliveryInfo(67490, 97034), DeliveryInfo(41159, 7093), DeliveryInfo(68455, 45222), DeliveryInfo(48371, 32671), DeliveryInfo(62797, 108675), DeliveryInfo(17884, 14841), DeliveryInfo(68878, 8259), DeliveryInfo(96097, 18291), DeliveryInfo(17884, 17585), DeliveryInfo(19919, 44870), DeliveryInfo(74738, 50575), DeliveryInfo(68878, 72860), DeliveryInfo(68878, 37040), DeliveryInfo(90057, 77948), DeliveryInfo(19919, 101648), DeliveryInfo(2817, 102593), DeliveryInfo(44652, 21453), DeliveryInfo(68455, 60332), DeliveryInfo(19446, 53260), DeliveryInfo(19446, 30587), DeliveryInfo(36865, 46534), DeliveryInfo(28051, 32740), DeliveryInfo(56224, 36759), DeliveryInfo(101697, 96491), DeliveryInfo(82134, 34928), DeliveryInfo(102974, 36049), DeliveryInfo(14416, 60833), DeliveryInfo(62797, 54746), DeliveryInfo(63578, 88504), DeliveryInfo(107653, 20139), DeliveryInfo(106921, 40336), DeliveryInfo(61080, 28262), DeliveryInfo(53317, 10101), DeliveryInfo(64776, 52696), DeliveryInfo(87070, 32431), DeliveryInfo(47665, 89283), DeliveryInfo(41159, 19997), DeliveryInfo(106170, 16494), DeliveryInfo(9425, 66296), DeliveryInfo(85812, 101883), DeliveryInfo(87998, 84048), DeliveryInfo(74855, 102068), DeliveryInfo(74738, 77819), DeliveryInfo(59220, 101648), DeliveryInfo(41159, 32671), DeliveryInfo(93408, 17081), DeliveryInfo(32026, 2717), DeliveryInfo(104300, 25539), DeliveryInfo(84916, 67679), DeliveryInfo(102974, 66404), DeliveryInfo(108618, 32671), DeliveryInfo(49519, 11982), DeliveryInfo(17884, 77819), DeliveryInfo(78260, 16446), DeliveryInfo(19446, 65428), DeliveryInfo(72263, 57866), DeliveryInfo(61080, 34484), DeliveryInfo(8687, 101648), DeliveryInfo(68878, 20216), DeliveryInfo(45369, 5383), DeliveryInfo(108291, 82221), DeliveryInfo(55468, 101883), DeliveryInfo(72263, 74273), DeliveryInfo(25504, 91500), DeliveryInfo(19919, 90625), DeliveryInfo(25531, 51171), DeliveryInfo(17884, 36829), DeliveryInfo(102974, 43698), DeliveryInfo(8064, 66404), DeliveryInfo(62797, 14841), DeliveryInfo(47897, 54888), DeliveryInfo(70787, 66476), DeliveryInfo(47665, 102336), DeliveryInfo(33914, 36208), DeliveryInfo(25504, 107894), DeliveryInfo(25504, 51171), DeliveryInfo(68557, 13978), DeliveryInfo(37843, 47616), DeliveryInfo(25504, 1736), DeliveryInfo(25504, 91027), DeliveryInfo(91568, 53583), DeliveryInfo(93964, 22241), DeliveryInfo(41159, 68154), DeliveryInfo(62797, 2483), DeliveryInfo(102974, 85994), DeliveryInfo(74738, 60584), DeliveryInfo(68878, 94508), DeliveryInfo(40736, 21791), DeliveryInfo(103090, 1973), DeliveryInfo(98543, 56693), DeliveryInfo(31308, 78357), DeliveryInfo(102974, 17289), DeliveryInfo(21585, 90482), DeliveryInfo(19446, 68751), DeliveryInfo(61080, 1736), DeliveryInfo(12023, 37499), DeliveryInfo(62797, 32431), DeliveryInfo(85817, 83833), DeliveryInfo(59415, 88123), DeliveryInfo(8148, 37177), DeliveryInfo(47665, 72860), DeliveryInfo(67336, 51171), DeliveryInfo(72283, 47506), DeliveryInfo(98543, 20394), DeliveryInfo(67307, 38349), DeliveryInfo(27381, 74718), DeliveryInfo(19919, 95951), DeliveryInfo(47665, 31509), DeliveryInfo(89476, 88123), DeliveryInfo(25504, 37040), DeliveryInfo(74387, 83936), DeliveryInfo(25504, 77819), DeliveryInfo(62797, 66404), DeliveryInfo(76673, 101012), DeliveryInfo(80359, 10208), DeliveryInfo(10005, 47164), DeliveryInfo(47624, 44384), DeliveryInfo(3258, 3415), DeliveryInfo(74431, 65055), DeliveryInfo(19317, 9384), DeliveryInfo(68878, 72860), DeliveryInfo(98543, 52696), DeliveryInfo(4119, 104996), DeliveryInfo(19446, 39492), DeliveryInfo(17884, 83625), DeliveryInfo(106396, 11164), DeliveryInfo(66296, 14841), DeliveryInfo(41159, 4726), DeliveryInfo(47757, 92667), DeliveryInfo(82346, 17585), DeliveryInfo(47665, 1682), DeliveryInfo(35923, 50927), DeliveryInfo(63199, 39492), DeliveryInfo(68455, 9755), DeliveryInfo(19446, 32431), DeliveryInfo(47665, 39492), DeliveryInfo(74738, 27735), DeliveryInfo(17884, 85331), DeliveryInfo(25504, 371), DeliveryInfo(62815, 5625), DeliveryInfo(41159, 103159), DeliveryInfo(98543, 59607), DeliveryInfo(45203, 20635), DeliveryInfo(19919, 90625), DeliveryInfo(87430, 101648), DeliveryInfo(71840, 52822), DeliveryInfo(9359, 106726), DeliveryInfo(29173, 101012), DeliveryInfo(62797, 25539), DeliveryInfo(2645, 11982), DeliveryInfo(38071, 70415), DeliveryInfo(100547, 44702), DeliveryInfo(19919, 3836), DeliveryInfo(102974, 88123), DeliveryInfo(51900, 27246), DeliveryInfo(54572, 60332), DeliveryInfo(82173, 43698), DeliveryInfo(93570, 32483), DeliveryInfo(1481, 90408), DeliveryInfo(97848, 30320), DeliveryInfo(17884, 38073), DeliveryInfo(27128, 87235), DeliveryInfo(73809, 59236), DeliveryInfo(59426, 83936), DeliveryInfo(91268, 30588), DeliveryInfo(99646, 18080), DeliveryInfo(81830, 83081), DeliveryInfo(89386, 32671), DeliveryInfo(19446, 89283), DeliveryInfo(100125, 11936), DeliveryInfo(68455, 60332), DeliveryInfo(14033, 65819), DeliveryInfo(19446, 25539), DeliveryInfo(50041, 51171), DeliveryInfo(19446, 22007), DeliveryInfo(41159, 23921), DeliveryInfo(91960, 75394), DeliveryInfo(68878, 94944), DeliveryInfo(98543, 80724), DeliveryInfo(102974, 61733), DeliveryInfo(17884, 72860), DeliveryInfo(68878, 13449), DeliveryInfo(51016, 39231)};
//        depots = {26, 101823, 66935, 87928, 70488};
//
//    {
//        //ECE297_TIME_CONSTRAINT(30000);
//
//        result_path = traveling_courier(deliveries, depots);
//    }
//
//    path_cost = compute_path_travel_time(result_path);
//    std::cout << "QoR extreme_multi_toronto: " << path_cost << std::endl;
//    all_path_cost.push_back(path_cost);
//    
//
//    close_map();
//    
//    //-------------------------------------------------------------------------------------------------------
//    //-------------------------------------------------------------------------------------------------------
//    
//    map_name = "/cad2/ece297s/public/maps/newyork.streets.bin";
//    load_map(map_name);
//    
//    
//    deliveries = {DeliveryInfo(68274, 73352), DeliveryInfo(139442, 9730), DeliveryInfo(25352, 130741), DeliveryInfo(117041, 140726), DeliveryInfo(52533, 82853), DeliveryInfo(93373, 109830), DeliveryInfo(41331, 80917), DeliveryInfo(23427, 76600), DeliveryInfo(57953, 136502), DeliveryInfo(93680, 109094), DeliveryInfo(15961, 100418), DeliveryInfo(60955, 116807), DeliveryInfo(81183, 107882), DeliveryInfo(44376, 63508), DeliveryInfo(573, 1768), DeliveryInfo(124986, 87619), DeliveryInfo(39695, 106028), DeliveryInfo(10459, 109960), DeliveryInfo(68570, 34878), DeliveryInfo(135820, 61176), DeliveryInfo(1428, 96731), DeliveryInfo(101852, 119393), DeliveryInfo(51838, 139533), DeliveryInfo(120915, 11533), DeliveryInfo(97264, 106146), DeliveryInfo(86805, 116022), DeliveryInfo(53023, 84888), DeliveryInfo(136689, 75068), DeliveryInfo(76914, 51190), DeliveryInfo(64885, 110547), DeliveryInfo(36370, 18163), DeliveryInfo(93136, 126481), DeliveryInfo(30041, 3531), DeliveryInfo(69278, 131756), DeliveryInfo(20315, 97318), DeliveryInfo(26533, 72620), DeliveryInfo(61851, 101930), DeliveryInfo(75563, 51649), DeliveryInfo(54391, 117236), DeliveryInfo(35274, 124986), DeliveryInfo(42835, 30088), DeliveryInfo(12005, 128609), DeliveryInfo(135399, 46931), DeliveryInfo(78881, 131168), DeliveryInfo(52932, 68701), DeliveryInfo(63397, 85925), DeliveryInfo(106839, 59629), DeliveryInfo(127390, 12582), DeliveryInfo(26811, 40441), DeliveryInfo(21385, 23561), DeliveryInfo(61336, 20033), DeliveryInfo(84365, 16640), DeliveryInfo(103080, 128855), DeliveryInfo(110681, 142678), DeliveryInfo(136840, 118364), DeliveryInfo(30414, 12594), DeliveryInfo(108384, 101275), DeliveryInfo(8218, 84675), DeliveryInfo(81728, 25379), DeliveryInfo(37271, 25903), DeliveryInfo(127006, 16961), DeliveryInfo(47281, 36275), DeliveryInfo(6214, 9344), DeliveryInfo(42163, 74163), DeliveryInfo(73954, 28875), DeliveryInfo(39091, 117227), DeliveryInfo(7967, 5703), DeliveryInfo(122417, 78560), DeliveryInfo(117557, 27784), DeliveryInfo(54516, 32303), DeliveryInfo(39217, 20572), DeliveryInfo(41527, 125894), DeliveryInfo(106260, 34057), DeliveryInfo(4488, 139281), DeliveryInfo(91957, 102722)};
//        depots = {43989, 66397, 59568, 103767};
//
//    {
//        //ECE297_TIME_CONSTRAINT(30000);
//
//        result_path = traveling_courier(deliveries, depots);
//    }
//
//    path_cost = compute_path_travel_time(result_path);
//    std::cout << "QoR hard_newyork: " << path_cost << std::endl;
//    all_path_cost.push_back(path_cost);
//    
//    //-------------------------------------------------------------------------------------------------------
//    deliveries = {DeliveryInfo(133686, 4806), DeliveryInfo(25609, 43597), DeliveryInfo(23272, 21288), DeliveryInfo(11052, 64799), DeliveryInfo(134890, 7424), DeliveryInfo(118398, 67664), DeliveryInfo(28776, 110174), DeliveryInfo(85924, 23066), DeliveryInfo(29907, 16656), DeliveryInfo(27845, 33913), DeliveryInfo(134890, 648), DeliveryInfo(134890, 42317), DeliveryInfo(131799, 83380), DeliveryInfo(3774, 31512), DeliveryInfo(120456, 136776), DeliveryInfo(77353, 57278), DeliveryInfo(22365, 106493), DeliveryInfo(52952, 34111), DeliveryInfo(68257, 114771), DeliveryInfo(21467, 117689), DeliveryInfo(39748, 95430), DeliveryInfo(73332, 90922), DeliveryInfo(114911, 23066), DeliveryInfo(138312, 134408), DeliveryInfo(80944, 62552), DeliveryInfo(95812, 13810), DeliveryInfo(134890, 103532), DeliveryInfo(43342, 123991), DeliveryInfo(77258, 98166), DeliveryInfo(39383, 56), DeliveryInfo(29907, 106403), DeliveryInfo(50143, 48101), DeliveryInfo(143607, 130598), DeliveryInfo(29907, 113233), DeliveryInfo(83471, 81999), DeliveryInfo(11653, 78441), DeliveryInfo(46323, 81539), DeliveryInfo(130055, 135934), DeliveryInfo(131935, 81962), DeliveryInfo(5760, 9627), DeliveryInfo(50860, 8233), DeliveryInfo(67621, 128004), DeliveryInfo(45426, 52006), DeliveryInfo(7892, 114676), DeliveryInfo(29907, 56679), DeliveryInfo(112272, 14232), DeliveryInfo(7764, 23066), DeliveryInfo(12203, 8611), DeliveryInfo(10511, 131866), DeliveryInfo(141156, 80168), DeliveryInfo(50279, 136776), DeliveryInfo(8505, 113209), DeliveryInfo(97501, 40429), DeliveryInfo(106823, 116211), DeliveryInfo(121176, 70964), DeliveryInfo(95588, 56377), DeliveryInfo(29079, 129523), DeliveryInfo(19945, 136776), DeliveryInfo(82632, 22120), DeliveryInfo(129213, 30720), DeliveryInfo(65297, 136776), DeliveryInfo(864, 27915), DeliveryInfo(38943, 38076), DeliveryInfo(85078, 37786), DeliveryInfo(29907, 10295), DeliveryInfo(38169, 97511), DeliveryInfo(32404, 93857), DeliveryInfo(21556, 116812), DeliveryInfo(13513, 103583), DeliveryInfo(77994, 86626), DeliveryInfo(20967, 138053), DeliveryInfo(88333, 124230), DeliveryInfo(112019, 39714), DeliveryInfo(59925, 10109), DeliveryInfo(105077, 43610)};
//        depots = {23, 87014, 30621, 35271, 113395, 47924, 88418};
//
//    {
//        //ECE297_TIME_CONSTRAINT(30000);
//
//        result_path = traveling_courier(deliveries, depots);
//    }
//
//    path_cost = compute_path_travel_time(result_path);
//    std::cout << "QoR hard_multi_newyork: " << path_cost << std::endl;
//    all_path_cost.push_back(path_cost);
//    //-------------------------------------------------------------------------------------------------------
//    deliveries = {DeliveryInfo(61841, 117003), DeliveryInfo(35544, 80546), DeliveryInfo(103555, 134237), DeliveryInfo(130455, 96271), DeliveryInfo(88315, 25975), DeliveryInfo(92635, 71177), DeliveryInfo(57515, 92503), DeliveryInfo(17114, 116939), DeliveryInfo(35401, 29202), DeliveryInfo(5690, 132041), DeliveryInfo(133620, 9961), DeliveryInfo(9700, 118066), DeliveryInfo(124179, 143139), DeliveryInfo(32947, 11824), DeliveryInfo(131147, 70069), DeliveryInfo(105426, 49128), DeliveryInfo(1258, 16990), DeliveryInfo(70549, 27084), DeliveryInfo(60090, 8570), DeliveryInfo(75584, 51317), DeliveryInfo(32450, 49713), DeliveryInfo(129867, 115919), DeliveryInfo(63339, 145664), DeliveryInfo(123223, 67980), DeliveryInfo(24232, 11395), DeliveryInfo(136605, 97522), DeliveryInfo(91570, 94319), DeliveryInfo(55890, 52181), DeliveryInfo(25661, 57608), DeliveryInfo(38068, 120577), DeliveryInfo(53152, 42744), DeliveryInfo(61574, 27454), DeliveryInfo(25093, 134514), DeliveryInfo(75829, 40602), DeliveryInfo(126448, 78542), DeliveryInfo(132959, 52811), DeliveryInfo(59456, 120877), DeliveryInfo(27444, 6617), DeliveryInfo(95389, 114401), DeliveryInfo(48803, 34314), DeliveryInfo(145896, 90755), DeliveryInfo(120076, 45029), DeliveryInfo(105657, 56315), DeliveryInfo(14261, 109483), DeliveryInfo(120535, 50305), DeliveryInfo(110079, 25699), DeliveryInfo(136253, 106846), DeliveryInfo(88321, 25369), DeliveryInfo(64803, 120136), DeliveryInfo(126393, 18069), DeliveryInfo(137126, 54794), DeliveryInfo(130630, 124618), DeliveryInfo(104838, 40897), DeliveryInfo(7271, 43798), DeliveryInfo(28981, 29953), DeliveryInfo(71781, 18745), DeliveryInfo(124022, 137898), DeliveryInfo(12706, 115707), DeliveryInfo(109230, 55696), DeliveryInfo(111572, 34404), DeliveryInfo(97769, 11379), DeliveryInfo(96012, 47326), DeliveryInfo(12656, 92375), DeliveryInfo(131225, 30454), DeliveryInfo(8342, 32005), DeliveryInfo(32919, 103087), DeliveryInfo(11702, 51863), DeliveryInfo(82209, 117368), DeliveryInfo(59670, 7753), DeliveryInfo(129847, 138905), DeliveryInfo(47200, 134988), DeliveryInfo(4711, 8647), DeliveryInfo(33544, 3275), DeliveryInfo(50034, 107673), DeliveryInfo(115484, 76410), DeliveryInfo(34470, 39829), DeliveryInfo(93170, 93463), DeliveryInfo(24244, 136840), DeliveryInfo(124835, 43108), DeliveryInfo(83516, 67874), DeliveryInfo(50440, 44847), DeliveryInfo(36783, 122669), DeliveryInfo(97888, 132242), DeliveryInfo(30439, 16938), DeliveryInfo(95852, 61239), DeliveryInfo(127733, 55474), DeliveryInfo(103054, 100816), DeliveryInfo(67180, 5067), DeliveryInfo(113624, 98930), DeliveryInfo(91152, 41251), DeliveryInfo(6767, 62606), DeliveryInfo(63582, 84585), DeliveryInfo(100440, 130536), DeliveryInfo(43187, 38374), DeliveryInfo(114546, 82139), DeliveryInfo(88417, 107542), DeliveryInfo(68424, 84996), DeliveryInfo(92437, 89242), DeliveryInfo(81409, 28781), DeliveryInfo(92727, 74462), DeliveryInfo(57111, 29857), DeliveryInfo(89590, 47850), DeliveryInfo(8669, 41801), DeliveryInfo(117217, 76087), DeliveryInfo(21462, 93807), DeliveryInfo(119122, 40388), DeliveryInfo(111325, 133346), DeliveryInfo(136421, 125080), DeliveryInfo(82506, 56862), DeliveryInfo(2282, 37584), DeliveryInfo(21240, 95677), DeliveryInfo(71340, 76490), DeliveryInfo(75798, 73156), DeliveryInfo(55638, 43351), DeliveryInfo(79901, 100305), DeliveryInfo(27945, 80741), DeliveryInfo(95008, 106514), DeliveryInfo(122641, 118945), DeliveryInfo(50297, 46398), DeliveryInfo(32566, 135772), DeliveryInfo(137092, 13189), DeliveryInfo(125005, 7722), DeliveryInfo(110775, 136835), DeliveryInfo(46147, 116523), DeliveryInfo(15492, 1106), DeliveryInfo(106237, 72387), DeliveryInfo(35411, 75127), DeliveryInfo(108852, 4738)};
//        depots = {143366};
//
//    {
//        //ECE297_TIME_CONSTRAINT(30000);
//
//        result_path = traveling_courier(deliveries, depots);
//    }
//
//    path_cost = compute_path_travel_time(result_path);
//    std::cout << "QoR extreme_newyork: " << path_cost << std::endl;
//    all_path_cost.push_back(path_cost);
//    //-------------------------------------------------------------------------------------------------------
//    deliveries = {DeliveryInfo(74717, 41044), DeliveryInfo(135023, 141288), DeliveryInfo(105095, 54703), DeliveryInfo(63954, 68717), DeliveryInfo(144480, 64957), DeliveryInfo(130760, 102336), DeliveryInfo(110297, 116925), DeliveryInfo(66498, 104502), DeliveryInfo(75502, 37669), DeliveryInfo(118680, 34285), DeliveryInfo(90631, 51499), DeliveryInfo(58354, 116925), DeliveryInfo(118680, 54703), DeliveryInfo(118680, 52683), DeliveryInfo(16146, 112867), DeliveryInfo(74717, 2331), DeliveryInfo(123492, 90042), DeliveryInfo(94657, 116906), DeliveryInfo(125437, 118339), DeliveryInfo(64132, 71957), DeliveryInfo(38950, 9706), DeliveryInfo(134035, 713), DeliveryInfo(42044, 141288), DeliveryInfo(144480, 122966), DeliveryInfo(42786, 143105), DeliveryInfo(74717, 87863), DeliveryInfo(43663, 41870), DeliveryInfo(3553, 81019), DeliveryInfo(131399, 97735), DeliveryInfo(142574, 48508), DeliveryInfo(130393, 3783), DeliveryInfo(142574, 101336), DeliveryInfo(143997, 98843), DeliveryInfo(93994, 47548), DeliveryInfo(133055, 13100), DeliveryInfo(39196, 108403), DeliveryInfo(74717, 143583), DeliveryInfo(102963, 82900), DeliveryInfo(144567, 60030), DeliveryInfo(59980, 112129), DeliveryInfo(96474, 89173), DeliveryInfo(76528, 99740), DeliveryInfo(115236, 136818), DeliveryInfo(138438, 100400), DeliveryInfo(130393, 3783), DeliveryInfo(110349, 1390), DeliveryInfo(48240, 114032), DeliveryInfo(69697, 70765), DeliveryInfo(144480, 40716), DeliveryInfo(97175, 119361), DeliveryInfo(59136, 3335), DeliveryInfo(11666, 10942), DeliveryInfo(79788, 67844), DeliveryInfo(97175, 62074), DeliveryInfo(94657, 11009), DeliveryInfo(85446, 71599), DeliveryInfo(94657, 137771), DeliveryInfo(14545, 118851), DeliveryInfo(15129, 109888), DeliveryInfo(54170, 64957), DeliveryInfo(36431, 90042), DeliveryInfo(19359, 111568), DeliveryInfo(93994, 46905), DeliveryInfo(144480, 27387), DeliveryInfo(74717, 116925), DeliveryInfo(53968, 24818), DeliveryInfo(100043, 64957), DeliveryInfo(143997, 143553), DeliveryInfo(90425, 87155), DeliveryInfo(79527, 110359), DeliveryInfo(130760, 103000), DeliveryInfo(74488, 115480), DeliveryInfo(16492, 127499), DeliveryInfo(82023, 37669), DeliveryInfo(73285, 498), DeliveryInfo(99893, 34285), DeliveryInfo(145422, 121700), DeliveryInfo(107913, 116925), DeliveryInfo(67200, 87155), DeliveryInfo(91088, 49741), DeliveryInfo(4375, 11091), DeliveryInfo(50819, 34297), DeliveryInfo(51125, 28810), DeliveryInfo(60926, 141288), DeliveryInfo(68509, 45395), DeliveryInfo(144480, 102336), DeliveryInfo(54962, 109888), DeliveryInfo(97041, 27379), DeliveryInfo(118680, 10942), DeliveryInfo(25941, 109913), DeliveryInfo(39196, 27379), DeliveryInfo(134457, 141288), DeliveryInfo(126183, 34285), DeliveryInfo(130393, 54703), DeliveryInfo(74717, 9526), DeliveryInfo(60843, 2650), DeliveryInfo(18845, 116925), DeliveryInfo(84869, 34285), DeliveryInfo(130760, 145256), DeliveryInfo(94657, 79547), DeliveryInfo(28986, 40263), DeliveryInfo(145862, 18061), DeliveryInfo(94657, 94560), DeliveryInfo(115243, 141710), DeliveryInfo(43008, 114032), DeliveryInfo(93994, 54703), DeliveryInfo(93994, 102336), DeliveryInfo(74717, 55001), DeliveryInfo(100522, 135648), DeliveryInfo(142574, 41076), DeliveryInfo(133813, 38966), DeliveryInfo(74717, 13436), DeliveryInfo(130393, 106461), DeliveryInfo(38754, 43621), DeliveryInfo(130760, 16028), DeliveryInfo(1989, 10942), DeliveryInfo(7265, 102336), DeliveryInfo(85378, 104675), DeliveryInfo(142574, 122238), DeliveryInfo(74717, 1586), DeliveryInfo(17610, 22150), DeliveryInfo(65482, 34285), DeliveryInfo(99953, 114032), DeliveryInfo(84354, 12649), DeliveryInfo(130760, 3783), DeliveryInfo(144480, 99713), DeliveryInfo(118680, 100338), DeliveryInfo(41525, 43873), DeliveryInfo(134035, 114098), DeliveryInfo(95059, 81693), DeliveryInfo(117703, 109888), DeliveryInfo(23350, 106461), DeliveryInfo(94657, 58681), DeliveryInfo(119783, 112717), DeliveryInfo(8074, 29868), DeliveryInfo(144480, 46539), DeliveryInfo(120937, 49457), DeliveryInfo(47828, 3783), DeliveryInfo(4024, 100454), DeliveryInfo(144480, 109888), DeliveryInfo(94657, 122966), DeliveryInfo(39196, 88387), DeliveryInfo(130760, 12565), DeliveryInfo(93994, 109913), DeliveryInfo(89887, 119897), DeliveryInfo(39196, 43551), DeliveryInfo(122563, 141288), DeliveryInfo(39177, 37669), DeliveryInfo(97068, 123439), DeliveryInfo(143997, 53034), DeliveryInfo(34249, 16091), DeliveryInfo(144480, 50025), DeliveryInfo(117409, 64957), DeliveryInfo(118680, 36588), DeliveryInfo(94657, 10942), DeliveryInfo(94657, 97842), DeliveryInfo(99117, 23615), DeliveryInfo(93994, 106499), DeliveryInfo(144480, 134286), DeliveryInfo(130760, 64321), DeliveryInfo(134035, 106461), DeliveryInfo(67942, 122966), DeliveryInfo(140064, 46539), DeliveryInfo(130393, 55579), DeliveryInfo(74717, 65707), DeliveryInfo(93994, 109913), DeliveryInfo(5531, 19930), DeliveryInfo(66580, 122966), DeliveryInfo(114561, 90042), DeliveryInfo(45543, 13436), DeliveryInfo(77746, 73709), DeliveryInfo(12569, 136502), DeliveryInfo(127403, 109913), DeliveryInfo(142574, 137425), DeliveryInfo(52634, 70429), DeliveryInfo(107262, 27148), DeliveryInfo(20694, 13436), DeliveryInfo(118680, 102336), DeliveryInfo(60702, 13417), DeliveryInfo(125655, 87155), DeliveryInfo(41857, 64321), DeliveryInfo(59136, 109888), DeliveryInfo(142574, 27379), DeliveryInfo(130760, 77708), DeliveryInfo(39196, 109913), DeliveryInfo(10830, 75678), DeliveryInfo(59136, 41772), DeliveryInfo(57454, 87155), DeliveryInfo(39196, 46554), DeliveryInfo(144286, 45020), DeliveryInfo(39196, 37669), DeliveryInfo(86987, 27379)};
//        depots = {36, 136736};
//
//    {
//        //ECE297_TIME_CONSTRAINT(30000);
//
//        result_path = traveling_courier(deliveries, depots);
//    }
//
//    path_cost = compute_path_travel_time(result_path);
//    std::cout << "QoR extreme_multi_newyork: " << path_cost << std::endl;
//    all_path_cost.push_back(path_cost);
//    close_map();
//    //-------------------------------------------------------------------------------------------------------
//    //-------------------------------------------------------------------------------------------------------
//    map_name = "/cad2/ece297s/public/maps/london_england.streets.bin";
//    load_map(map_name);
//    
//    deliveries = {DeliveryInfo(156322, 213465), DeliveryInfo(169790, 276994), DeliveryInfo(60442, 287428), DeliveryInfo(401665, 242595), DeliveryInfo(235088, 372239), DeliveryInfo(39315, 38530), DeliveryInfo(192987, 245805), DeliveryInfo(173700, 306362), DeliveryInfo(342430, 30437), DeliveryInfo(223234, 323219), DeliveryInfo(349892, 207769), DeliveryInfo(252656, 39098), DeliveryInfo(113676, 163020), DeliveryInfo(353422, 188132), DeliveryInfo(215876, 298495), DeliveryInfo(158540, 259709), DeliveryInfo(375153, 116228), DeliveryInfo(30748, 66136), DeliveryInfo(64723, 278752), DeliveryInfo(424643, 59002), DeliveryInfo(243409, 417548), DeliveryInfo(266079, 387137), DeliveryInfo(241313, 220437), DeliveryInfo(179046, 190713), DeliveryInfo(432418, 149487), DeliveryInfo(152996, 334355), DeliveryInfo(336561, 261345), DeliveryInfo(201742, 270168)};
//        depots = {59};
//
//    {
//        //ECE297_TIME_CONSTRAINT(30000);
//
//        result_path = traveling_courier(deliveries, depots);
//    }
//
//    path_cost = compute_path_travel_time(result_path);
//    std::cout << "QoR hard_london: " << path_cost << std::endl;
//    all_path_cost.push_back(path_cost);
//    //-------------------------------------------------------------------------------------------------------
//    deliveries = {DeliveryInfo(343938, 336040), DeliveryInfo(335283, 150084), DeliveryInfo(403738, 25457), DeliveryInfo(62758, 23623), DeliveryInfo(66940, 394891), DeliveryInfo(264642, 116559), DeliveryInfo(33082, 394891), DeliveryInfo(143440, 117876), DeliveryInfo(319729, 179361), DeliveryInfo(339401, 195441), DeliveryInfo(129725, 76650), DeliveryInfo(291829, 400133), DeliveryInfo(339401, 86129), DeliveryInfo(429827, 247326), DeliveryInfo(34879, 202395), DeliveryInfo(91652, 394891), DeliveryInfo(89516, 286772), DeliveryInfo(135963, 219488), DeliveryInfo(254647, 257177), DeliveryInfo(2586, 394891), DeliveryInfo(274269, 354374), DeliveryInfo(339401, 11296), DeliveryInfo(367690, 138649), DeliveryInfo(59697, 264388), DeliveryInfo(422492, 231240), DeliveryInfo(36527, 360534), DeliveryInfo(152228, 64254), DeliveryInfo(249835, 69656)};
//        depots = {68, 260440};
//
//    {
//        //ECE297_TIME_CONSTRAINT(30000);
//
//        result_path = traveling_courier(deliveries, depots);
//    }
//
//    path_cost = compute_path_travel_time(result_path);
//    std::cout << "QoR hard_multi_london: " << path_cost << std::endl;
//    all_path_cost.push_back(path_cost);
//    //-------------------------------------------------------------------------------------------------------
//    deliveries = {DeliveryInfo(372057, 286150), DeliveryInfo(158582, 291858), DeliveryInfo(297338, 326077), DeliveryInfo(51247, 404215), DeliveryInfo(411409, 408719), DeliveryInfo(283149, 59351), DeliveryInfo(41858, 345731), DeliveryInfo(435349, 38894), DeliveryInfo(424868, 103296), DeliveryInfo(41499, 244074), DeliveryInfo(113984, 337165), DeliveryInfo(359794, 152477), DeliveryInfo(428833, 397570), DeliveryInfo(392435, 256168), DeliveryInfo(392850, 324278), DeliveryInfo(6713, 52438), DeliveryInfo(190387, 317889), DeliveryInfo(335309, 97358), DeliveryInfo(391814, 294971), DeliveryInfo(404326, 392841), DeliveryInfo(29948, 309079), DeliveryInfo(417550, 163204), DeliveryInfo(435575, 61455), DeliveryInfo(416040, 182796), DeliveryInfo(193714, 126001), DeliveryInfo(275244, 431078), DeliveryInfo(141150, 279690), DeliveryInfo(264234, 313554), DeliveryInfo(402188, 318737), DeliveryInfo(253722, 355915), DeliveryInfo(291456, 73057), DeliveryInfo(37002, 397311), DeliveryInfo(5918, 367396), DeliveryInfo(64077, 61831), DeliveryInfo(65163, 172007), DeliveryInfo(269236, 333490), DeliveryInfo(409675, 346281), DeliveryInfo(231894, 403880), DeliveryInfo(400842, 143089), DeliveryInfo(196578, 169958), DeliveryInfo(390036, 18216), DeliveryInfo(114688, 401926), DeliveryInfo(65526, 351642)};
//        depots = {98};
//
//    {
//        //ECE297_TIME_CONSTRAINT(30000);
//
//        result_path = traveling_courier(deliveries, depots);
//    }
//
//    path_cost = compute_path_travel_time(result_path);
//    std::cout << "QoR extreme_london: " << path_cost << std::endl;
//    all_path_cost.push_back(path_cost);
//    //-------------------------------------------------------------------------------------------------------
//    deliveries = {DeliveryInfo(78162, 77645), DeliveryInfo(290453, 432699), DeliveryInfo(78162, 12044), DeliveryInfo(37886, 321044), DeliveryInfo(401177, 238030), DeliveryInfo(165433, 436576), DeliveryInfo(128727, 229056), DeliveryInfo(78162, 201135), DeliveryInfo(130686, 109040), DeliveryInfo(430995, 361973), DeliveryInfo(353417, 5953), DeliveryInfo(78162, 57943), DeliveryInfo(426735, 436576), DeliveryInfo(80065, 174659), DeliveryInfo(37886, 182356), DeliveryInfo(78162, 69889), DeliveryInfo(78162, 361973), DeliveryInfo(245503, 377677), DeliveryInfo(191584, 288753), DeliveryInfo(165433, 125841), DeliveryInfo(276847, 352295), DeliveryInfo(71884, 351414), DeliveryInfo(260359, 342891), DeliveryInfo(153022, 404136), DeliveryInfo(281331, 299167), DeliveryInfo(353417, 342891), DeliveryInfo(165433, 182108), DeliveryInfo(13095, 366840), DeliveryInfo(78162, 369622), DeliveryInfo(355219, 344910), DeliveryInfo(171966, 436576), DeliveryInfo(223633, 298989), DeliveryInfo(102511, 152107), DeliveryInfo(391375, 43534), DeliveryInfo(37886, 152107), DeliveryInfo(165433, 203356), DeliveryInfo(353417, 342891), DeliveryInfo(225984, 344910), DeliveryInfo(78162, 376094), DeliveryInfo(176999, 402439), DeliveryInfo(37886, 152107), DeliveryInfo(353417, 361973), DeliveryInfo(431859, 57943), DeliveryInfo(37886, 201135), DeliveryInfo(165433, 436576), DeliveryInfo(165433, 57943), DeliveryInfo(396082, 344932), DeliveryInfo(254020, 238811), DeliveryInfo(330284, 393288), DeliveryInfo(37886, 404136), DeliveryInfo(409263, 252478), DeliveryInfo(252405, 61940), DeliveryInfo(353417, 330129), DeliveryInfo(413889, 404136), DeliveryInfo(353417, 426737), DeliveryInfo(117317, 181687), DeliveryInfo(353417, 152107), DeliveryInfo(165433, 161531), DeliveryInfo(308178, 179525), DeliveryInfo(300870, 344910), DeliveryInfo(37886, 152107), DeliveryInfo(300402, 358520), DeliveryInfo(78162, 342891), DeliveryInfo(432441, 342891), DeliveryInfo(275146, 404136), DeliveryInfo(78162, 344910)};
//        depots = {108};
//
//    {
//        //ECE297_TIME_CONSTRAINT(30000);
//
//        result_path = traveling_courier(deliveries, depots);
//    }
//
//    path_cost = compute_path_travel_time(result_path);
//    std::cout << "QoR extreme_multi_london: " << path_cost << std::endl;
//    all_path_cost.push_back(path_cost);
//    //-------------------------------------------------------------------------------------------------------
//    for (unsigned i=0;i<all_path_cost.size();i++)
//        path_cost_sum*=all_path_cost.at(i);
//    
//    double final_QoR=1;
//    for (unsigned i=0;i<12;i++)
//        final_QoR*=pow(all_path_cost.at(i),1.0/12.0);
//    cout<<"QoR geometric average: "<<final_QoR;
//    
//    //-------------------------------------------------------------------------------------------------------
//    
//    
//    
//    
//    
//    close_map();
//    

        draw_map();
        close_map();

    return 0;
}
