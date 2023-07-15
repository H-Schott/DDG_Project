#include "topo_mesh_3D.hpp"
#include "mesh.hpp"

#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"


TopoMesh3D::TopoMesh3D(const Mesh& mesh) {
    vertices.clear();
    faces.clear();
    
    std::unordered_map<glm::vec3, unsigned int> vertices_map;

    // process all faces
    int nb_faces = int(mesh.indices.size() / 3);
    for (int i = 0; i < nb_faces; i++) {
        std::vector<unsigned int> vid;
        std::vector<unsigned int> fid = {0, 0, 0};
        
        // process the 3 vertices
        for (int j = 0; j < 3; j++) {
            glm::vec3 pos(1, 1, 1);// = mesh.vertices[mesh.indices[3 * i + j]].Position;
            // check if vertice j has never been seen
            if (!vertices_map.count(pos)) {
                unsigned int id = vertices_map.size();
                vertices_map[pos] = id;
                Point p = Point(pos.x, pos.y, pos.z);
                Vertex3 v = Vertex3(p, faces.size());
            }
            vid.push_back(vertices_map[pos]);
        }
        
        Face3 f = Face3(vid, fid);
        faces.push_back(f);
    }
    
}



TopoMesh3D::TopoMesh3D() {}

TopoMesh3D::TopoMesh3D(const char* file_name) {
    std::ifstream ifs;
    ifs.open(file_name);
    if(ifs.bad()) {
        std::cout << "Impossible d'ouvrir le fichier " << file_name << " en lecture \n";
        exit(1);
    }
    double temp;

    // nombres de sommets et faces a recuperer
    ifs >> temp;
    unsigned int nb_vertices = temp;
    ifs >> temp;
    unsigned int nb_faces = temp;

    double x;
    double y;
    double z;
    std::vector<Point> data_points(nb_vertices, Point());
    for(unsigned int i = 0; i < nb_vertices; i++){
        ifs >> temp;
        x = temp;
        ifs >> temp;
        y = temp;
        ifs >> temp;
        z = temp;
        data_points[i] = Point(x, y, z);
    }

    std::vector<std::vector<unsigned int>> data_faces(nb_faces, std::vector<unsigned int>(3, 0));
    for(unsigned int i = 0; i < nb_faces; i++){
        ifs >> temp;
        for(unsigned int j = 0; j < 3; j++){
            ifs >> temp;
            data_faces[i][j] = temp;
        }
    }
    ifs.close();

    std::vector<unsigned int> empty_fid(3, 0);
    // init faces du mesh cousu
    for(unsigned int i = 0; i < nb_faces; i++){
        faces.push_back(Face3(data_faces[i], empty_fid));
    }
    // init vertices du mesh cousu
    for(unsigned int i = 0; i < nb_vertices; i++){
        vertices.push_back(Vertex3(data_points[i], 0));
    }

    // 2 indices de points en cle, 1 indice de face + 1 indice de sommet en valeur
    std::map<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int>> dico;
    std::map<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int>>::iterator test_in;

    std::pair<unsigned int, unsigned int> arete_real;  // arete dans le sens ou on l observe
    std::pair<unsigned int, unsigned int> arete_test;  // arete dans l autre sens pour tester sa presence dans le dico

    for(unsigned int i_face = 1; i_face < nb_faces + 1; i_face++){

        for(unsigned int arete_id = 0; arete_id < 3; arete_id++) {
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

TopoMesh3D::TopoMesh3D(Point p1, Point p2, Point p3, Point p4) {
    
}

void TopoMesh3D::ClearAndInit(Point p1, Point p2, Point p3, Point p4) {

}


double TopoMesh3D::Area() const {
    double area = 0;

    unsigned int nb_faces = faces.size();
    for (unsigned int i = 0; i < nb_faces; i++) {
        area += faces[i].ToTriangle(this).Area();
    }
    
    return area;
}

unsigned int TopoMesh3D::GetNbPoint() const {
    return vertices.size();
}

unsigned int TopoMesh3D::GetNbTriangle() const {
    return faces.size() - 1; // dont count the first fake face
}


Vertex3 TopoMesh3D::Vertex(unsigned int ID) const {
    return vertices[ID];
}

Face3 TopoMesh3D::Face(unsigned int ID) const {
    return faces[ID];
}



std::vector<unsigned int> TopoMesh3D::GetFacesFromVertex(unsigned int vertex_id) const {

    std::vector<unsigned int> neighbour_faces;

    unsigned int begin_face_id = vertices[vertex_id].Face_ID;

    unsigned int relativ_vertex_id = 4;
    for(unsigned int i = 0; i<3; i++){
        if(faces[begin_face_id].Vertex_ID[i] == vertex_id){
            relativ_vertex_id = i;
        }
    }

    // premier sommet
    relativ_vertex_id = (relativ_vertex_id + 1) % 3;
    unsigned int begin_vertex_id = faces[begin_face_id].Vertex_ID[relativ_vertex_id];
    unsigned int current_vertex_id;
    unsigned int current_face_id = begin_face_id;
    unsigned int previous_face_id;

    do{
        neighbour_faces.push_back(current_face_id);
        previous_face_id = current_face_id;
        current_face_id = faces[previous_face_id].Face_ID[relativ_vertex_id];
        relativ_vertex_id = (relativ_vertex_id + 1) % 3;
        current_vertex_id = faces[previous_face_id].Vertex_ID[relativ_vertex_id];

        relativ_vertex_id = 4;
        for(unsigned int i = 0; i<3; i++){
            if(faces[current_face_id].Vertex_ID[i] == current_vertex_id){
                relativ_vertex_id = i;
            }
        }

        bool ok = false;
        for(unsigned int i = 0; i < 3; i++){
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

std::vector<unsigned int> TopoMesh3D::GetVerticesFromVertex(unsigned int vertex_id) const {
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



void TopoMesh3D::DebugLog() const {
    unsigned int nb_vertices = vertices.size();
    unsigned int nb_faces = faces.size();

    std::cout << nb_vertices << " vertices.\n";
    for (unsigned int i = 0; i < nb_vertices; i++) {
        std::cout << i << " : " << vertices[i] << " : " << vertices[i].Face_ID << std::endl;
    }

    std::cout << nb_faces << " faces.\n";
    for (unsigned int i = 0; i < nb_faces; i++) {
        std::cout << i << " : ";
        std::cout << faces[i].Vertex_ID[0] << " " << faces[i].Vertex_ID[1] << " " << faces[i].Vertex_ID[2] << " : ";
        std::cout << faces[i].Face_ID[0] << " " << faces[i].Face_ID[1] << " " << faces[i].Face_ID[2] << std::endl;
    }
    
}



//EOF