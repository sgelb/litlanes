#include <terrain.h>

// TODO: create Vertice class and divide position and color

Terrain::Terrain(const GLuint meshSize) : meshSize(meshSize) {
  this->createVertices();
  this->createIndices();
}


void Terrain::createVertices() {

  /*

   y ^ Right-handed coordinates system. y is height.
     |
     |
     +-----> x
    / 
  z/ 

  the polygon mesh consists of vertices forming a large square from (0, y, 0) to
  (width, y, width) with width*width subtiles. each subtile is split in two
  triangles from top left to bottom right.

  */

  this->vertices = std::vector<GLfloat>();
  noise::module::Perlin noise;


  for (size_t z = 0; z < meshSize; z++) {
    for (size_t x = 0; x < meshSize; x++) {
      // use x and z (mapped to [-1, 1]) to create height generated with 
      // perlin noise.
      GLfloat y = noise.GetValue(mapToInterval(x), 0.5f, mapToInterval(z));

      // coordinates
      vertices.push_back(static_cast<GLfloat>(x));
      vertices.push_back(static_cast<GLfloat>(Constants::MaxMeshHeight * y));
      vertices.push_back(static_cast<GLfloat>(z));
      // color
      vertices.push_back(static_cast<GLfloat>(y));
      vertices.push_back(static_cast<GLfloat>(y));
      vertices.push_back(static_cast<GLfloat>(y));
    }
  }
}


void Terrain::createIndices() {
  this->indices = std::vector<GLuint>();
  for (size_t z = 0; z < meshSize-1; z++) {
    for (size_t x = 0; x < meshSize-1; x++) {
      /*

      +---x
      |
      |   TL_TR  We split each subtile in two triangles and put their indices
      y   |\  |  counterclockwise in indices-Array:
          | \ |  Left triangle:  TL->BL->BR
          |__\|  Right triangle: TL->BR->TR
          BL BR

      */

      GLuint tl = x + meshSize*z;
      GLuint tr = tl + 1;
      GLuint bl = tl + meshSize;
      GLuint br = bl + 1;

      // left triangle
      indices.push_back(tl);
      indices.push_back(bl);
      indices.push_back(br);

      // right triangle
      indices.push_back(tl);
      indices.push_back(br);
      indices.push_back(tr);
    }
  }
}

std::vector<GLuint> Terrain::getIndices() {
  return this->indices;
}

std::vector<GLfloat> Terrain::getVertices() {
  return this->vertices;
}

GLfloat Terrain::mapToInterval(const GLfloat &input) {
  // map input from [0, Constants::MeshWidth] to [-1, 1]
  return 2 * (input/Constants::MeshWidth) - 1;
}
