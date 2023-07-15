#ifndef __TOPO_MESH_BASE_HPP__
#define __TOPO_MESH_BASE_HPP__

#include <cmath>
#include <vector>
#include <queue>
#include <fstream>
#include <map>

#include "point.hpp"
#include "vector.hpp"
#include "point2.hpp"
#include "triangle2.hpp"

#define uint unsigned int


class TopoMeshBase;

class Vertex2 : public Point2 {
public:
    uint Face_ID = 0;

    Vertex2();
    Vertex2(Point2, uint);
};


class Face2 {
public:
    uint Vertex_ID[3];
    uint Face_ID[3];

    Face2();
    //Face2(uint);
    Face2(uint[], uint[]);
    Face2(const std::vector<uint>&, const std::vector<uint>&);
    Face2(uint, uint, uint, uint, uint, uint);

    Triangle2 ToTriangle(const TopoMeshBase*) const;
};


class TopoMeshBase {
protected:
    std::vector<Vertex2> vertices;
    std::vector<Face2> faces;
public:
    TopoMeshBase();
    TopoMeshBase(const char* file_name);
    TopoMeshBase(Point2, Point2, Point2);
    void ClearAndInit(Point2, Point2, Point2);

    double Area() const;  // return the total area covered by the mesh
    uint GetNbPoint() const;
    uint GetNbTriangle() const;

    Vertex2 Vertex(uint) const;
    Face2 Face(uint) const;

    std::vector<uint> GetFacesFromVertex(uint vertex_id) const; // return the list of face ID neighbouring vertex_id
    std::vector<uint> GetVerticesFromVertex(uint vertex_id) const; // return the list of vertex ID neighbouring vertex_id

    void DebugLog() const;

public: // pass to private once every thing is finished
    void ReOrderIDs(uint face_id, uint order);
    void SplitFace(uint face_id, const Point2& new_point);
    void FlipEdge(uint face_id_1, uint face_id_2);  // flip the edge between 2 faces

};


#endif