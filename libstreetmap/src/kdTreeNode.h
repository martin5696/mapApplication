/* 
 * File:   kdTreeNode.h
 * Author: zhengz28
 *
 * Created on January 27, 2016, 3:45 PM
 */

#ifndef KDTREENODE_H
#define KDTREENODE_H

#include <math.h>
#include "kdtree.hpp"

struct coordinates {
    typedef double value_type;
    //two dimensions coordinates
    double xy[2];
    size_t index;
    int ID;

    coordinates(value_type a, value_type b, int ID_) {
        xy[0] = a;
        xy[1] = b;
        ID = ID_;
    }

    value_type operator[](size_t n) const {
        return xy[n];
    }
    
    
    
    double distance(const coordinates &other) {
        double x = xy[0] - other.xy[0];
        double y = xy[1] - other.xy[1];
        double distance = sqrt(x * x - y * y);
        return distance;
    }
};

bool operator == (const coordinates& lhs,const coordinates& rhs);
#endif /* KDTREENODE_H */

