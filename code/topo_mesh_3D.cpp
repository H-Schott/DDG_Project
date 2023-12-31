#include "topo_mesh_3D.hpp"
#include "mesh.hpp"

#include <unordered_map>


// so the std::unordered_map can handle the glm classes
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#include <Eigen/Dense>

TopoMesh3D::TopoMesh3D(const Mesh& mesh) {
    vertices.clear();
    vertices.push_back(Vertex3()); //  dummy vertex
    faces.clear();
    faces.push_back(Face3());  // dummy face
    
    std::unordered_map<glm::vec3, unsigned int> vertices_map;
    std::map<std::pair<unsigned int, unsigned int>, unsigned int> edge_face_map;

    // process all faces
    int nb_faces = int(mesh.indices.size() / 3);
    for (int i = 0; i < nb_faces; i++) {
        
        std::vector<unsigned int> vid;
        std::vector<unsigned int> fid = {0, 0, 0};
        
        // process the 3 vertices
        for (int j = 0; j < 3; j++) {
            glm::vec3 pos = mesh.vertices[mesh.indices[3 * i + j]].Position;
            // check if vertice j has never been seen
            if (!vertices_map.count(pos)) {
                unsigned int id = vertices_map.size() + 1;
                vertices_map[pos] = id;
                Point p = Point(pos.x, pos.y, pos.z);
                Vertex3 v = Vertex3(p, faces.size());
                vertices.push_back(v);
            }
            vid.push_back(vertices_map[pos]);
        }
        Face3 f = Face3(vid, fid);
        faces.push_back(f);

        // sewing step
        for (int j = 0; j < 3; j++) {
            std::pair<unsigned int, unsigned int> edge = std::make_pair(vid[j], vid[(j + 1) % 3]);
            // never saw this edge yet
            if (!edge_face_map.count(edge)) {
                std::swap(edge.first, edge.second);
                edge_face_map[edge] = faces.size() - 1;
            }
            // already this edge, we can sew
            else {
                // current face
                faces[faces.size() - 1].Face_ID[(j + 2) % 3] = edge_face_map[edge];
                // old face
                Face3& old_face = faces[edge_face_map[edge]];
                for (int k = 0; k < 3; k++) {
                    if (edge.first != old_face.Vertex_ID[k] && edge.second != old_face.Vertex_ID[k]) {
                        old_face.Face_ID[k] = faces.size() - 1;
                        break;
                    }
                }
            }
        }
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


Vertex3 TopoMesh3D::GetVertex(unsigned int ID) const {
    return vertices[ID];
}

Face3 TopoMesh3D::GetFace(unsigned int ID) const {
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
        if (current_face_id == 0) {
            neighbour_faces.clear();
            return neighbour_faces;
        }
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

    //std::cout << i_sommet_relativ << std::endl;

    // premier sommet
    i_sommet_relativ = (i_sommet_relativ + 1) % 3;
    unsigned int i_begin_sommet = faces[i_begin_face].Vertex_ID[i_sommet_relativ];
    unsigned int i_current_sommet = i_begin_sommet;
    unsigned int i_current_face = i_begin_face;
    unsigned int i_precedent_face;

    do{
        all_neighbours.push_back(i_current_sommet);
        i_precedent_face = i_current_face;
        i_current_face = faces[i_precedent_face].Face_ID[i_sommet_relativ];
        if (i_current_face == 0) {
            all_neighbours.clear();
            return all_neighbours;
        }
        i_sommet_relativ = (i_sommet_relativ + 1) % 3;
        i_current_sommet = faces[i_precedent_face].Vertex_ID[i_sommet_relativ];
        i_sommet_relativ = 4;
        for(unsigned int i = 0; i<3; i++){
            if(faces[i_current_face].Vertex_ID[i] == i_current_sommet){
                i_sommet_relativ = i;
            }
        }

        //std::cout << i_sommet_relativ << std::endl;
    }while(i_begin_sommet != i_current_sommet);  // on stop si on revient au i_begin_somet

    return all_neighbours;
}

std::vector<unsigned int> TopoMesh3D::GetValence() const {
    std::vector<unsigned int> valences;

    //std::vector<unsigned int> val_v = GetVerticesFromVertex(7855);

    //for (int i = 0; i < 0; i++) {
    for (int i = 0; i < vertices.size(); i++) {
        std::vector<unsigned int> val_v = GetVerticesFromVertex(i);
        unsigned int val = val_v.size();
        while (val >= valences.size()) {
            valences.push_back(0);
        }
        valences[val] += 1;
    }

    return valences;
}

double TopoMesh3D::Laplacian(unsigned int vertex_id, unsigned int component_id) const {
    double laplacian = 0;

    std::vector<unsigned int> n_v_ids = GetVerticesFromVertex(vertex_id);
    if (n_v_ids.size() == 0) return 0.;

    // laplacian vars
    double du;
    double cot_alpha;
    double cot_beta;
    double A_i = 0;

    unsigned int i_alpha_sommet;
    unsigned int i_beta_sommet;
    unsigned int i_j_sommet;

    // on parcourt les sommets voisins
    for (unsigned int j = 0; j < n_v_ids.size(); j++) {
        i_j_sommet = n_v_ids[j];
        i_alpha_sommet = n_v_ids[(j + n_v_ids.size() - 1) % n_v_ids.size()];
        i_beta_sommet = n_v_ids[(j + 1) % n_v_ids.size()];

        // du
        switch (component_id) {
        case 0:
            du = vertices[i_j_sommet].x - vertices[vertex_id].x;
            break;
        case 1:
            du = vertices[i_j_sommet].y - vertices[vertex_id].y;
            break;
        case 2:
            du = vertices[i_j_sommet].z - vertices[vertex_id].z;
            break;
        }

        // cot_alpha
        cot_alpha = CoTan(vertices[vertex_id], vertices[i_alpha_sommet], vertices[i_j_sommet]);

        // cot_beta
        cot_beta = CoTan(vertices[vertex_id], vertices[i_beta_sommet], vertices[i_j_sommet]);

        laplacian += (cot_beta + cot_alpha) * du;

    }

    // calcul de A_i

    std::vector<unsigned int> i_neighbours_face = GetFacesFromVertex(vertex_id);

    for (unsigned int& i : i_neighbours_face) {
        A_i += faces[i].ToTriangle(this).Area();
    }

    laplacian = 3. / (2. * A_i) * laplacian;

    return laplacian;
}

Vector TopoMesh3D::Laplacian(unsigned int vertex_id) const {
    return Vector(Laplacian(vertex_id, 0), Laplacian(vertex_id, 1), Laplacian(vertex_id, 2));
}

std::vector<double> TopoMesh3D::LaplacianNorms(bool normalized) const {
    std::vector<double> laps;
    double min_lap = Laplacian(1).Norm();
    double max_lap = 0.;
    for (int i = 1; i < vertices.size(); i++) {
        Vector lap = Laplacian(i);
        double lap_n = lap.Norm();
        laps.push_back(lap_n);
        if (lap_n < min_lap) min_lap = lap_n;
        if (lap_n > max_lap) max_lap = lap_n;
    }

    max_lap *= 0.3;

    if (normalized && max_lap > 0.) {
        for (int i = 0; i < laps.size(); i++) {
            laps[i] = (laps[i] - min_lap) / (max_lap - min_lap);
        }
    }

    std::vector<double> laps_3;

    for (int i = 1; i < faces.size(); i++) {
        for (int j = 0; j < 3; j++) {
            laps_3.push_back(laps[faces[i].Vertex_ID[j] - 1]);
        }
    }

    return laps_3;
}

std::vector<double> TopoMesh3D::LaplacianNormsMatrix(bool normalized) const {
    std::vector<double> laps;

    Eigen::VectorXd coo_x(vertices.size() - 1);
    Eigen::VectorXd coo_y(vertices.size() - 1);
    Eigen::VectorXd coo_z(vertices.size() - 1);
    for (int i = 1; i < vertices.size(); i++) {
        coo_x(i - 1) = vertices[i].x;
        coo_y(i - 1) = vertices[i].y;
        coo_z(i - 1) = vertices[i].z;
    }

    Eigen::SparseMatrix<double> lapMat = GetLaplacianMatrix();

    Eigen::VectorXd lap_x = lapMat * coo_x;
    Eigen::VectorXd lap_y = lapMat * coo_y;
    Eigen::VectorXd lap_z = lapMat * coo_z;


    double min_lap = Vector(lap_x[0], lap_y[0], lap_z[0]).Norm();
    double max_lap = 0.;
    for (int i = 0; i < vertices.size() - 1; i++) {
        Vector v(lap_x[i], lap_y[i], lap_z[i]);
        double lap_n = v.Norm();
        laps.push_back(lap_n);

        if (lap_n < min_lap) min_lap = lap_n;
        if (lap_n > max_lap) max_lap = lap_n;
    }

    max_lap *= 0.3;

    if (normalized && max_lap > 0.) {
        for (int i = 0; i < laps.size(); i++) {
            laps[i] = (laps[i] - min_lap) / (max_lap - min_lap);
        }
    }

    // x3 for gl mesh coherency
    std::vector<double> laps_3;
    for (int i = 1; i < faces.size(); i++) {
        for (int j = 0; j < 3; j++) {
            laps_3.push_back(laps[faces[i].Vertex_ID[j] - 1]);
        }
    }

    return laps_3;
}

std::vector<Vector> TopoMesh3D::LaplaciansMatrix(bool normalized) const {
    std::vector<Vector> laps;

    Eigen::VectorXd coo_x(vertices.size() - 1);
    Eigen::VectorXd coo_y(vertices.size() - 1);
    Eigen::VectorXd coo_z(vertices.size() - 1);
    for (int i = 1; i < vertices.size(); i++) {
        coo_x(i - 1) = vertices[i].x;
        coo_y(i - 1) = vertices[i].y;
        coo_z(i - 1) = vertices[i].z;
    }

    Eigen::SparseMatrix<double> lapMat = GetLaplacianMatrix();

    Eigen::VectorXd lap_x = lapMat * coo_x;
    Eigen::VectorXd lap_y = lapMat * coo_y;
    Eigen::VectorXd lap_z = lapMat * coo_z;

    for (int i = 0; i < vertices.size() - 1; i++) {
        Vector v(lap_x[i], lap_y[i], lap_z[i]);
        laps.push_back(v.Normalized());
    }

    std::vector<Vector> laps_3;

    for (int i = 1; i < faces.size(); i++) {
        for (int j = 0; j < 3; j++) {
            laps_3.push_back(laps[faces[i].Vertex_ID[j] - 1]);
        }
    }

    return laps_3;
}

std::vector<Vector> TopoMesh3D::Laplacians(bool normalized) const {
    std::vector<Vector> laps;

    for (int i = 1; i < vertices.size(); i++) {
        Vector lap = Laplacian(i);
        if (normalized) laps.push_back(lap.Normalized());
        else laps.push_back(lap);
    }

    std::vector<Vector> laps_3;

    for (int i = 1; i < faces.size(); i++) {
        for (int j = 0; j < 3; j++) {
            laps_3.push_back(laps[faces[i].Vertex_ID[j] - 1]);
        }
    }

    return laps_3;
}


Vector TopoMesh3D::Gradient(unsigned int face_id, double f0, double f1, double f2) const {
    Vector grad;

    double f[3] = { f0, f1, f2 };

    Face3 face = faces[face_id];
    Vector normal = face.ToTriangle(this).Normal();
    for (int i = 0; i < 3; i++) {
        Vector Je = Cross(normal, face.ToTriangle(this).Edge(i));
        grad += f[i] * Je;
    }
    grad /= 2. * face.ToTriangle(this).Area();

    return grad;
}

std::vector<double> TopoMesh3D::GradientNormsZ(bool normalized) const {
    std::vector<double> grads;
    double min_grad = Gradient(1, vertices[faces[1].Vertex_ID[0]].x, vertices[faces[1].Vertex_ID[1]].x, vertices[faces[1].Vertex_ID[2]].x).Norm();
    double max_grad = 0.;
    for (int i = 1; i < faces.size(); i++) {
        Face3 face = faces[i];
        Vector grad = Gradient(i, vertices[face.Vertex_ID[0]].x, vertices[face.Vertex_ID[1]].x, vertices[face.Vertex_ID[2]].x);
        double grad_n = grad.Norm();
        grads.push_back(grad_n);
        if (grad_n < min_grad) min_grad = grad_n;
        if (grad_n > max_grad) max_grad = grad_n;
    }

    if (normalized && max_grad > 0.) {
        for (int i = 0; i < grads.size(); i++) {
            grads[i] = (grads[i] - min_grad) / (max_grad - min_grad);
        }
    }

    std::vector<double> grads_3;

    for (int i = 1; i < faces.size(); i++) {
        grads_3.push_back(grads[i]);
        grads_3.push_back(grads[i]);
        grads_3.push_back(grads[i]);
    }

    return grads_3;
}


Eigen::SparseMatrix<bool> TopoMesh3D::GetConnectivityMatrix() const {
    int dimension = vertices.size() - 1;
    Eigen::SparseMatrix<bool> conMat(dimension, dimension);

    for (int i = 1; i < dimension + 1; i++) {
        std::vector<unsigned int> iv_neis = GetVerticesFromVertex(i);
        for (int j = 0; j < iv_neis.size(); j++) {
            conMat.coeffRef(i - 1, iv_neis[j] - 1) = 1;
        }
    }

    return conMat;
}

Eigen::SparseMatrix<double> TopoMesh3D::GetGradientMatrix() const {
    int dim_vertices = vertices.size() - 1;
    int dim_faces = faces.size() - 1;
    Eigen::SparseMatrix<double> gradMat(dim_faces, dim_vertices);

    // add each face in the matrix
    for (int i = 1; i < dim_faces + 1; i++) {

        Face3 face = faces[i];
        double area_coeff = 1. / (2. * face.ToTriangle(this).Area());

        // add 3 vertices
        for (int j = 0; j < 3; j++) {
            //gradMat.coeffRef(i - 1, face.Vertex_ID[j] - 1) = area_coeff * ;
        }
    }

    return gradMat;
}

Eigen::SparseMatrix<double> TopoMesh3D::GetLaplacianMatrix() const {


    int dimension = vertices.size() - 1;
    std::vector<Eigen::Triplet<double>> triplets;

    for (int i = 1; i < dimension + 1; i++) {
        // area normalization
        double area_coeff = 0.;
        std::vector<unsigned int> i_neighbours_face = GetFacesFromVertex(i);
        for (unsigned int& i : i_neighbours_face) {
            area_coeff += faces[i].ToTriangle(this).Area();
        }
        area_coeff = 3. / (2. * area_coeff);
        

        std::vector<unsigned int> iv_neis = GetVerticesFromVertex(i);
        double total_cot_coeff = 0.;
        for (int j = 0; j < iv_neis.size(); j++) {
            unsigned int i_j_sommet = iv_neis[j];
            unsigned int i_alpha_sommet = iv_neis[(j + iv_neis.size() - 1) % iv_neis.size()];
            unsigned int i_beta_sommet = iv_neis[(j + 1) % iv_neis.size()];
            double cot_alpha = CoTan(vertices[i], vertices[i_alpha_sommet], vertices[i_j_sommet]);
            double cot_beta = CoTan(vertices[i], vertices[i_beta_sommet], vertices[i_j_sommet]);
            double cot_coeff = cot_alpha + cot_beta;

            total_cot_coeff += cot_coeff;

            triplets.push_back(Eigen::Triplet<double>(i - 1, iv_neis[j] - 1, - cot_coeff * area_coeff));
        }
        triplets.push_back(Eigen::Triplet<double>(i - 1, i - 1, total_cot_coeff * area_coeff));
    }

    Eigen::SparseMatrix<double> lapMat(dimension, dimension);
    lapMat.setFromTriplets(triplets.begin(), triplets.end());

    return lapMat;
}


void TopoMesh3D::Diffusion(double k) {
    for (int i = 1; i < vertices.size(); i++) {
        Vector lap = Laplacian(i);
        vertices[i].x += k * lap.x;
        vertices[i].y += k * lap.y;
        vertices[i].z += k * lap.z;
    }
}

void TopoMesh3D::Smooth() {
    for (int i = 1; i < vertices.size(); i++) {
        std::vector<unsigned int> v_nei_id = GetVerticesFromVertex(i);

        if (v_nei_id.size() == 0) continue;

        Vector v = vertices[i];

        for (int j = 0; j < v_nei_id.size(); j++) {
            v += vertices[v_nei_id[j]];
        }

        v /= (v_nei_id.size() + 1);

        vertices[i].x = v.x;
        vertices[i].y = v.y;
        vertices[i].z = v.z;
    }
}


void TopoMesh3D::DebugLog() const {
    unsigned int nb_vertices = vertices.size();
    unsigned int nb_faces = faces.size();

    std::cout << nb_vertices << " vertices.\n";
    //for (unsigned int i = 0; i < nb_vertices; i++) {
    for (unsigned int i = 0; i < 100; i++) {
        std::cout << i << " : " << vertices[i] << " : " << vertices[i].Face_ID << std::endl;
    }

    std::cout << nb_faces << " faces.\n";
    //for (unsigned int i = 0; i < nb_faces; i++) {
    for (unsigned int i = 0; i < 100; i++) {
        std::cout << i << " : ";
        std::cout << faces[i].Vertex_ID[0] << " " << faces[i].Vertex_ID[1] << " " << faces[i].Vertex_ID[2] << " : ";
        std::cout << faces[i].Face_ID[0] << " " << faces[i].Face_ID[1] << " " << faces[i].Face_ID[2] << std::endl;
    }
    
}


void TopoMesh3D::ReOrderIDs(unsigned int face_id, unsigned int order) {
    // if order is zero, reorder to get the infinite vertex first
    if (order == 0) {
        if (faces[face_id].Vertex_ID[1] == 0) order = 1;
        if (faces[face_id].Vertex_ID[2] == 0) order = 2;
    }

    // reorder to get the Vertex_ID[order] at Vertex_ID[0]
    unsigned int temp_f_id = faces[face_id].Face_ID[0];
    unsigned int temp_v_id = faces[face_id].Vertex_ID[0];
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

void TopoMesh3D::SplitFace(unsigned int face_id, const Point& new_point) {

    // add ne point in the vertices list
    vertices.push_back(Vertex3(new_point, face_id));
    unsigned int new_vertex_id = vertices.size() - 1;

    // keep IDs of the neighbour faces that will later be updated
    unsigned int update_face_id_2 = faces[face_id].Face_ID[0];
    unsigned int update_face_id_3 = faces[face_id].Face_ID[1];
    unsigned int vertex_face_id_2 = faces[face_id].Vertex_ID[2];

    // update the split face to become one the 3 new faces
    faces[face_id].Vertex_ID[2] = new_vertex_id;
    unsigned int new_face_id_1 = faces.size();
    unsigned int new_face_id_2 = new_face_id_1 + 1;
    faces[face_id].Face_ID[0] = new_face_id_1;
    faces[face_id].Face_ID[1] = new_face_id_2;

    // update of the chosen vertex of the split face
    vertices[vertex_face_id_2].Face_ID = new_face_id_1;

    // update the neighbour faces
    for (unsigned int i = 0; i < 3; i++) {
        if (faces[update_face_id_2].Face_ID[i] == face_id) {
            faces[update_face_id_2].Face_ID[i] = new_face_id_1;
        }
        if (faces[update_face_id_3].Face_ID[i] == face_id) {
            faces[update_face_id_3].Face_ID[i] = new_face_id_2;
        }
    }

    // add the 2 other new faces
    unsigned int temp_vertices_ID[3];
    unsigned int temp_faces_ID[3];

    temp_vertices_ID[0] = faces[face_id].Vertex_ID[1];
    temp_vertices_ID[1] = vertex_face_id_2;
    temp_vertices_ID[2] = new_vertex_id;
    temp_faces_ID[0] = new_face_id_2;
    temp_faces_ID[1] = face_id;
    temp_faces_ID[2] = update_face_id_2;
    faces.push_back(Face3(temp_vertices_ID, temp_faces_ID));
    ReOrderIDs(faces.size() - 1, 0);

    temp_vertices_ID[0] = vertex_face_id_2;
    temp_vertices_ID[1] = faces[face_id].Vertex_ID[0];
    temp_vertices_ID[2] = new_vertex_id;
    temp_faces_ID[0] = face_id;
    temp_faces_ID[1] = new_face_id_1;
    temp_faces_ID[2] = update_face_id_3;
    faces.push_back(Face3(temp_vertices_ID, temp_faces_ID));
    ReOrderIDs(faces.size() - 1, 0);
}

void TopoMesh3D::FlipEdge(unsigned int face_id_1, unsigned int face_id_2) {
    // on recupere les indices relatifs des 4 faces adjacentes au
    // parallelogramme dans lequel on effectue le flip
    unsigned int face_id_1_1;
    unsigned int face_id_1_2;
    unsigned int face_id_2_1;
    unsigned int face_id_2_2;

    for (unsigned int i = 0; i < 3; i++) {
        if (faces[face_id_1].Face_ID[i] == face_id_2) {
            face_id_1_1 = (i + 1) % 3;
            face_id_1_2 = (i + 2) % 3;
        }
        if (faces[face_id_2].Face_ID[i] == face_id_1) {
            face_id_2_1 = (i + 1) % 3;
            face_id_2_2 = (i + 2) % 3;
        }
    }

    // on recupere les indices des sommets de l ancienne arete
    unsigned int i_sommet_A = faces[face_id_1].Vertex_ID[face_id_1_1];
    unsigned int i_sommet_B = faces[face_id_1].Vertex_ID[face_id_1_2];

    // on recupere les indices des sommets de l arete a construire
    unsigned int i_sommet_C = faces[face_id_1].Vertex_ID[(face_id_1_2 + 1) % 3];
    unsigned int i_sommet_D = faces[face_id_2].Vertex_ID[(face_id_2_2 + 1) % 3];

    // on change les indices de sommets des faces 1 et 2 pour flip l arete
    faces[face_id_1].Vertex_ID[face_id_1_2] = i_sommet_D;
    faces[face_id_2].Vertex_ID[face_id_2_2] = i_sommet_C;

    // on change les indices de faces pour les faces adjacentes
    // au parallelogramme
    for (unsigned int i = 0; i < 3; i++) {
        if (faces[faces[face_id_1].Face_ID[face_id_1_1]].Vertex_ID[i] == i_sommet_B) {
            faces[faces[face_id_1].Face_ID[face_id_1_1]].Face_ID[(i + 1) % 3] = face_id_2;
        }
        if (faces[faces[face_id_2].Face_ID[face_id_2_1]].Vertex_ID[i] == i_sommet_A) {
            faces[faces[face_id_2].Face_ID[face_id_2_1]].Face_ID[(i + 1) % 3] = face_id_1;
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


Mesh TopoMesh3D::ToGlMesh() const {
    std::vector<Vertex> ve;
    std::vector<unsigned int> id;
    std::vector<Texture> tx;

    glm::vec3 mi(vertices[1].x, vertices[1].y, vertices[1].z);
    glm::vec3 ma(vertices[1].x, vertices[1].y, vertices[1].z);

    glm::vec3 color(0.38, 0.306, 0.102);

    for (int i = 1; i < vertices.size(); i++) {
        glm::vec3 p(vertices[i].x, vertices[i].y, vertices[i].z);
        Vector normal = faces[vertices[i].Face_ID].ToTriangle(this).Normal();
        Vector lap = Laplacian(i).Normalized();
        glm::vec3 c(std::abs(lap.x), std::abs(lap.y), std::abs(lap.z));
        c = color;
        Vertex v = Vertex(p, glm::vec3(normal.x, normal.y,normal.z), c);
        ve.push_back(v);

        for (int j = 0; j < 3; j++) {
            if (p[j] > ma[j]) ma[j] = p[j];
            if (p[j] < mi[j]) mi[j] = p[j];
        }
    }

    for (int i = 1; i < faces.size(); i++) {
        id.push_back(faces[i].Vertex_ID[0] - 1);
        id.push_back(faces[i].Vertex_ID[1] - 1);
        id.push_back(faces[i].Vertex_ID[2] - 1);
    }

    Mesh m = Mesh(ve, id, tx);
    m.min_vertex = mi;
    m.max_vertex = ma;
    m.SetPrimitives(GL_TRIANGLES);
    return m;
}

Mesh TopoMesh3D::ToGlMesh_3() const {
    std::vector<Vertex> ve;
    std::vector<unsigned int> id;
    std::vector<Texture> tx;

    glm::vec3 mi(vertices[1].x, vertices[1].y, vertices[1].z);
    glm::vec3 ma(vertices[1].x, vertices[1].y, vertices[1].z);

    glm::vec3 color(0.38, 0.306, 0.102);
    glm::vec3 color_red(1., 0., 0.);

    for (int i = 1; i < vertices.size(); i++) {
        glm::vec3 p(vertices[i].x, vertices[i].y, vertices[i].z);

        for (int j = 0; j < 3; j++) {
            if (p[j] > ma[j]) ma[j] = p[j];
            if (p[j] < mi[j]) mi[j] = p[j];
        }
    }

    for (int i = 1; i < faces.size(); i++) {
        Vertex3 tv;
        glm::vec3 p;
        Vector normal;
        glm::vec3 c = color;

        tv = vertices[faces[i].Vertex_ID[0]];
        p = glm::vec3(tv.x, tv.y, tv.z);
        normal = faces[i].ToTriangle(this).Normal();
        Vertex v0 = Vertex(p, glm::vec3(normal.x, normal.y, normal.z), c, glm::vec3(1, 0, 0));
        c = color;

        tv = vertices[faces[i].Vertex_ID[1]];
        p = glm::vec3(tv.x, tv.y, tv.z);
        normal = faces[i].ToTriangle(this).Normal();
        Vertex v1 = Vertex(p, glm::vec3(normal.x, normal.y, normal.z), c, glm::vec3(0, 1, 0));
        c = color;

        tv = vertices[faces[i].Vertex_ID[2]];
        p = glm::vec3(tv.x, tv.y, tv.z);
        normal = faces[i].ToTriangle(this).Normal();
        Vertex v2 = Vertex(p, glm::vec3(normal.x, normal.y, normal.z), c, glm::vec3(0, 0, 1));
        c = color;
        
        ve.push_back(v0);
        ve.push_back(v1);
        ve.push_back(v2);

        id.push_back(3 * (i - 1));
        id.push_back(3 * (i - 1) + 1);
        id.push_back(3 * (i - 1) + 2);
    }

    Mesh m = Mesh(ve, id, tx);
    m.min_vertex = mi;
    m.max_vertex = ma;
    m.SetPrimitives(GL_TRIANGLES);
    return m;
}




//EOF