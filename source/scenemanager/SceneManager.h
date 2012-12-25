/*!	Scene manager class definition
 *	@author Guido Schmidt
 *	@date 	25.12.12
 */

#ifndef SCENEMANAGER_H_
#define SCENEMANAGER_H_

//! C++ includes
#include <vector>
//! Local includes
#include "Node.h"
#include "Singleton.h"
//! Assimp includes
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

namespace scenegraph {

class SceneManager : public Singleton {
	private:
		SceneManager();
		~SceneManager();
		Node root;
		aiScene scene;

	public:
		bool LoadScene(std::string filename);
};

} //! namespace scenegraph
#endif //! SCENEMANAGER_H_
