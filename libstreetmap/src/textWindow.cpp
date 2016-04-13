/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   textWindow.cpp
 * Author: zhengz28
 * 
 * Created on March 13, 2016, 4:52 PM
 */

#include "textWindow.h"
#define FONT_HEIGHT 5
using namespace std;
textWindow::textWindow(Display* display_,Window& parent, int pos_x, int pos_y , int width_,int height_ , unsigned long side_color, 
            unsigned long bgc):X_Window(display_,parent, pos_x,  pos_y ,  width_, height_ ,  side_color, bgc) {
}

textWindow::textWindow(){
    
}
textWindow::textWindow(const textWindow& orig) {
}

textWindow::~textWindow() {
}

void textWindow::Build(Display* display_, Window& parent, int pos_x, int pos_y, int width_, int height_, unsigned long side_color,
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
    
    //setup font size in text box
    string fontname("-*-helvetica-*-r-*-*-15-*-*-*-*-*-*-*");
    font = XLoadQueryFont (display, fontname.c_str());
    XSetFont (display, gc, font->fid);
}


void textWindow::show_contents(){
     //position where text is gonna be drawn
    int pos_x; 
    int pos_y;
    // variable used to read current text property
    int direction;
    int ascent;
    int descent;
    XCharStruct overall;
    
    XClearWindow (display, win);
    for(unsigned i = 0 ; i < text.size(); i++){
        XTextExtents (font, text[i].c_str(), text[i].length(),
                    & direction, & ascent, & descent, & overall);
        pos_x = x + width /2 - overall.width / 2;
        pos_y = y + FONT_HEIGHT * (i+1) * (ascent - descent) / 2;
        XDrawString (display, win, gc, pos_x, pos_y, text[i].c_str(), text[i].length());
    }
}

void textWindow::writeContents(string newContents){
    text.push_back(newContents);
    show_contents();
}

void textWindow::clearContents(){
    text.clear();
    show_contents();
}

void textWindow::displayWindow(bool turn_on){
    if(turn_on){
        text.clear();
        XMapWindow(display,win);
        XClearWindow(display,win);
    }
    else{ 
        text.clear();
        XClearWindow(display,win);
        XUnmapWindow(display,win);
    }
}