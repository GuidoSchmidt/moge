//!
/*!
 * @date	11.02.2013
 * @author	Guido Schmidt
 */

#ifndef BILLBOARD_H_
#define BILLBOARD_H_

#include "Mesh.h"

namespace scene {

	class Billboard: public scene::Mesh {
		private:
			glm::vec3 m_min;
			glm::vec3 m_max;
			glm::vec3 m_normal;

			aiMesh* m_mesh_ptr;
			std::vector<GLfloat> m_vertices;
			std::vector<GLint> m_indices;
			std::vector<GLfloat> m_normals;
			std::vector<GLfloat> m_uvs;
			//BoundingBox m_boundingBox;

			//! Buffer handlers
			GLenum ErrorCheckMesh;
			GLuint VAO_id, BBVAO_id;
			GLuint VBO_id, IBO_id, NBO_id, UVBO_id, BBO_id, BBIO_id;

		protected:
			void CalculateNormal(void);

		public:
			Billboard();
			Billboard(aiMesh * m);
			virtual ~Billboard();

			//! Material inherited from Mesh
			//! Texture inherited from Mesh

			// Getter
			glm::vec3 GetMin(void);
			glm::vec3 GetMax(void);
			glm::vec3 GetNormal(void);

			void Draw(void);
			void DrawBoundingBox(void);
	};

} //! namespace scene
#endif //! BILLBOARD_H_