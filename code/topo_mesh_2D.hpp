#ifndef __TOPO_MESH_2D_HPP__
#define __TOPO_MESH_2D_HPP__

#include "topo_mesh_base.hpp"

class TopoMesh2D : public TopoMeshBase {
protected:
    std::vector<Vertex2> vertices = {Vertex2()};  // this first vertex is the infinite vertex
    std::vector<Face2> faces = {Face2()};
public:
    TopoMesh2D();
    TopoMesh2D(const char* file_name);
    TopoMesh2D(Point2, Point2, Point2);

    bool IsInfinite(uint) const;
    bool IsOnConvexHull(uint) const;

    bool TestDelaunay(uint, uint) const;
    void PartialLawson(const std::queue<std::pair<uint, uint>>& start_queue);  // apply the lawson algorithm from the given queue
    void CompleteLawson();  // apply the lawson algorithm to the whole mesh

    uint GetFace(const Point2& point) const;  // return the face_id of the face containing the point
    bool AddPoint(const Point2&, bool keepDelaunay=false);  // add a point in the mesh

public: // pass to private once every thing is finished
    void SplitFaceDelaunay(uint face_id, const Point2& new_point);
    void AddOutsiderPoint(const Point2& new_point, bool keepDelaunay=false);

};


#endif