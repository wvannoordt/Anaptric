#ifndef TIKZCOLOR_H
#define TIKZCOLOR_H

#include <string>

namespace gTree
{
    namespace TikzColor
    {
        enum TikzColor
        {
            red,
            green,
            blue,
            cyan,
            magenta,
            yellow,
            black,
            gray,
            darkgray,
            lightgray,
            brown,
            lime,
            olive,
            orange,
            pink,
            purple,
            teal,
            violet,
            white
        };
    }

    inline static std::string TikzColorStr(const int color)
    {
        switch (color)
        {
            case TikzColor::red: return "red";
            case TikzColor::green: return "green";
            case TikzColor::blue: return "blue";
            case TikzColor::cyan: return "cyan";
            case TikzColor::magenta: return "magenta";
            case TikzColor::yellow: return "yellow";
            case TikzColor::black: return "black";
            case TikzColor::gray: return "gray";
            case TikzColor::darkgray: return "darkgray";
            case TikzColor::lightgray: return "lightgray";
            case TikzColor::brown: return "brown";
            case TikzColor::lime: return "lime";
            case TikzColor::olive: return "olive";
            case TikzColor::orange: return "orange";
            case TikzColor::pink: return "pink";
            case TikzColor::purple: return "purple";
            case TikzColor::teal: return "teal";
            case TikzColor::violet: return "violet";
            case TikzColor::white: return "white";
        }
        return "??????????????????";
    }
}
#endif
