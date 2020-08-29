#include <iostream>
#include <cmath>
#include "gTree.h"

int main(int argc, char** argv)
{
    gTree::Initialize();
    gTree::ReadInput("input.ptl");
    gTree::RefinementBlock domain("Domain");
    double coords[2];
    coords[0] = 0.432;
    coords[1] = 0.354;
    domain.RefineAt(coords, 1);
    std::string filename = "output/main.tex";
    gTree::TikzObject picture;
    picture.Open(filename);
	picture.SetClip(0,0,1,1);
    domain.Render(&picture);
    picture.Close();
    gTree::Finalize();
    return 0;
}