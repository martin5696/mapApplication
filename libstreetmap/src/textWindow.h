/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   textWindow.h
 * Author: zhengz28
 *
 * Created on March 13, 2016, 4:52 PM
 */

#ifndef TEXTWINDOW_H
#define TEXTWINDOW_H
#include "X_Window.h"
#include <string>
#include <vector>
class textWindow:public X_Window {
public:
    textWindow(Display* display_,Window& parent, int pos_x, int pos_y , int width_,int height_ , unsigned long side_color, 
            unsigned long bgc);
    
    textWindow();
    
    textWindow(const textWindow& orig);
    virtual ~textWindow();
    
    void writeContents(std::string newContents);
    
    void clearContents();
    
    void Build (Display* display,Window& parent, int pos_x, int pos_y , int width_,int height_ , unsigned long side_color, 
            unsigned long bgc);
    
    void displayWindow(bool turn_on);
    
private:
    //Store different line of contents 
    std::vector<std::string> text;
    
    XFontStruct* font;
     
    void show_contents();
    
};

#endif /* TEXTWINDOW_H */

