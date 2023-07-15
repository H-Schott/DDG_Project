#include "topo_mesh_2D.hpp"

Vertex2::Vertex2() : Point2() {};

Vertex2::Vertex2(Point2 _point, unsigned int _Face_ID) : Point2(_point), Face_ID(_Face_ID) {};


//EOF