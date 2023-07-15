#ifndef __TOPO_MESH_2D_HPP__
#define __TOPO_MESH_2D_HPP__


#include <cmath>
#include <vector>
#include <queue>
#include <fstream>
#include <map>

#include "point.hpp"
#include "vector.hpp"
#include "point2.hpp"
#include "triangle2.hpp"


class TopoMesh2D;


class Vertex2 : public Point2 {
public:
    unsigned int Face_ID = 0;

    Vertex2();
    Vertex2(Point2, unsigned int);
};


class Face2 {
public:
    unsigned int Vertex_ID[3];
    unsigned int Face_ID[3];

    Face2();
    Face2(unsigned int[], unsigned int[]);
    Face2(const std::vector<unsigned int>& vid, const std::vector<unsigned int>& fid);
    Face2(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);

    Triangle2 ToTriangle(const TopoMesh2D*) const;
};


class TopoMesh2D {
protected:
    std::vector<Vertex2> vertices = {Vertex2()};  // this first vertex is the infinite vertex
    std::vector<Face2> faces = {Face2()};
public:
    TopoMesh2D();
    TopoMesh2D(const char* file_name);
    TopoMesh2D(Point2, Point2, Point2);

    void ClearAndInit(Point2, Point2, Point2);

    double Area() const;  // return the total area covered by the mesh
    unsigned int GetNbPoint() const;
    unsigned int GetNbTriangle() const;

    Vertex2 Vertex(unsigned int) const;
    Face2 Face(unsigned int) const;

    std::vector<unsigned int> GetFacesFromVertex(unsigned int vertex_id) const; // return the list of face ID neighbouring vertex_id
    std::vector<unsigned int> GetVerticesFromVertex(unsigned int vertex_id) const; // return the list of vertex ID neighbouring vertex_id

    void DebugLog() const;

    bool IsInfinite(unsigned int) const;
    bool IsOnConvexHull(unsigned int) const;

    bool TestDelaunay(unsigned int, unsigned int) const;
    void PartialLawson(const std::queue<std::pair<unsigned int, unsigned int>>& start_queue);  // apply the lawson algorithm from the given queue
    void CompleteLawson();  // apply the lawson algorithm to the whole mesh

    unsigned int GetFace(const Point2& point) const;  // return the face_id of the face containing the point
    bool AddPoint(const Point2&, bool keepDelaunay=false);  // add a point in the mesh

public: // pass to private once every thing is finished
    void ReOrderIDs(unsigned int face_id, unsigned int order);
    void SplitFace(unsigned int face_id, const Point2& new_point);
    void FlipEdge(unsigned int face_id_1, unsigned int face_id_2);  // flip the edge between 2 faces
    void SplitFaceDelaunay(unsigned int face_id, const Point2& new_point);
    void AddOutsiderPoint(const Point2& new_point, bool keepDelaunay=false);

};


#endif