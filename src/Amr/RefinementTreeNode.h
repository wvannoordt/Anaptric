#ifndef RefinementTreeNode_H
#define RefinementTreeNode_H

#include <string>
#include "Config.h"
#include "TikzObject.h"
#include <vector>
#include <utility>

namespace gTree
{
    struct NodeEdge
    {
        char edgeDirection;
        bool isDomainEdge;
    };
    
    class RefinementTreeNode
    {
        public:
            RefinementTreeNode(double* hostBounds, char refineType_in, char refineOrientation_in, int level_in, RefinementTreeNode* host_in);
            ~RefinementTreeNode(void);
            void Destroy(void);
            void RefineRandom();
            void DrawToObject(TikzObject* picture);
            void ResolveNewRefinementWithNeighbor(RefinementTreeNode* issuer);
            void CreateNewNeighbor(RefinementTreeNode* target, char edgeDirection, bool isDomainEdge);
            bool IsAnyDomainBoundary(void);
            RefinementTreeNode* RecursiveGetNodeAt(double coords[DIM]);
            void Refine(char newRefinementType);
        private:
            void InheritDomainBoundaryInfo(void);
            void DefineDirectionLevels(void);
            int GetIndexFromOctantAndRefineType(char location, char refinementType);
            int NumberOfNewSubNodes(char refinementType);
            int GetCoordBasis(char refinementType);
            int GetInvCoordBasis(char refinementType);
            void DefineBounds(double* hostBounds, char refineType_in, char refineOrientation_in);
            char refineType, refineOrientation;
            bool isTerminal, deallocSubTrees;
            char subNodeRefinementType;
            double blockBounds[2*DIM];
            RefinementTreeNode** subNodes;
            RefinementTreeNode* host;
            int numSubNodes, level;
            int directionLevels[DIM];
            std::map<RefinementTreeNode*, NodeEdge> neighbors;
            bool isOnBoundary[2*DIM];

    };
}

#endif