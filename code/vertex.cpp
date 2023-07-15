#include "topo_mesh_3D.hpp"

Vertex3::Vertex3() : Point() {};

Vertex3::Vertex3(Point _point, unsigned int _Face_ID) : Point(_point), Face_ID(_Face_ID) {};


//EOF