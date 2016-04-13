/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   FeatureSort.h
 * Author: zhengz28
 *
 * Created on February 21, 2016, 3:12 PM
 */

#ifndef FEATURESORT_H
#define FEATURESORT_H

struct FeatureSort{
    std::vector<unsigned> lakeAndGreenSpaceAndIslandsAndShoreline;
    std::vector<unsigned> freshWater;
    std::vector<unsigned> buildings;
    std::vector<unsigned> beach;
    std::vector<unsigned> unknowns;
};


#endif /* FEATURESORT_H */

