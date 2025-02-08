#ifndef SPRITE_H
#define SPRITE_H

#include <random>
#include <IVideoDriver.h>
#include <iostream>

using namespace irr;

class Sprite
{
public:
    Sprite(int X, int Y, int Width, int Height);
    ~Sprite();

    inline unsigned int get_id() { return id; }
    
    void draw(video::IVideoDriver* driver);

    bool IsVisible() { return visible;};

    void setVisible(bool flag) { visible = flag; }

    core::rect<s32> get_rect();

    void set_position(int X, int Y);

    std::vector<s32> get_position();

    void set_rect(int W, int H);

    void set_moved(bool flag) {isMoving = flag;}

    bool is_moved() {return isMoving;}

private:
    unsigned int id;
    int X, Y;
    int Width;
    int Height;

    bool visible = false;
    bool isMoving = false;
};

#endif