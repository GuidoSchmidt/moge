//! SceneGraph.h
/*!
 * @date 	02.01.2013
 * @author 	Guido Schmidt
 */

#ifndef SCENEGRAPH_H_
#define SCENEGRAPH_H_

//! C++ includes
#include <string>
#include <fstream>
#include <iostream>
#include <stack>
#include <omp.h>
//! Assimp includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>
#include <assimp/quaternion.h>
//! GLM includes
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
//! Local includes
#include "../../utilities/Singleton.h"
#include "../MaterialManager.h"
#include "Billboard.h"
#include "Camera.h"
#include "Light.h"
#include "Material.h"
#include "Mesh.h"
#include "Node.h"

namespace scene {

	class SceneGraph {

		private:
			bool m_setupComplete;
			Camera* m_activeCamera_ptr;
			unsigned int m_activeLight_ptr;
			std::string m_scene_name;

			//! Assimp
			Assimp::Importer m_aiImporter;
			const aiScene* m_scene_ptr;

			//! Node hierachry
			Node m_root;
			std::vector<Light*> m_lights;
			std::vector<Billboard*> m_impostors;

			//! Material manager
			scene::MaterialManager* m_materialman_ptr;

			//! Logfile
			bool m_writeLogFile;
			std::ofstream m_logfile;

			//! Matrix-stack
			std::stack<glm::mat4> matrixStack;

			//! Light geometry
			unsigned int m_lightMatIndex;
			Mesh* m_pointLightMesh;
			Mesh* m_spotLightMesh;

			//! Scene approximation for parallax-corrected cubemaps
			BoundingBox* m_approx;

		protected:
			void ProcessScene(const aiScene* scene);

		public:

			SceneGraph();
			virtual ~SceneGraph();

			void Initialize(void);
			void LoadSceneFromFile(const std::string filename);
			void LoadScene(const std::string scenename);
			void Logging(bool logging);
			unsigned int NodeCount(void);

			//! Getter
			Node* GetNode(int i);
			Camera* GetActiveCamera(void);
			Light* GetActiveLight(void);
			Light* GetLight(int i);
			Mesh* GetLightMesh(lighttype type);
			int GetLightCount(void);
			//! Billboard impostors
			Billboard* GetImpostor(int i);
			int GetImpostorCount(void);
			//! Scene proxy geometry
			BoundingBox* GetSceneApproximation(void);

			//! Setter
			void SetActiveLight(int i);
			void SetActiveCamera(Camera* camera);

			//! Drawing
			glm::mat4 DrawNodes(void);
			glm::mat4 DrawNode(unsigned int i);
	};

} //! namespace scene
#endif //! SCENEGRAPH_H_
