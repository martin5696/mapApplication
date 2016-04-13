/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   X_Window.h
 * Author: zhengz28
 *
 * Created on March 13, 2016, 3:28 PM
 */

#ifndef X_WINDOW_H
#define X_WINDOW_H

#include <./X11/Xlib.h>

class X_Window {
public:
    X_Window(Display* display_, Window& parent, int pos_x, int pos_y , int width_,int height_ , unsigned long side_color, 
            unsigned long bgc);
    X_Window();
    X_Window(const X_Window& orig);
    
    void Build (Display* display,Window& parent, int pos_x, int pos_y , int width_,int height_ , unsigned long side_color, 
            unsigned long bgc);
    virtual ~X_Window();
    
    void displayWindow(bool turn_on);
    
protected:
    Display* display;
    int x;
    int y;
    int width;
    int height;
    
    Window win;
    GC gc;
    
};

#endif /* X_WINDOW_H */

