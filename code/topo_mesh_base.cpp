#include "topo_mesh_base.hpp"

TopoMeshBase::TopoMeshBase() {}

TopoMeshBase::TopoMeshBase(const char* file_name) {
    std::ifstream ifs;
    ifs.open(file_name);
    if(ifs.bad()) {
        std::cout << "Impossible d'ouvrir le fichier " << file_name << " en lecture \n";
        exit(1);
    }
    double temp;

    // nombres de sommets et faces a recuperer
    ifs >> temp;
    uint nb_vertices = temp;
    ifs >> temp;
    uint nb_faces = temp;

    double x;
    double y;
    std::vector<Point2> data_points(nb_vertices, Point2());
    for(uint i = 0; i < nb_vertices; i++){
        ifs >> temp;
        x = temp;
        ifs >> temp;
        y = temp;
        data_points[i] = Point2(x, y);
    }

    std::vector<std::vector<uint>> data_faces(nb_faces, std::vector<uint>(3, 0));
    for(uint i = 0; i < nb_faces; i++){
        ifs >> temp;
        for(uint j = 0; j < 3; j++){
            ifs >> temp;
            data_faces[i][j] = temp;
        }
    }
    ifs.close();

    std::vector<uint> empty_fid(3, 0);
    // init faces du mesh cousu
    for(uint i = 0; i < nb_faces; i++){
        faces.push_back(Face2(data_faces[i], empty_fid));
    }
    // init vertices du mesh cousu
    for(uint i = 0; i < nb_vertices; i++){
        vertices.push_back(Vertex2(data_points[i], 0));
    }

    // 2 indices de points en cle, 1 indice de face + 1 indice de sommet en valeur
    std::map<std::pair<uint, uint>, std::pair<uint, uint>> dico;
    std::map<std::pair<uint, uint>, std::pair<uint, uint>>::iterator test_in;

    std::pair<uint, uint> arete_real;  // arete dans le sens ou on l observe
    std::pair<uint, uint> arete_test;  // arete dans l autre sens pour tester sa presence dans le dico

    for(uint i_face = 1; i_face < nb_faces + 1; i_face++){

        for(uint arete_id = 0; arete_id < 3; arete_id++) {
            arete_real = std::make_pair(faces[i_face].Vertex_ID[arete_id], faces[i_face].Vertex_ID[(arete_id + 1)%3]);
            arete_test = std::make_pair(faces[i_face].Vertex_ID[(arete_id + 1)%3], faces[i_face].Vertex_ID[arete_id]);

            test_in = dico.find(arete_test);
            if(test_in == dico.end()){  //  pas dans la map
                dico[arete_real] = std::make_pair(i_face, (arete_id + 2)%3);
            }else{                      //  deja dans la map
                faces[i_face].Face_ID[(arete_id + 2)%3] = test_in->second.first;
                faces[test_in->second.first].Face_ID[test_in->second.second] = i_face;
            }
            // test points
            if(vertices[arete_real.first].Face_ID == 0){
                vertices[arete_real.first].Face_ID = i_face;
            }
            if(vertices[arete_real.second].Face_ID == 0){
                vertices[arete_real.second].Face_ID = i_face;
            }
        }
    }

    std::cout << vertices.size() << " vertices.\n";
    std::cout << faces.size() << " faces.\n";
}

TopoMeshBase::TopoMeshBase(Point2 p1, Point2 p2, Point2 p3) {
    bool order = Triangle2(p1, p2, p3).TestTrigo();

    if(!order) {
        Point2 temp = p2;
        p2 = p3;
        p3 = temp;
    }

    vertices.emplace_back(p1, 3);  // 1
    vertices.emplace_back(p2, 2);  // 2
    vertices.emplace_back(p3, 2);  // 3
    faces.emplace_back(1, 2, 3, 2, 3, 4);  // 1
    faces.emplace_back(0, 3, 2, 1, 4, 3);  // 2
    faces.emplace_back(0, 1, 3, 1, 2, 4);  // 3
    faces.emplace_back(0, 2, 1, 1, 3, 2);  // 4
    vertices[0].Face_ID = 2;
}

void TopoMeshBase::ClearAndInit(Point2 p1, Point2 p2, Point2 p3) {
    vertices = {Vertex2()};
    faces = {Face2()};

    bool order = Triangle2(p1, p2, p3).TestTrigo();

    if(!order) {
        Point2 temp = p2;
        p2 = p3;
        p3 = temp;
    }

    vertices.emplace_back(p1, 3);  // 1
    vertices.emplace_back(p2, 2);  // 2
    vertices.emplace_back(p3, 2);  // 3
    faces.emplace_back(1, 2, 3, 2, 3, 4);  // 1
    faces.emplace_back(0, 3, 2, 1, 4, 3);  // 2
    faces.emplace_back(0, 1, 3, 1, 2, 4);  // 3
    faces.emplace_back(0, 2, 1, 1, 3, 2);  // 4
    vertices[0].Face_ID = 2;
}


double TopoMeshBase::Area() const {
    double area = 0;

    uint nb_faces = faces.size();
    for (uint i = 0; i < nb_faces; i++) {
        area += faces[i].ToTriangle(this).Area();
    }
    
    return area;
}

uint TopoMeshBase::GetNbPoint() const {
    return vertices.size();
}

uint TopoMeshBase::GetNbTriangle() const {
    return faces.size() - 1; // dont count the first fake face
}


Vertex2 TopoMeshBase::Vertex(uint ID) const {
    return vertices[ID];
}

Face2 TopoMeshBase::Face(uint ID) const {
    return faces[ID];
}



std::vector<uint> TopoMeshBase::GetFacesFromVertex(uint vertex_id) const {

    std::vector<uint> neighbour_faces;

    uint begin_face_id = vertices[vertex_id].Face_ID;

    uint relativ_vertex_id = 4;
    for(uint i = 0; i<3; i++){
        if(faces[begin_face_id].Vertex_ID[i] == vertex_id){
            relativ_vertex_id = i;
        }
    }

    // premier sommet
    relativ_vertex_id = (relativ_vertex_id + 1) % 3;
    uint begin_vertex_id = faces[begin_face_id].Vertex_ID[relativ_vertex_id];
    uint current_vertex_id;
    uint current_face_id = begin_face_id;
    uint previous_face_id;

    do{
        neighbour_faces.push_back(current_face_id);
        previous_face_id = current_face_id;
        current_face_id = faces[previous_face_id].Face_ID[relativ_vertex_id];
        relativ_vertex_id = (relativ_vertex_id + 1) % 3;
        current_vertex_id = faces[previous_face_id].Vertex_ID[relativ_vertex_id];

        relativ_vertex_id = 4;
        for(uint i = 0; i<3; i++){
            if(faces[current_face_id].Vertex_ID[i] == current_vertex_id){
                relativ_vertex_id = i;
            }
        }

        bool ok = false;
        for(uint i = 0; i < 3; i++){
            if(faces[current_face_id].Vertex_ID[i] == vertex_id){
                ok = true;
                break;
            }
        }
        if(!ok){
            printf("neighbours errors\n");
        }

    }while(begin_vertex_id != current_vertex_id);

    return neighbour_faces;
}

std::vector<uint> TopoMeshBase::GetVerticesFromVertex(uint vertex_id) const {
    std::vector<unsigned int> all_neighbours;

    unsigned int i_begin_face = vertices[vertex_id].Face_ID;

    unsigned int i_sommet_relativ = 4;
    for(unsigned int i = 0; i<3; i++){
        if(faces[i_begin_face].Vertex_ID[i] == vertex_id){
            i_sommet_relativ = i;
        }
    }

    // premier sommet
    i_sommet_relativ = (i_sommet_relativ + 1)%3;
    unsigned int i_begin_sommet = faces[i_begin_face].Vertex_ID[i_sommet_relativ];
    unsigned int i_current_sommet = i_begin_sommet;
    unsigned int i_current_face = i_begin_face;
    unsigned int i_precedent_face;

    do{
        all_neighbours.push_back(i_current_sommet);
        i_precedent_face = i_current_face;
        i_current_face = faces[i_precedent_face].Face_ID[i_sommet_relativ];
        i_sommet_relativ = (i_sommet_relativ + 1) % 3;
        i_current_sommet = faces[i_precedent_face].Vertex_ID[i_sommet_relativ];
        i_sommet_relativ = 4;
        for(unsigned int i = 0; i<3; i++){
            if(faces[i_current_face].Vertex_ID[i] == i_current_sommet){
                i_sommet_relativ = i;
            }
        }

    }while(i_begin_sommet != i_current_sommet);  // on stop si on revient au i_begin_somet

    return all_neighbours;
}



void TopoMeshBase::DebugLog() const {
    uint nb_vertices = vertices.size();
    uint nb_faces = faces.size();

    std::cout << nb_vertices << " vertices.\n";
    for (uint i = 0; i < nb_vertices; i++) {
        std::cout << i << " : " << vertices[i] << " : " << vertices[i].Face_ID << std::endl;
    }

    std::cout << nb_faces << " faces.\n";
    for (uint i = 0; i < nb_faces; i++) {
        std::cout << i << " : ";
        std::cout << faces[i].Vertex_ID[0] << " " << faces[i].Vertex_ID[1] << " " << faces[i].Vertex_ID[2] << " : ";
        std::cout << faces[i].Face_ID[0] << " " << faces[i].Face_ID[1] << " " << faces[i].Face_ID[2] << std::endl;
    }
    
}


// PRIVATE METHOD

void TopoMeshBase::ReOrderIDs(uint face_id, uint order) {
    // if order is zero, reorder to get the infinite vertex first
    if (order == 0) {
        if (faces[face_id].Vertex_ID[1] == 0) order = 1;
        if (faces[face_id].Vertex_ID[2] == 0) order = 2;
    }

    // reorder to get the Vertex_ID[order] at Vertex_ID[0]
    uint temp_f_id = faces[face_id].Face_ID[0];
    uint temp_v_id = faces[face_id].Vertex_ID[0];
    if (order == 1) {
        faces[face_id].Face_ID[0] = faces[face_id].Face_ID[1];
        faces[face_id].Vertex_ID[0] = faces[face_id].Vertex_ID[1];
        faces[face_id].Face_ID[1] = faces[face_id].Face_ID[2];
        faces[face_id].Vertex_ID[1] = faces[face_id].Vertex_ID[2];
        faces[face_id].Face_ID[2] = temp_f_id;
        faces[face_id].Vertex_ID[2] = temp_v_id;
    }

    if (order == 2) {
        faces[face_id].Face_ID[0] = faces[face_id].Face_ID[2];
        faces[face_id].Vertex_ID[0] = faces[face_id].Vertex_ID[2];
        faces[face_id].Face_ID[2] = faces[face_id].Face_ID[1];
        faces[face_id].Vertex_ID[2] = faces[face_id].Vertex_ID[1];
        faces[face_id].Face_ID[1] = temp_f_id;
        faces[face_id].Vertex_ID[1] = temp_v_id;
    }

}

void TopoMeshBase::SplitFace(uint face_id, const Point2& new_point) {

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
}

void TopoMeshBase::FlipEdge(uint face_id_1, uint face_id_2) {
    // on recupere les indices relatifs des 4 faces adjacentes au
    // parallelogramme dans lequel on effectue le flip
    uint face_id_1_1;
    uint face_id_1_2;
    uint face_id_2_1;
    uint face_id_2_2;

    for(uint i = 0; i < 3; i++){
        if(faces[face_id_1].Face_ID[i] == face_id_2){
            face_id_1_1 = (i+1)%3;
            face_id_1_2 = (i+2)%3;
        }
        if(faces[face_id_2].Face_ID[i] == face_id_1){
            face_id_2_1 = (i+1)%3;
            face_id_2_2 = (i+2)%3;
        }
    }

    // on recupere les indices des sommets de l ancienne arete
    uint i_sommet_A = faces[face_id_1].Vertex_ID[face_id_1_1];
    uint i_sommet_B = faces[face_id_1].Vertex_ID[face_id_1_2];

    // on recupere les indices des sommets de l arete a construire
    uint i_sommet_C = faces[face_id_1].Vertex_ID[(face_id_1_2 + 1) % 3];
    uint i_sommet_D = faces[face_id_2].Vertex_ID[(face_id_2_2 + 1) % 3];

    // on change les indices de sommets des faces 1 et 2 pour flip l arete
    faces[face_id_1].Vertex_ID[face_id_1_2] = i_sommet_D;
    faces[face_id_2].Vertex_ID[face_id_2_2] = i_sommet_C;

    // on change les indices de faces pour les faces adjacentes
    // au parallelogramme
    for(unsigned int i = 0; i < 3; i++){
        if(faces[faces[face_id_1].Face_ID[face_id_1_1]].Vertex_ID[i] == i_sommet_B){
            faces[faces[face_id_1].Face_ID[face_id_1_1]].Face_ID[(i + 1) % 3] = face_id_2;
        }
        if(faces[faces[face_id_2].Face_ID[face_id_2_1]].Vertex_ID[i] == i_sommet_A){
            faces[faces[face_id_2].Face_ID[face_id_2_1]].Face_ID[(i + 1)%3] = face_id_1;
        }
    }


    // on change les faces adjacentes pour face_id_1 et face_id_2
    faces[face_id_1].Face_ID[(face_id_1_2 + 1) % 3] = faces[face_id_2].Face_ID[face_id_2_1];
    faces[face_id_2].Face_ID[(face_id_2_2 + 1) % 3] = faces[face_id_1].Face_ID[face_id_1_1];
    faces[face_id_1].Face_ID[(face_id_1_2 + 2) % 3] = face_id_2;
    faces[face_id_2].Face_ID[(face_id_2_2 + 2) % 3] = face_id_1;


    // on change les indices de faces associes au 2 points
    // du parallelogramme qui formaient l ancienne arete
    vertices[i_sommet_A].Face_ID = face_id_1;
    vertices[i_sommet_B].Face_ID = face_id_2;
}




//EOF