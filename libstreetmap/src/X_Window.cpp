/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   X_Window.cpp
 * Author: zhengz28
 * 
 * Created on March 13, 2016, 3:28 PM
 */

#include "X_Window.h"
using namespace std;

X_Window::X_Window(Display* display_,Window& parent, int pos_x, int pos_y , int width_,int height_ , unsigned long side_color, 
            unsigned long bgc): display(display_),x(pos_x),y(pos_y),width(width_),height(height_){
    
    win = XCreateSimpleWindow(display_, parent, pos_x,pos_y,width_,height_,1,
           side_color,bgc);
    
    XSelectInput(display, win, ButtonPressMask|ExposureMask|KeyPressMask|KeyReleaseMask|KeymapStateMask);
    
    unsigned long valuemask = 0; /* ignore XGCvalues and use defaults */
    XGCValues values;
    
    gc = XCreateGC(display,win,valuemask,&values);
    
}

X_Window::X_Window(){
    
}


X_Window::X_Window(const X_Window& orig) {
}

X_Window::~X_Window() {
    
}

void X_Window::Build(Display* display_, Window& parent, int pos_x, int pos_y, int width_, int height_, unsigned long side_color,
        unsigned long bgc) {
    display = display_;
    x = pos_x;
    y = pos_y;
    width = width_;
    height = height_;
    win = XCreateSimpleWindow(display_, parent, pos_x, pos_y, width_, height_, 1,
            side_color, bgc);

    XSelectInput(display, win, ExposureMask | KeyPressMask | KeyReleaseMask | KeymapStateMask);
    
    unsigned long valuemask = 0; /* ignore XGCvalues and use defaults */
    XGCValues values;
    
    gc = XCreateGC(display,win,valuemask,&values);
    
}

void X_Window::displayWindow(bool turn_on){
    if(turn_on){
        XMapWindow(display,win);
    }
    else{ 
        XUnmapWindow(display,win);
    }
}