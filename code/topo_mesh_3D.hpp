#ifndef __TOPO_MESH_3D_HPP__
#define __TOPO_MESH_3D_HPP__


#include <cmath>
#include <vector>
#include <queue>
#include <fstream>
#include <map>

#include "point.hpp"
#include "vector.hpp"
#include "triangle.hpp"

#include <Eigen/SparseCore>


class Mesh;
class TopoMesh3D;

class Vertex3 : public Point {
public:
    unsigned int Face_ID = 0;

    Vertex3();
    Vertex3(Point, unsigned int);
};


class Face3 {
public:
    unsigned int Vertex_ID[3];
    unsigned int Face_ID[3];

    Face3();
    Face3(unsigned int[], unsigned int[]);
    Face3(const std::vector<unsigned int>& vid, const std::vector<unsigned int>& fid);
    Face3(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);

    Triangle ToTriangle(const TopoMesh3D*) const;
};


class TopoMesh3D {
protected:
    std::vector<Vertex3> vertices;  // this first vertex is the infinite vertex
    std::vector<Face3> faces;
public:
    TopoMesh3D();
    TopoMesh3D(const char* file_name);
    TopoMesh3D(Point, Point, Point, Point);
    TopoMesh3D(const Mesh&);

    void ClearAndInit(Point, Point, Point, Point);

    double Area() const;  // return the total area covered by the mesh
    unsigned int GetNbPoint() const;
    unsigned int GetNbTriangle() const;

    Vertex3 GetVertex(unsigned int) const;
    Face3 GetFace(unsigned int) const;

    std::vector<unsigned int> GetFacesFromVertex(unsigned int vertex_id) const; // return the list of face ID neighbouring vertex_id
    std::vector<unsigned int> GetVerticesFromVertex(unsigned int vertex_id) const; // return the list of vertex ID neighbouring vertex_id
    std::vector<unsigned int> GetValence() const; // res[i] = #vertices_with_i_neighbors

    double Laplacian(unsigned int vertex_id, unsigned int component_id) const;
    Vector Laplacian(unsigned int vertex_id) const;
    std::vector<double> LaplacianNorms(bool normalized = true) const;
    std::vector<double> LaplacianNormsMatrix(bool normalized = true) const;
    std::vector<Vector> Laplacians(bool normalized = true) const;
    std::vector<Vector> LaplaciansMatrix(bool normalized = true) const;

    Vector Gradient(unsigned int face_id, double f0, double f1, double f2) const;
    std::vector<double> GradientNormsZ(bool normalized = true) const;

    // Matrices
    Eigen::SparseMatrix<bool> GetConnectivityMatrix() const;
    Eigen::SparseMatrix<double> GetGradientMatrix() const;
    Eigen::SparseMatrix<double> GetLaplacianMatrix() const;

    // Edition
    void Diffusion(double k);
    void Smooth();

    void DebugLog() const;

    Mesh ToGlMesh_3() const;
    Mesh ToGlMesh() const;

public: // pass to private once every thing is finished
    void ReOrderIDs(unsigned int face_id, unsigned int order);
    void SplitFace(unsigned int face_id, const Point& new_point);
    void FlipEdge(unsigned int face_id_1, unsigned int face_id_2);  // flip the edge between 2 faces

};


#endif