/**
 * @file planar_mesh.h
 * @author Hugo Schott
 * @brief Implements several classes to handle planar meshs.
 * @version 0.1
 * @date 2022-03-05
 */

#ifndef __PLANAR_MESH_HPP__
#define __PLANAR_MESH_HPP__

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


class PlanarMesh;


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

    Triangle2 ToTriangle(const PlanarMesh*) const;
};



class PlanarMesh {
protected:
    std::vector<Vertex2> vertices = {Vertex2()};  // this first vertex is the infinite vertex
    std::vector<Face2> faces = {Face2()};
public:
    PlanarMesh();
    PlanarMesh(const char* file_name);
    PlanarMesh(Point2, Point2, Point2);
    void ClearAndInit(Point2, Point2, Point2);

    double Area() const;  // return the total area covered by the mesh
    uint GetNbPoint() const;
    uint GetNbTriangle() const;

    Vertex2 Vertex(uint) const;
    Face2 Face(uint) const;

    bool IsInfinite(uint) const;
    bool IsOnConvexHull(uint) const;

    std::vector<uint> GetFacesFromVertex(uint vertex_id) const; // return the list of face ID neighbouring vertex_id
    std::vector<uint> GetVerticesFromVertex(uint vertex_id) const; // return the list of vertex ID neighbouring vertex_id

    bool TestDelaunay(uint, uint) const;
    void PartialLawson(const std::queue<std::pair<uint, uint>>& start_queue);  // apply the lawson algorithm from the given queue
    void CompleteLawson();  // apply the lawson algorithm to the whole mesh

    uint GetFace(const Point2& point) const;  // return the face_id of the face containing the point
    bool AddPoint(const Point2&, bool keepDelaunay=false);  // add a point in the mesh

    void DebugLog() const;

public: // pass to private once every thing is finished
    void ReOrderIDs(uint face_id, uint order);
    void SplitFace(uint face_id, const Point2& new_point, bool keepDelaunay=false);
    void AddOutsiderPoint(const Point2& new_point, bool keepDelaunay=false);
    void FlipEdge(uint face_id_1, uint face_id_2);  // flip the edge between 2 faces

};


#endif