#include "trianglemesh.h"

// Constructor of a triangle mesh.
TriangleMesh::TriangleMesh()
{
	numVertices = 0;
	numTriangles = 0;
	objCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	vboId = 0;
	objExtent = glm::vec3(0.0f, 0.0f, 0.0f);
}

// Destructor of a triangle mesh.
TriangleMesh::~TriangleMesh()
{
	vertices.clear();
	glDeleteBuffers(1, &vboId);
}

// Load the geometry and material data from an OBJ file.
bool TriangleMesh::LoadFromFile(const std::string& filePath, const bool normalized)
{	
    std::ifstream f(filePath);  // Open the file
    if (!f.is_open()) {  // If the file cannot be opened, report an error and return false
        std::cerr << "Error: Could not open the file " << filePath << std::endl;
        return false;
    }

    std::string line;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> textures;
    VertexPTN newVertex;

    int point_count = 0;  // Count the number of points
    auto subMesh_pointer = subMeshes.end();
    std::vector<unsigned int> currentVertexIndices;

    while (std::getline(f, line)) {
        if (line.substr(0, 2) == "v ") {
            std::istringstream iss(line.substr(2));
            glm::vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        }
        else if (line.substr(0, 3) == "vt ") {
            std::istringstream iss(line.substr(3));
            glm::vec2 tex;
            iss >> tex.x >> tex.y;
            textures.push_back(tex);
        }
        else if (line.substr(0, 3) == "vn ") {
            std::istringstream iss(line.substr(3));
            glm::vec3 nor;
            iss >> nor.x >> nor.y >> nor.z;
            normals.push_back(nor);
        }
        else if (line.substr(0, 2) == "f ") {
            std::istringstream iss(line.substr(2));
            std::string vertexIndicesStr;
            std::vector<unsigned int> tempVertexIndices;

            while (iss >> vertexIndicesStr) { // Read a whole line
                std::istringstream vertexIndicesStream(vertexIndicesStr);
                std::string str;
                while (std::getline(vertexIndicesStream, str, '/'))   // Remove slashes
                    tempVertexIndices.push_back(std::stoi(str) - 1);  // Convert to 0 base
            }

            for (int i = 0; i < tempVertexIndices.size(); i += 3) {
                // Store a PTN for one point in the structure
                vertices.push_back(VertexPTN(positions[tempVertexIndices[i]], normals[tempVertexIndices[i + 2]], textures[tempVertexIndices[i + 1]]));
            }
            int points_number = tempVertexIndices.size() / 3;  // How many points in one line
            if (points_number == 3) {  // Only one triangle
                for (int i = 0; i < 3; i++) {
                    currentVertexIndices.push_back(point_count);
                    point_count++;
                }
            }
            else {  // Split triangles (more than one point in a line)
                for (int i = 2; i < points_number; i++) {
                    currentVertexIndices.push_back(point_count);
                    currentVertexIndices.push_back(point_count + i - 1);
                    currentVertexIndices.push_back(point_count + i);
                }
                point_count += points_number;
            }
            if (subMesh_pointer != subMeshes.end()) {  // 把存好的點放進vertexIndices
                subMesh_pointer->vertexIndices.insert(subMesh_pointer->vertexIndices.end(), currentVertexIndices.begin(), currentVertexIndices.end());
            }

            numTriangles += currentVertexIndices.size() / 3;
            currentVertexIndices.clear();  // 清空暫存的的vertexIndices接著跑下一個submesh
        }
        else if (line.substr(0, 7) == "usemtl ") {  // 判斷要用哪一個material
            std::istringstream iss(line.substr(7));
            std::string mat;
            iss >> mat;
            subMesh_pointer = std::find_if(subMeshes.begin(), subMeshes.end(), [&mat](const SubMesh& subMesh) {
                return subMesh.material && subMesh.material->GetName() == mat;
                });
        }
        else if (line.substr(0, 7) == "mtllib ") {  // 判斷要用哪一個model的material library
            std::istringstream iss(line.substr(7));
            std::string lib;
            iss >> lib;

            size_t lastSlash = filePath.find_last_of('/');  // 找到輸入路徑的最後一個slash

            std::string newFileName = filePath;
            newFileName = newFileName.substr(0, lastSlash + 1) + lib;  // 把last slash後面的字串存成新檔案的名字

            LoadMaterialsFromFile(newFileName);
        }

    }
    f.close();

    numVertices = vertices.size();

    // Normalize the geometry data.
    if (normalized) {
        Normalize();
    }
    return true;
}

void TriangleMesh::Normalize()  // same as HW1
{
    float center_x;
    float center_y;
    float center_z;
    float max_x = vertices[0].position.x;
    float min_x = vertices[0].position.x;
    float max_y = vertices[0].position.y;
    float min_y = vertices[0].position.y;
    float max_z = vertices[0].position.z;
    float min_z = vertices[0].position.z;

    for (int i = 1; i < vertices.size(); i++) {
        if (vertices[i].position.x > max_x)
            max_x = vertices[i].position.x;
        if (vertices[i].position.x < min_x)
            min_x = vertices[i].position.x;
        if (vertices[i].position.y > max_y)
            max_y = vertices[i].position.y;
        if (vertices[i].position.y < min_y)
            min_y = vertices[i].position.y;
        if (vertices[i].position.z > max_z)
            max_z = vertices[i].position.z;
        if (vertices[i].position.z < min_z)
            min_z = vertices[i].position.z;
    }
    center_x = (max_x + min_x) * 0.5f;
    center_y = (max_y + min_y) * 0.5f;
    center_z = (max_z + min_z) * 0.5f;

    objCenter.x = center_x;
    objCenter.y = center_y;
    objCenter.z = center_z;

    for (int i = 0; i < vertices.size(); i++) {
        vertices[i].position.x -= center_x;
        vertices[i].position.y -= center_y;
        vertices[i].position.z -= center_z;
    }

    float length_x = max_x - min_x;
    float length_y = max_y - min_y;
    float length_z = max_z - min_z;

    float max_length = length_x;
    if (length_y > max_length) {
        max_length = length_y;
    }

    if (length_z > max_length) {
        max_length = length_z;
    }
    for (int i = 0; i < vertices.size(); i++) {
        vertices[i].position.x /= max_length;
        vertices[i].position.y /= max_length;
        vertices[i].position.z /= max_length;
    }
}

bool TriangleMesh::LoadMaterialsFromFile(std::string newFileName)
{
    std::ifstream f(newFileName);  // Open the file
    if (!f.is_open()) {  // If the file cannot be opened, report an error and return false
        std::cerr << "Error: Could not open the material file " << newFileName << std::endl;
        return false;
    }

    std::string line;
    PhongMaterial* nowMaterial = nullptr;
    ImageTexture* texture = nullptr;

    while (std::getline(f, line)) {
        std::istringstream iss(line);
        std::string temp;
        iss >> temp;

        if (temp == "newmtl") {
            nowMaterial = new PhongMaterial();
            std::string material;
            iss >> material;
            nowMaterial->SetName(material);  // 存現在這個面的material

            SubMesh newSubMesh;
            newSubMesh.material = nowMaterial;
            subMeshes.push_back(newSubMesh);
        }
        else if (nowMaterial && temp == "Ka") {
            glm::vec3 Ka;
            iss >> Ka.x >> Ka.y >> Ka.z;
            nowMaterial->SetKa(Ka);
        }
        else if (nowMaterial && temp == "Kd") {
            glm::vec3 Kd;
            iss >> Kd.x >> Kd.y >> Kd.z;
            nowMaterial->SetKd(Kd);
        }
        else if (nowMaterial && temp == "Ks") {
            glm::vec3 Ks;
            iss >> Ks.x >> Ks.y >> Ks.z;
            nowMaterial->SetKs(Ks);
        }
        else if (nowMaterial && temp == "Ns") {
            float Ns;
            iss >> Ns;
            nowMaterial->SetNs(Ns);
        }
        else if (nowMaterial && temp == "map_Kd") {
            std::string s;
            iss >> s;
            size_t lastSlash = newFileName.find_last_of('/');  // 找到輸入路徑的最後一個slash

            std::string texFileName = newFileName;
            texFileName = texFileName.substr(0, lastSlash + 1) + s;  // 把last slash後面的字串存成新檔案的名字

            texture = new ImageTexture(texFileName);
            nowMaterial->SetMapKd(texture);
        }
    }
    f.close();
}

void TriangleMesh::Rendering(SubMesh submesh)
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (const GLvoid*)12);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (const GLvoid*)24);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, submesh.iboId);
    glDrawElements(GL_TRIANGLES, (GLsizei)(submesh.vertexIndices.size()), GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

// Create the buffers.
void TriangleMesh::CreateBuffers() {
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPTN) * numVertices, vertices.data(), GL_STATIC_DRAW);
    for (auto& submesh : subMeshes) {
        glGenBuffers(1, &(submesh.iboId));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, submesh.iboId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * submesh.vertexIndices.size(), submesh.vertexIndices.data(), GL_STATIC_DRAW);
    }
}
// Release the buffers.
void TriangleMesh::ReleaseBuffers() {
    glDeleteBuffers(1, &vboId);
    for (auto& submesh : subMeshes) {
        glDeleteBuffers(1, &submesh.iboId);
    }
}
// Show model information.
void TriangleMesh::ShowInfo()
{
	std::cout << "# Vertices: " << numVertices << std::endl;
	std::cout << "# Triangles: " << numTriangles << std::endl;
	std::cout << "Total " << subMeshes.size() << " subMeshes loaded" << std::endl;
	for (unsigned int i = 0; i < subMeshes.size(); ++i) {
		const SubMesh& g = subMeshes[i];
		std::cout << "SubMesh " << i << " with material: " << g.material->GetName() << std::endl;
		std::cout << "Num. triangles in the subMesh: " << g.vertexIndices.size() / 3 << std::endl;
	}
	std::cout << "Model Center: " << objCenter.x << ", " << objCenter.y << ", " << objCenter.z << std::endl;
	std::cout << "Model Extent: " << objExtent.x << " x " << objExtent.y << " x " << objExtent.z << std::endl;
}

