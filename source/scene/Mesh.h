//! Mesh.h
/*!
 * @date 	02.01.2013
 * @author	Guido Schmidt
 */

#ifndef MESH_H_
#define MESH_H_

//! C++ includes
#include <vector>
#include <iostream>
//! OpenGL includes
#include <GL/glew.h>
//! Assimp includes
#include <assimp/mesh.h>
//! DevIL includes
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
//! Local includes
#include "Node.h"
#include "Material.h"

namespace scene {

	class Mesh : public Node {
		private:
			//! Topology variables
			aiMesh* mesh;
			std::vector<GLfloat> vertices;
			std::vector<GLint> indices;
			std::vector<GLfloat> normals;
			std::vector<GLfloat> uvs;

			//! Buffer handlers
			GLenum ErrorCheckMesh;
			GLuint VAO_id;
			GLuint VBO_id, IBO_id, NBO_id, UVBO_id;

			//! Material
			Material* material;

		public:
			Mesh(aiMesh* m);
			virtual ~Mesh();

			void Initialize(void);
			void CreateBuffers(void);

			void SetMaterial(Material* material);

			void Draw(void);
	};

} //! namespace scene
#endif //! MESH_H_
