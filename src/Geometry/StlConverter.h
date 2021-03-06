#ifndef CMF_STL_CONVERTER
#define CMF_STL_CONVERTER
#include <string>
#include "ICmfGeometryConverter.h"
#include "CmfScreen.h"
#define STL_HDR_SIZE 80
namespace cmf
{
    /// @brief A class for reading and convertind STL file triangulation data
    /// @author WVN
    class StlConverter : public ICmfGeometryConverter
    {
        public:
            
            /// @brief Constructor for StlConverter class
            /// @param filename The file name to be read
            /// @param target_in The surface object to be converted
            /// @author WVN
            StlConverter(std::string filename_in, SurfaceTriangulation* target);
            
            /// @brief Destrctor for StlConverter class
            /// @author WVN
            ~StlConverter(void);
            
            /// @brief Writes a point cloud CSV file
            /// @param filename The file to write to
            /// @author WVN
            void SavePointCloud(std::string filename);
            
            /// @brief Converts the current representation to the cmf standard format
            /// @author WVN
            void ConvertGeometry(void);
            
            /// @brief Saves the native geometry as the corresponding file type
            /// @param target The surface to write
            /// @param filename The file to write to
            /// @author WVN
            void SaveGeometry(void);
            
            /// @brief Outputs a subset of the surface triangulation as an STL file
            /// @param set The indices of the triangles to write
            /// @author WVN
            void OutputFaceSubsetAsStl(std::vector<int> set);
            
        private:
            
            /// @brief The number of faces in the stl file
            int facetCount;
            
            /// @brief A buffer to store header metadata
            char header[STL_HDR_SIZE];
            
            /// @brief minimum x-coordinate
            float xmin;
            
            /// @brief maximum x-coordinate
            float xmax;
            
            /// @brief minimum y-coordinate
            float ymin;
            
            /// @brief maximum y-coordinate
            float ymax;
            
            /// @brief minimum z-coordinate
            float zmin;
            
            /// @brief maximum z-coordinate
            float zmax;
            
            /// @brief indicates whether the underlying normal and point buffers need to be freed
            bool requireFree;
            
            /// @brief allocates the normal and point buffers
            /// @param num number of faces
            /// @author WVN
            void Allocate(int num);
            
            /// @brief reads data from the given file
            /// @param filename The file to read from
            /// @author WVN
            void ReadFromFile(std::string filename);
            
            /// @brief indicates whether the current machine uses big-endian byte order
            bool bigEndian;
            
            /// @brief buffer for normal vector data
            float* normalData;
            
            /// @brief buffer for vertex data
            float* vertexData;
    };
};

#endif