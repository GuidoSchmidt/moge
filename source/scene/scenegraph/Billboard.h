//!
/*!
 * @date	11.02.2013
 * @author	Guido Schmidt
 */

#ifndef BILLBOARD_H_
#define BILLBOARD_H_

#include "Mesh.h"
#include "Camera.h"

namespace scene {

	class Billboard: public scene::Mesh {
		private:
			glm::vec3 m_min;
			glm::vec3 m_max;
			glm::vec3 m_u;
			glm::vec3 m_v;
			glm::vec3 m_normal;

			aiMesh* m_mesh_ptr;

			glm::vec3 m_center;
			std::vector<GLfloat> m_vertices;
			std::vector<GLint> m_indices;
			std::vector<GLfloat> m_normals;
			std::vector<GLfloat> m_uvs;
			BoundingBox m_boundingBox;

			Camera* m_camera_ptr;

			//! Buffer handlers
			GLenum ErrorCheckMesh;
			GLuint VAO_id, BBVAO_id;
			GLuint VBO_id, IBO_id, NBO_id, UVBO_id, BBO_id, BBIO_id;

		protected:
			void CalculateNormal(void);
			void Initialize(void);
			void CreateBuffers(void);

		public:
			Billboard();
			Billboard(aiMesh* mesh);
			virtual ~Billboard();

			glm::vec3 GetNormal(void);
			Camera* GetCamera(void);
			glm::vec3 GetVertex(int i);
			glm::vec3 GetUMax(void);
			glm::vec3 GetVMax(void);

			void SetCamera(Camera* camera);

			void Draw(void);

	};

} //! namespace scene
#endif //! BILLBOARD_H_
