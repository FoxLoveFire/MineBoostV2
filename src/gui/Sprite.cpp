#include "Sprite.h"

int generate_id(int min, int max)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(min, max);

    return dist6(rng);
}

Sprite::Sprite(int X, int Y, int Width, int Height)
    : X(X), Y(Y), Width(Width), Height(Height), id(generate_id(100, 1000))
{

}

void Sprite::set_position(int X, int Y)
{
    this->X = X; this->Y = Y;
}

void Sprite::set_rect(int W, int H)
{
    this->Width = W; this->Height = H;
}

core::rect<s32> Sprite::get_rect()
{
    return core::rect<s32>(X, Y, X + Width, Y + Height);
}

std::vector<s32> Sprite::get_position()
{
    return std::vector<s32>{this->X, this->Y};
}

void Sprite::draw(video::IVideoDriver* driver)
{
    if (IsVisible()) {
        driver->draw2DRectangle(video::SColor(125, 0, 255, 0),
                core::rect<s32>(X, Y, X + Width, Y + Height));
    }
}
