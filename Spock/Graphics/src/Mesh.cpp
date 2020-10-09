#include "../inc/Mesh.h"




Mesh::Mesh(Device* d)
{
	this->ThisDevice = d;
}


Mesh::~Mesh()
{
	//glDeleteBuffers(1, &(this->vertBuffer));
	delete[] this->data;
}


void Mesh::Draw() {

}


void Mesh::Load(std::string objFileName) {

	std::ifstream file;
	this->modelName = objFileName;
	file.open(objFileName, std::fstream::in);
	if (!file) {
#ifdef DEBUG
		fprintf(stderr, "Error opening obj file %s\n", objFileName.c_str());
#endif
		return;
	}
	int lines = 0;
	std::string line = "";
	while (std::getline(file, line)) {
		++lines;
	}
		file.clear();
		file.seekg(0);


	uint64_t vertCount = 0;
	uint64_t normCount = 0;
	uint64_t texCount = 0;
	uint64_t triCount = 0;

	uint64_t maxTris = lines * 3;



	//Initialize the arrays


	glm::vec3* verts = new glm::vec3[lines];
	glm::vec3* norms = new glm::vec3[lines];
	glm::vec2* tex = new glm::vec2[lines];

	glm::ivec3** tris = new glm::ivec3*[lines];
	for (int i = 0; i < lines; ++i) {
		//Special note: Using an ivec3 istead of a vec3 to avoid roundoff errors and compiler warnings
		tris[i] = new glm::ivec3[3];//using glm::vec3 creatively here. 
		//OBJ file recoreds faces as: f v1/t1/n1 v2/t2/n2 v3/t3/n3
		//so tris[n][0].x = v1 .y = t1 .z = n1
		//tris[n][1].x = v2 .y = t2 .z = n2
		//tris[n][2].x = v3 .y = t3 .z = n3
	}


	//end init of arrays
	while (std::getline(file, line)) {
		if ((line[0] == 'v' || line[0] == 'V') && line[1] == ' ') {
			float x, y, z;
			sscanf_s(line.c_str(), "v %f %f %f\n", &x, &y, &z);
			verts[vertCount].x = x;
			verts[vertCount].y = y;
			verts[vertCount].z = z;
			++vertCount;
		}
		if (line[0] == 'v' && line[1] == 'n') {
			float xn, yn, zn;
			sscanf_s(line.c_str(), "vn %f %f %f\n", &xn, &yn, &zn);
			norms[normCount].x = xn;
			norms[normCount].y = yn;
			norms[normCount].z = zn;
			++normCount;
		}
		if (line[0] == 'v' && line[1] == 't') {
			float s, t;
			sscanf_s(line.c_str(), "vt %f %f\n", &s, &t);
			tex[texCount].s = s;
			tex[texCount].t = t;
			++texCount;
		}
		if (line[0] == 'f') {
			int vertsPerFace = 0;
			for (int i = 0; i < strlen(line.c_str()); ++i) {
				if (line[i] == '/') {
					++vertsPerFace;
				}
			}
			vertsPerFace /= 2;

			int v1, v2, v3, v4;
			int t1, t2, t3, t4;
			int n1, n2, n3, n4;
			v1 = 0;
			v2 = 0;
			v3 = 0;
			v4 = 0;
			t1 = 0;
			t2 = 0;
			t3 = 0;
			t4 = 0;
			n1 = 0;
			n2 = 0;
			n3 = 0;
			n4 = 0;


			if (vertsPerFace == 3) {
				//fix for no texture coords
				std::size_t pos = line.find("//");
				while (pos != std::string::npos) {
					line.insert(pos + 1, "0");
					pos = line.find("//");
				}
				//load in the triangle
				sscanf_s(line.c_str(), "f %i/%i/%i %i/%i/%i %i/%i/%i", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
				if (triCount == maxTris) {
					//need to resize tris
					//gonna leave this out for now, heh......

				}
				tris[triCount][0].x = v1;
				tris[triCount][0].y = t1;
				tris[triCount][0].z = n1;

				tris[triCount][1].x = v2;
				tris[triCount][1].y = t2;
				tris[triCount][1].z = n2;

				tris[triCount][2].x = v3;
				tris[triCount][2].y = t3;
				tris[triCount][2].z = n3;
				++triCount;


			}
			if (vertsPerFace == 4) {
				//fix for no texture coords
				std::size_t pos = line.find("//");
				while (pos != std::string::npos) {
					line.insert(pos + 1, "0");
					pos = line.find("//");
				}
				//load in the quad and try to triangulate
				sscanf_s(line.c_str(), "f %i/%i/%i %i/%i/%i %i/%i/%i %i/%i/%i", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3, &v4, &t4, &n4);
				if (triCount == maxTris) {
					//need to resize tris
				}


				tris[triCount][0].x = v1;
				tris[triCount][0].y = t1;
				tris[triCount][0].z = n1;

				tris[triCount][1].x = v2;
				tris[triCount][1].y = t2;
				tris[triCount][1].z = n2;

				tris[triCount][2].x = v3;
				tris[triCount][2].y = t3;
				tris[triCount][2].z = n3;
				++triCount;

				tris[triCount][0].x = v3;
				tris[triCount][0].y = t3;
				tris[triCount][0].z = n3;

				tris[triCount][1].x = v4;
				tris[triCount][1].y = t4;
				tris[triCount][1].z = n4;

				tris[triCount][2].x = v1;
				tris[triCount][2].y = t1;
				tris[triCount][2].z = n1;
				++triCount;
			}
		}
	}


	//fprintf(stderr, "Triangle counts in mesh %s: %i\n",this->modelName.c_str(),  triCount);
	//int dataLen = triCount * 3 + triCount * 3 + triCount *2;
	//int dataLen = triCount * 8 * 3;
	int dataLen = triCount * 8 * 3;
	this->dataSize = dataLen;

	int idx = 0;
	//GLfloat* data = new GLfloat[dataLen];
	this->data = new float[dataLen];
	for (int i = 0; i < dataLen; ++i) {
		data[i] = 0.0;
	}
	for (int i = 0; i < triCount; ++i) {
		glm::ivec3* tri = tris[i];
		int v1 = tri[0].x;
		int t1 = tri[0].y;
		int n1 = tri[0].z;

		int v2 = tri[1].x;
		int t2 = tri[1].y;
		int n2 = tri[1].z;

		int v3 = tri[2].x;
		int t3 = tri[2].y;
		int n3 = tri[2].z;


		//v1
		data[idx++] = verts[v1 - 1].x;
		data[idx++] = verts[v1 - 1].y;
		data[idx++] = verts[v1 - 1].z;

		data[idx++] = norms[n1 - 1].x;
		data[idx++] = norms[n1 - 1].y;
		data[idx++] = norms[n1 - 1].z;

		data[idx++] = tex[t1 - 1].s;
		data[idx++] = tex[t1 - 1].t;


		//v2
		data[idx++] = verts[v2 - 1].x;
		data[idx++] = verts[v2 - 1].y;
		data[idx++] = verts[v2 - 1].z;

		data[idx++] = norms[n2 - 1].x;
		data[idx++] = norms[n2 - 1].y;
		data[idx++] = norms[n2 - 1].z;

		data[idx++] = tex[t2 - 1].s;
		data[idx++] = tex[t2 - 1].t;


		//v3
		data[idx++] = verts[v3 - 1].x;
		data[idx++] = verts[v3 - 1].y;
		data[idx++] = verts[v3 - 1].z;

		data[idx++] = norms[n3 - 1].x;
		data[idx++] = norms[n3 - 1].y;
		data[idx++] = norms[n3 - 1].z;

		data[idx++] = tex[t3 - 1].s;
		data[idx++] = tex[t3 - 1].t;

	}

	delete [] verts;
	delete [] norms;
	delete [] tex;
	delete [] tris;

	this->verts = triCount * 3;
}

float* Mesh::GetRaw(int& size) {
	size = this->verts;
	return this->data;
}