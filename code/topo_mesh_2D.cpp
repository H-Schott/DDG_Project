#include "topo_mesh_2D.hpp"


bool TopoMesh2D::IsInfinite(uint face_id) const {
    return faces[face_id].Vertex_ID[0] == 0;
}

bool TopoMesh2D::IsOnConvexHull(uint vertex_id) const {
    return IsInfinite(vertices[vertex_id].Face_ID);
}


bool TopoMesh2D::TestDelaunay(uint face_id_1, uint face_id_2) const {

    // if zero face, consider delaunay
    if (IsInfinite(face_id_1) || IsInfinite(face_id_2)) return true;

    // place each face wrt each other
    uint i_R_12 = 4;
    uint i_R_21 = 4;
    for (uint i = 0; i < 3; i++) {
        if(faces[face_id_1].Face_ID[i] == face_id_2) i_R_12 = i;
        if(faces[face_id_2].Face_ID[i] == face_id_1) i_R_21 = i;
    }

    if (i_R_12 == 4 || i_R_21 == 4) {
        std::cerr << "error -TestDelaunay- edge does not exist\n";
        return true;
    }

    // get 4 points of the 2 faces
    Point A = vertices[faces[face_id_1].Vertex_ID[i_R_12]].ToPoint();
    Point B = vertices[faces[face_id_1].Vertex_ID[(i_R_12 + 1) % 3]].ToPoint();
    Point C = vertices[faces[face_id_1].Vertex_ID[(i_R_12 + 2) % 3]].ToPoint();
    Point P = vertices[faces[face_id_2].Vertex_ID[i_R_21]].ToPoint();

    // on calcule les points sur le paraboloide
    A.z = A.x * A.x + A.y * A.y;
    B.z = B.x * B.x + B.y * B.y;
    C.z = C.x * C.x + C.y * C.y;
    P.z = P.x * P.x + P.y * P.y;

    // on calcule les vecteurs utiles
    Point AB = Point(B.x - A.x, B.y - A.y, B.z - A.z);
    Point AC = Point(C.x - A.x, C.y - A.y, C.z - A.z);
    Point AP = Point(P.x - A.x, P.y - A.y, P.z - A.z);

    // on calcule le vecteur normal au plan du cercle circonscrit
    Vector N = Cross(Vector(AB), Vector(AC));

    return Dot(N, Vector(AP)) > 0;
}

void TopoMesh2D::PartialLawson(const std::queue<std::pair<uint, uint>>& start_queue) {

    std::queue<std::pair<uint, uint>> edge_queue = start_queue;

    std::pair<uint, uint> edge;
    std::pair<uint, uint> edge_temp;
    uint k = 0;
    while(!edge_queue.empty()){
        k++;
        edge = edge_queue.front();
        edge_queue.pop();

        if(!TestDelaunay(edge.first, edge.second)){

            FlipEdge(edge.first, edge.second);

            // check and push neighbouring edges
            uint i_R_12;
            uint i_R_21;
            for (uint i = 0; i < 3; i++){
                if(faces[edge.first].Face_ID[i] == edge.second){
                    i_R_12 = i;
                }
                if(faces[edge.second].Face_ID[i] == edge.first){
                    i_R_21 = i;
                }
            }

            edge_temp.first = faces[edge.first].Face_ID[(i_R_12 + 1) % 3];
            edge_temp.second = edge.first;
            if(!TestDelaunay(edge_temp.first, edge_temp.second)){
                edge_queue.push(edge_temp);
            }
            edge_temp.first = faces[edge.first].Face_ID[(i_R_12 + 2) % 3];
            if(!TestDelaunay(edge_temp.first, edge_temp.second)){
                edge_queue.push(edge_temp);
            }
            edge_temp.first = faces[edge.second].Face_ID[(i_R_21 + 1) % 3];
            edge_temp.second = edge.second;
            if(!TestDelaunay(edge_temp.first, edge_temp.second)){
                edge_queue.push(edge_temp);
            }
            edge_temp.first = faces[edge.second].Face_ID[(i_R_21 + 2) % 3];
            if(!TestDelaunay(edge_temp.first, edge_temp.second)){
                edge_queue.push(edge_temp);
            }
        }
    }
}

void TopoMesh2D::CompleteLawson() {
    std::queue<std::pair<uint, uint>> edge_queue;

    uint nb_faces = faces.size();
    for (uint i = 1; i < nb_faces; i++) {
        if (IsInfinite(i)) continue;
        for (uint j = 0; j < 3; j++) {
            uint second_face_id = faces[i].Face_ID[j];
            if (!IsInfinite(second_face_id) && i < second_face_id) {
                edge_queue.push(std::make_pair(i, second_face_id));
            }
        }
    }
    PartialLawson(edge_queue);
}


uint TopoMesh2D::GetFace(const Point2& point) const {
    // start from face 1
    uint current_face_id = 1;
    if (faces[current_face_id].ToTriangle(this).Contains(point)) return current_face_id;

    // init previous_face_id
    uint previous_face_id = 0;

    for (uint i = 0; i < 3; i++) {
        Point2 a = vertices[faces[current_face_id].Vertex_ID[i]];
        Point2 b = vertices[faces[current_face_id].Vertex_ID[(i + 1) % 3]];
        if (!IsInfinite(faces[current_face_id].Face_ID[(i + 2) % 3]) && !Triangle2(a, b, point).TestTrigo()) {
            previous_face_id = current_face_id;
            current_face_id = faces[current_face_id].Face_ID[(i + 2) % 3];
            break;
        }
    }

    if (previous_face_id == 0) {
        return 0;
    }

    
    // init 3 vertices of current face
    Point2 A = vertices[faces[current_face_id].Vertex_ID[0]];
    Point2 B = vertices[faces[current_face_id].Vertex_ID[1]];
    Point2 C = vertices[faces[current_face_id].Vertex_ID[2]];


    bool test_intersect;
    while(!IsInfinite(current_face_id) && !Triangle2(A, B, C).Contains(point)){  // tant qu'on est en dehors du triangle

        for(uint i = 0; i<3; i++){
            if(faces[current_face_id].Face_ID[i] == previous_face_id){
                test_intersect = Triangle2(vertices[faces[current_face_id].Vertex_ID[i]],
                                           vertices[faces[current_face_id].Vertex_ID[(i + 2) % 3]],
                                           point).TestTrigo();
                previous_face_id = current_face_id;
                if(test_intersect){
                    current_face_id = faces[current_face_id].Face_ID[(i + 1) % 3];
                    break;
                }else{
                    current_face_id = faces[current_face_id].Face_ID[(i + 2) % 3];
                    break;
                }
            }
        }

        A = vertices[faces[current_face_id].Vertex_ID[0]];
        B = vertices[faces[current_face_id].Vertex_ID[1]];
        C = vertices[faces[current_face_id].Vertex_ID[2]];
    }

    return current_face_id;
}

bool TopoMesh2D::AddPoint(const Point2& new_point, bool keepDelaunay) {
    uint face_id = GetFace(new_point);

    if (IsInfinite(face_id)) {  // outsider point
        //std::cerr << "Point is out of the current convex shape of the mesh.\n";
        AddOutsiderPoint(new_point, keepDelaunay);

        return false;
    } else {
        if (keepDelaunay) SplitFaceDelaunay(face_id, new_point);
        else SplitFace(face_id, new_point);

        return true;
    }
}


void TopoMesh2D::SplitFaceDelaunay(uint face_id, const Point2& new_point) {

    // add ne point in the vertices list
    vertices.push_back(Vertex2(new_point, face_id));
    uint new_vertex_id = vertices.size() - 1;

    // keep IDs of the neighbour faces that will later be updated
    uint update_face_id_2 = faces[face_id].Face_ID[0];
    uint update_face_id_3 = faces[face_id].Face_ID[1];
    uint vertex_face_id_2 = faces[face_id].Vertex_ID[2];

    // update the split face to become one the 3 new faces
    faces[face_id].Vertex_ID[2] = new_vertex_id;
    uint new_face_id_1 = faces.size();
    uint new_face_id_2 = new_face_id_1 + 1;
    faces[face_id].Face_ID[0] = new_face_id_1;
    faces[face_id].Face_ID[1] = new_face_id_2;

    // update of the chosen vertex of the split face
    vertices[vertex_face_id_2].Face_ID = new_face_id_1;

    // update the neighbour faces
    for(uint i = 0; i < 3; i++){
        if(faces[update_face_id_2].Face_ID[i] == face_id){
            faces[update_face_id_2].Face_ID[i] = new_face_id_1;
        }
        if(faces[update_face_id_3].Face_ID[i] == face_id){
            faces[update_face_id_3].Face_ID[i] = new_face_id_2;
        }
    }

    // add the 2 other new faces
    uint temp_vertices_ID[3];
    uint temp_faces_ID[3];

    temp_vertices_ID[0] = faces[face_id].Vertex_ID[1];
    temp_vertices_ID[1] = vertex_face_id_2;
    temp_vertices_ID[2] = new_vertex_id;
    temp_faces_ID[0] = new_face_id_2;
    temp_faces_ID[1] = face_id;
    temp_faces_ID[2] = update_face_id_2;
    faces.push_back(Face2(temp_vertices_ID, temp_faces_ID));
    ReOrderIDs(faces.size() - 1, 0);

    temp_vertices_ID[0] = vertex_face_id_2;
    temp_vertices_ID[1] = faces[face_id].Vertex_ID[0];
    temp_vertices_ID[2] = new_vertex_id;
    temp_faces_ID[0] = face_id;
    temp_faces_ID[1] = new_face_id_1;
    temp_faces_ID[2] = update_face_id_3;
    faces.push_back(Face2(temp_vertices_ID, temp_faces_ID));
    ReOrderIDs(faces.size() - 1, 0);

    // get modified edges to feed PartialLawson
    std::queue<std::pair<uint, uint>> init_LD;
    std::vector<uint> nei_faces = GetFacesFromVertex(new_vertex_id);
    for(uint nei_face_id : nei_faces){
        for(uint i = 0; i < 3; i++){
            if(faces[nei_face_id].Vertex_ID[i] == new_vertex_id){
                init_LD.push(std::make_pair(nei_face_id, faces[nei_face_id].Face_ID[i]));
            }
        }
    }

    PartialLawson(init_LD);
}

void TopoMesh2D::AddOutsiderPoint(const Point2& new_point, bool keepDelauany) {

    std::vector<uint> all_infinite_faces = GetFacesFromVertex(0);
    all_infinite_faces.insert(all_infinite_faces.end(), all_infinite_faces.begin(), all_infinite_faces.end());
    uint nb_inf_faces = all_infinite_faces.size();

    bool met_false = false;  // did we met non visible edges ?
    bool did_split = false;  // did we split the first face ?

    for (uint inf_face_id : all_infinite_faces) {
        uint vertex_id_1 = faces[inf_face_id].Vertex_ID[1];
        uint vertex_id_2 = faces[inf_face_id].Vertex_ID[2];
        bool visibility_test = Triangle2(vertices[vertex_id_1], vertices[vertex_id_2], new_point).TestTrigo();

        if (visibility_test && !met_false) continue;
        if (!visibility_test && !did_split) {
            met_false = true;
            continue;
        }
        if (!visibility_test && did_split) break;
        if (visibility_test && met_false && !did_split) {  // do the split
            uint in_face_id = faces[inf_face_id].Face_ID[0];
            SplitFace(inf_face_id, new_point);
            did_split = true;
            // change face_id to detect edge point
            // uint new_vertex_id = vertices.size() - 1;
            // uint new_face_id = faces.size() - 2;
            // if (IsInfinite(inf_face_id)) {
            //     vertices[new_vertex_id].Face_ID = inf_face_id;
            //     continue;
            // }
            // if (IsInfinite(new_face_id)) {
            //     vertices[new_vertex_id].Face_ID = new_face_id;
            //     continue;
            // }
            // vertices[new_vertex_id].Face_ID = new_face_id + 1;
            continue;
        }
        if (visibility_test && met_false && did_split)
            FlipEdge(inf_face_id, faces[inf_face_id].Face_ID[2]);
    }

    if (keepDelauany) {
        std::queue<std::pair<uint, uint>> init_LD;
        uint new_v_id = vertices.size() - 1;
        std::vector<uint> new_nei_f_ids = GetFacesFromVertex(new_v_id);

        for (uint f_id : new_nei_f_ids) {
            if (IsInfinite(f_id)) continue;
            for (uint i = 0; i < 3; i++) {
                if (faces[f_id].Vertex_ID[i] == new_v_id) {
                    init_LD.push(std::make_pair(f_id, faces[f_id].Face_ID[i]));
                    break;
                }
            }
        }

        PartialLawson(init_LD);
    }
}


// EOF