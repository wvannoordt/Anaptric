#include <iostream>
#include <cmath>
#include "cmf.h"
#include "cmftestutils.h"
int main(int argc, char** argv)
{
    EXIT_WARN_IF_DIM_NOT(3);
    bool doRefinement = false;
    cmf::Initialize();
    cmf::ReadInput("input.ptl");
    cmf::globalSettings = cmf::GlobalSettings(cmf::mainInput["GlobalSettings"]);
    cmf::CartesianMeshInputInfo inputInfo(cmf::mainInput["Domain"]);
    cmf::CartesianMesh domain(inputInfo);
    double coords[2];
    coords[0] = 0.23;
    coords[1] = 0.54;
    double radius = 0.1;
    cmf::AxisAlignedLongCylinder cyl(coords, radius, 2);
    domain.Blocks()->SetRefineLimitCriterion([](cmf::RefinementTreeNode* n){return (n->GetLevel() > 3);});
    if (doRefinement)
    {
        for (cmf::BlockIterator lb(domain.Blocks(), cmf::BlockFilters::Terminal); lb.HasNext(); lb++)
        {
            if (cyl.BoxIntersectsBoundary(lb.Node()->GetBlockBounds())) lb.Node()->Refine(7);
        }
    }
    domain.Blocks()->OutputDebugVtk("output/mesh.vtk");
    cmf::CartesianMeshArray x = domain.CreateCoordinateVariable(0);
    cmf::CartesianMeshArray y = domain.CreateCoordinateVariable(1);
    cmf::CartesianMeshArray z = domain.CreateCoordinateVariable(2);
    cmf::SerialCartesianVtk dataVtk(domain, "output/data.vtk");
    dataVtk << x;
    dataVtk << y;
    dataVtk << z;
    dataVtk.Write();
    cmf::Finalize();
    return 0;
}
