#include "planar_mesh.hpp"

Face2::Face2() {
    Vertex_ID[0] = 0;
    Vertex_ID[1] = 0;
    Vertex_ID[2] = 0;

    Face_ID[0] = 0;
    Face_ID[1] = 0;
    Face_ID[2] = 0;
}

Face2::Face2(uint vid[3], uint fid[3]) {
    Vertex_ID[0] = vid[0];
    Vertex_ID[1] = vid[1];
    Vertex_ID[2] = vid[2];

    Face_ID[0] = fid[0];
    Face_ID[1] = fid[1];
    Face_ID[2] = fid[2];
}

Face2::Face2(uint vid1, uint vid2, uint vid3, uint fid1, uint fid2, uint fid3) {
    Vertex_ID[0] = vid1;
    Vertex_ID[1] = vid2;
    Vertex_ID[2] = vid3;

    Face_ID[0] = fid1;
    Face_ID[1] = fid2;
    Face_ID[2] = fid3;
}

Triangle2 Face2::ToTriangle(const PlanarMesh* PM) const {
    return Triangle2(PM->Vertex(Vertex_ID[0]), PM->Vertex(Vertex_ID[1]), PM->Vertex(Vertex_ID[2]));
}


//EOF