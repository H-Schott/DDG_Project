#include "topo_mesh_3D.hpp"

Face3::Face3() {
    Vertex_ID[0] = 0;
    Vertex_ID[1] = 0;
    Vertex_ID[2] = 0;

    Face_ID[0] = 0;
    Face_ID[1] = 0;
    Face_ID[2] = 0;
}

Face3::Face3(unsigned int vid[3], unsigned int fid[3]) {
    Vertex_ID[0] = vid[0];
    Vertex_ID[1] = vid[1];
    Vertex_ID[2] = vid[2];

    Face_ID[0] = fid[0];
    Face_ID[1] = fid[1];
    Face_ID[2] = fid[2];
}

Face3::Face3(const std::vector<unsigned int>& vid, const std::vector<unsigned int>& fid) {
    Vertex_ID[0] = vid[0];
    Vertex_ID[1] = vid[1];
    Vertex_ID[2] = vid[2];

    Face_ID[0] = fid[0];
    Face_ID[1] = fid[1];
    Face_ID[2] = fid[2];
}

Face3::Face3(unsigned int vid1, unsigned int vid2, unsigned int vid3, unsigned int fid1, unsigned int fid2, unsigned int fid3) {
    Vertex_ID[0] = vid1;
    Vertex_ID[1] = vid2;
    Vertex_ID[2] = vid3;

    Face_ID[0] = fid1;
    Face_ID[1] = fid2;
    Face_ID[2] = fid3;
}

Triangle Face3::ToTriangle(const TopoMesh3D* PM) const {
    return Triangle(PM->Vertex(Vertex_ID[0]), PM->Vertex(Vertex_ID[1]), PM->Vertex(Vertex_ID[2]));
}


//EOF