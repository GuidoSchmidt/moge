//! Renderer.cpp
/*!
 * @date 	28.11.2012
 * @author 	Guido Schmidt
 */

#include "Renderer.h"


//! ANTTWEAKBAR CALLBACKS BEGIN
/*! Variables and button callbacks */
bool deferred;
bool loaded;
bool rotation;
float theta = 0, phi = 0;
float r = 0;
static void TW_CALL SwitchDeffered(void* clientData){ deferred = !deferred;}
static void TW_CALL SwitchRotation(void* clientData){ rotation = !rotation;}
//! ANTTWEAKBAR CALLBACKS END


//! Constructor
/*!
 *
 * @param width
 * @param height
 */
Renderer::Renderer(int width, int height)
{
	deferred = true;
	rotation = true;
	m_rotSpeed = 0.005f;

	tw_currentScene = HEAD;
	tw_currentDeferredTex = TEX_COMPOSIT;
	loaded = false;

	context_ptr = new Context();
	fsq_ptr = new FSQ();
	m_shininess = 12.0f;

	m_backgroundColor = glm::vec3(0.15f, 0.25f, 0.35f);

	Initialize(width, height);
}

//! Destructor
/*!
 *
 */
Renderer::~Renderer(void)
{

}

//! Initialize GLEW
/*! Initializes GLEW.*/
void Renderer::InitGLEW(void){
	GLenum GLEWinit = glewInit();
	if(GLEWinit != GLEW_OK){
		std::cout << "ERROR: could not initialize GLEW: " << glewGetErrorString(GLEWinit) << std::endl;
	}
}

//! Initialize
/*!
 * Initializes the renderer. Includes openning a context, adding of AntTweakBar variables,
 * Loading the scene, creating shader programs, and calling the other initialize methods.
 */
void Renderer::Initialize(int width, int height)
{
	context_ptr->OpenWindow(width, height, "Render Window", 4, 2);
	context_ptr->AddAntTweakBar();

	WriteLog(CONSOLE);
	InitGLEW();
	//! Initialize image loader utility
	InitializeILUT();

	glEnable(GL_DEPTH_TEST);

	//! AntTweakBar
	TwAddButton(context_ptr->GetBar(), "toggledeferred", SwitchDeffered, NULL, "key='space' label='Toggle Deferred Rendering' group='Rendering'");
	//! Deferred: render targets choice
	TwEnumVal texEV[NUM_TEXS] = { {TEX_COMPOSIT, "Composited"}, {TEX_POSITION, "Positionmap"} ,{TEX_COLOR, "Colormap"}, {TEX_NORMAL, "Normalmap"}, {TEX_DEPTH, "Depthmap"}};
	TwType texType = TwDefineEnum("TextureType", texEV, NUM_TEXS);
	TwAddVarRW(context_ptr->GetBar(), "deferredTextureChoice", texType, &tw_currentDeferredTex, "label='Rendering' group='Rendering' keyIncr='<' keyDecr='>' help='View the maps rendered in first pass.' ");
	//! Scene choice
	TwEnumVal sceneEV[NUM_SCENES] = { {HEAD, "Head"}, {GEOMETRY, "Geometry"}};
	TwType sceneType = TwDefineEnum("SceneType", sceneEV, NUM_SCENES);
	TwAddVarRW(context_ptr->GetBar(), "sceneChoice", sceneType, &tw_currentScene, "label='Scene' group='Scene' keyIncr='<' keyDecr='>' help='Load another scene.' ");
	//! Material
	TwAddVarRW(context_ptr->GetBar(), "shininess", TW_TYPE_FLOAT, &m_shininess, "step='0.01' max='100.0' min='0.0' label='Shininess' group='Material'");
	//! Background color
	TwAddVarRW(context_ptr->GetBar(), "background", TW_TYPE_COLOR3F, &m_backgroundColor, "label='Background' group='Scene'");
	//! Rotation
	TwAddButton(context_ptr->GetBar(), "togglerotation", SwitchRotation, NULL, "key='r' label='Toggle Rotation' group='Rotation'");
	TwAddVarRW(context_ptr->GetBar(), "rotationSpeed", TW_TYPE_FLOAT, &m_rotSpeed, "step='0.001' max='1.0' min='0.0' label='Rotationspeed' group='Rotation'");

	//! Initialize singleton instances
	scenegraph_ptr = Singleton<scene::SceneGraph>::Instance();

	/*! Init forward rendering
	 **************************/
		forwardProgram_ptr = new ShaderProgram(
				GLSL::VERTEX, "./source/shaders/forward/forward.vert.glsl",
				GLSL::FRAGMENT, "./source/shaders/forward/forward.frag.glsl"
		);

	/*! Init deferred rendering
	 * *************************/
		//! Inits for 1st pass
		deferredProgram_Pass1_ptr = new ShaderProgram(
				GLSL::VERTEX, "./source/shaders/deferred/pass_one.vert.glsl",
				GLSL::FRAGMENT, "./source/shaders/deferred/pass_one.frag.glsl"
		);
		firstPassFBO_ptr = new FrameBufferObject(
				context_ptr->GetWidth(), context_ptr->GetHeight()
		);

		//! Inits for 2nd pass
		deferredProgram_Pass2_ptr = new ShaderProgram(
				GLSL::VERTEX, "./source/shaders/deferred/deferred.vert.glsl",
				GLSL::FRAGMENT, "./source/shaders/deferred/deferred.frag.glsl"
		);
		fsq_ptr->CreateFSQ();

	InitializeMatrices();
	InitializeLight();

	m_running = true;
}


//! Initializes matrix setup
/*!
 * Creates a matrix setup for all non-dynamic matrices.
 */
void Renderer::InitializeMatrices(void)
{
	//! Identitymatrix
	IdentityMatrix = glm::mat4(1.0f);

	//! Viewmatrix
	CameraPosition = glm::vec3(0.0, 2.0, 10.0);
	CameraTargetPosition = glm::vec3(0.0, 1.0, 0.0);
	CameraUp = glm::vec3(0, 1, 0);

	//! Projectionmatrix
	ProjectionMatrix = glm::perspective(60.0f, (float)context_ptr->GetWidth()/(float)context_ptr->GetHeight(), 0.01f, 100.0f);
}

//! Initialize light setup
/*!
 * Creates a light setup, including light's position, ambient, diffuse & specular color.
 * Adds AntTweakBar variables, too.
 */
void Renderer::InitializeLight(void)
{
	LightPosition = glm::vec4(0.0f, 5.0f, 2.0f, 0.0f);
	LightAmbient = glm::vec3(0.0f, 0.0f, 0.0f);
	LightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	LightSpecular = glm::vec3(0.35f, 0.45f, 0.55f);

	TwAddVarRW(context_ptr->GetBar(), "lightAmbient", TW_TYPE_COLOR3F, &LightAmbient, "label='Ambient' group='Light'");
	TwAddVarRW(context_ptr->GetBar(), "lightDiffuse", TW_TYPE_COLOR3F, &LightDiffuse, "label='Diffuse' group='Light'");
	TwAddVarRW(context_ptr->GetBar(), "lightSpecular", TW_TYPE_COLOR3F, &LightSpecular, "label='Specular' group='Light'");
}

//! Initializes the DevIL image loader utility
/*!
 *
 */
void Renderer::InitializeILUT(void)
{
	ilutRenderer(ILUT_OPENGL);
	ilutRenderer(ILUT_OPENGL);
	ilInit();
	iluInit();
	ilutInit();
}


//! Writes the log
/*!
 * Wrtites a render log including the used OpenGL renderer (GPU) and it's vendor, OpenGL Version,
 * GLSL Version. The log can be written to console or to a log file
 */
void Renderer::WriteLog(log logLocation)
{
	glinfo_renderer_ptr = glGetString(GL_RENDERER);
	glinfo_vendor_ptr = glGetString(GL_VENDOR);
	glindo_openglVersion_ptr = glGetString(GL_VERSION);
	glinfo_glslVersion_ptr = glGetString(GL_SHADING_LANGUAGE_VERSION);

	glGetIntegerv(GL_MAJOR_VERSION, &glinfo_major);
	glGetIntegerv(GL_MINOR_VERSION, &glinfo_minor);

	if(logLocation == CONSOLE){
		std::cout << "\n------------------------------------------------------------" << std::endl;
		std::cout << "RENDERER INFO LOG" << std::endl;
		std::cout.width(25);
		std::cout << std::left <<"OpenGL Vendor:";
		std::cout.width(10);
		std::cout << std::left << glinfo_vendor_ptr << std::endl;
		std::cout.width(25);
		std::cout << std::left << "OpenGL Renderer:";
		std::cout.width(10);
		std::cout << std::left << glinfo_renderer_ptr << std::endl;
		std::cout.width(25);
		std::cout << "OpenGL Version:";
		std::cout.width(10);
		std::cout << std::left << glindo_openglVersion_ptr << std::endl;
		std::cout.width(25);
		std::cout << "GLSL Version:";
		std::cout.width(10);
		std::cout << std::left << glinfo_glslVersion_ptr << std::endl;
		std::cout << "------------------------------------------------------------" << std::endl;
	}
	else if(logLocation == FILE){
	}
}


//! Calculates frames per second
/*!
 * Calculates the frames per second and can write them to glfw window title or to
 * an AntTweakBar read only variable.
 */
void Renderer::CalculateFPS(double timeInterval, bool toWindowTitle)
{
	//! Static values which only get initialized the first time the function runs
	static double initTime = glfwGetTime(); //! Set the initial time to now
	static int frameCount = 0;             //! Set the initial FPS frame count to 0
	static double fps = 0.0;
	static bool fpsAtBar = false;

	//! Get the current time in seconds since the program started (non-static, so executed every time)
	double currentTime = glfwGetTime();

	//! Calculate and display the FPS every specified time interval
	if ((currentTime - initTime) > timeInterval)
	{
		//! Calculate the FPS as the number of frames divided by the interval in seconds
		fps = (double)frameCount / (currentTime - initTime);

		//! Reset the FPS frame counter and set the initial time to be now
		frameCount = 0;
		initTime = glfwGetTime();
	}
	else //! FPS calculation time interval hasn't elapsed yet? Simply increment the FPS frame counter
	{
		frameCount++;
	}

	if(toWindowTitle){
		std::stringstream sstream;
		sstream << fps;
		std::string newTitle = *context_ptr->GetTitle() + " @ " + sstream.str() + " fps";
		glfwSetWindowTitle(newTitle.c_str());
	}else{
		if(!fpsAtBar){
			TwAddVarRO(context_ptr->GetBar(), "FPS", TW_TYPE_DOUBLE, &fps, "group='General'");
			fpsAtBar = !fpsAtBar;
		}
	}
}


//! Keyboard function
/*!
 * Processes keyboard input using glfw.
 */
void Renderer::KeyboardFunction(void)
{
	//! Camera
	double speed = 0.005;
	if(glfwGetKey('W'))
	{
		scenegraph_ptr->GetActiveCamera()->TranslateZ(-speed);
	}
	if(glfwGetKey('S'))
	{
		scenegraph_ptr->GetActiveCamera()->TranslateZ(speed);
	}
	if(glfwGetKey('A'))
	{
		scenegraph_ptr->GetActiveCamera()->TranslateX(-speed);
	}
	if(glfwGetKey('D'))
	{
		scenegraph_ptr->GetActiveCamera()->TranslateX(speed);
	}

	//! Light
	if(glfwGetKey('I'))
	{
		LightPosition.z += speed;
	}
	if(glfwGetKey('K'))
	{
		LightPosition.z -= speed;
	}
	if(glfwGetKey('J'))
	{
		LightPosition.x += speed;
	}
	if(glfwGetKey('L'))
	{
		LightPosition.x -= speed;
	}
}


void Renderer::CameraMovement()
{
	if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT))
	{
		int x_pos, y_pos;
		glfwGetMousePos(&x_pos, &y_pos);
		//! Right
		if(x_pos > static_cast<float>(context_ptr->GetWidth())/2)
		{
//			std::cout << "mouse right" << std::endl;
		}
		//! Left
		if(x_pos < static_cast<float>(context_ptr->GetWidth())/2)
		{
//			std::cout << "mouse left" << std::endl;
		}
		//! Down
		if(y_pos > static_cast<float>(context_ptr->GetHeight())/2)
		{
//			std::cout << "mouse down" << std::endl;
		}
		//! Up
		if(y_pos < static_cast<float>(context_ptr->GetHeight())/2)
		{
//			std::cout << "mouse up" << std::endl;
		}
	}
}


//! Render loop
/*!
 * While variabke RUNNING is true, the renderer loops through this function.
 */
void Renderer::RenderLoop(void){
	while(m_running){
		if(!loaded){
			switch (tw_currentScene) {
				case HEAD:
					scenegraph_ptr->LoadSceneFromFile("./assets/scenes/collada/SimpleScene.dae");
					break;
				case GEOMETRY:
					scenegraph_ptr->LoadSceneFromFile("./assets/scenes/blend/Scene.blend");
					break;
				case BUDDHA:
					scenegraph_ptr->LoadSceneFromFile("./assets/scenes/collada/Buddha.dae");
					break;
				case TEAPOT:
					scenegraph_ptr->LoadSceneFromFile("./assets/scenes/blend/Teapot.blend");
					break;
				default:
					scenegraph_ptr->LoadSceneFromFile("./assets/scenes/blend/Head.blend");
					break;
			}
			renderQ_ptr = Singleton<scene::SceneOrganizer>::Instance()->OrganizeScene();
			std::cout << "RenderQ has size: " << renderQ_ptr->size() << std::endl;
			loaded = true;
		}

		//! Set background color
		glClearColor(m_backgroundColor.x, m_backgroundColor.y, m_backgroundColor.z, 1.0f);

		//! Calculations
		CalculateFPS(0.5, false);
		KeyboardFunction();
		CameraMovement();
		if(rotation)
			m_angle += m_rotSpeed;
		//! Modelmatrix
		glm::vec3 RotationAxis(0, 1, 0);
		glm::mat4 RotationMatrix = glm::rotate(m_angle, RotationAxis);
		//! Viewmatrix
		ViewMatrix = scenegraph_ptr->GetActiveCamera()->GetViewMatrix();


		/************************************
		 *  RENDER LOOP
		 ************************************/
		if(deferred){
			/*!* * * * * * * * * * * * * * *
			 *		DEFERRED RENDERING	   *
			 *		1ST RENDER PASS		   *
			 * * * * * * * * * * * * * * * */
			//! Framebuffer object setup and clear buffers
			firstPassFBO_ptr->Use();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//! Shader program setup & uniform bindings
			deferredProgram_Pass1_ptr->Use();
			//! uniform camera binding
			deferredProgram_Pass1_ptr->SetUniform("camera", CameraPosition);

			//! Drawing
			for(unsigned int n=0; n < renderQ_ptr->size(); n++)
			{
				if(static_cast<scene::Mesh*>((*renderQ_ptr)[n]))
				{
					ModelMatrix = static_cast<scene::Mesh*>((*renderQ_ptr)[n])->GetModelMatrix();
					ModelMatrix = glm::mat4(1.0f) * RotationMatrix * ModelMatrix;
					MVPMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;
					deferredProgram_Pass1_ptr->SetUniform("mvp", MVPMatrix);
					if(static_cast<scene::Mesh*>((*renderQ_ptr)[n])->GetMaterial()->HasTexture())
					{
						int current_tex_id = dynamic_cast<scene::Mesh*>((*renderQ_ptr)[n])->GetMaterial()->GetID();
						deferredProgram_Pass1_ptr->SetUniformSampler("colorTex", *(Singleton<scene::MaterialManager>::Instance()->GetTexture(current_tex_id)), 5);
					}
					static_cast<scene::Mesh*>((*renderQ_ptr)[n])->Draw();
				}
			}

			firstPassFBO_ptr->Unuse();
			deferredProgram_Pass1_ptr->Unuse();
			/*!* * * * * * * * * * * * * * *
			 *		DEFERRED RENDERING	   *
			 *		2ND RENDER PASS		   *
			 * * * * * * * * * * * * * * * */

			deferredProgram_Pass2_ptr->Use();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			deferredProgram_Pass2_ptr->SetUniform("Light.Position", LightPosition);
			deferredProgram_Pass2_ptr->SetUniform("Light.Ambient", LightAmbient);
			deferredProgram_Pass2_ptr->SetUniform("Light.Diffuse", LightDiffuse);
			deferredProgram_Pass2_ptr->SetUniform("Light.Specular", LightSpecular);
			deferredProgram_Pass2_ptr->SetUniform("Shininess", m_shininess);

			deferredProgram_Pass2_ptr->SetUniform("textureID", tw_currentDeferredTex);

			deferredProgram_Pass1_ptr->SetUniform("camera", CameraPosition);

			deferredProgram_Pass2_ptr->SetUniformSampler("deferredPositionTex", firstPassFBO_ptr->GetTexture(0), 1);
			deferredProgram_Pass2_ptr->SetUniformSampler("deferredColorTex", firstPassFBO_ptr->GetTexture(1), 2);
			deferredProgram_Pass2_ptr->SetUniformSampler("deferredNormalTex", firstPassFBO_ptr->GetTexture(2), 3);
			deferredProgram_Pass2_ptr->SetUniformSampler("deferredDepthTex", firstPassFBO_ptr->GetDepthTexture(), 4);

			fsq_ptr->Draw();

			deferredProgram_Pass2_ptr->Unuse();
		}
		else{
			/*!* * * * * * * * * * * * * * *
			 *		FORWARD RENDERING	   *
			 * * * * * * * * * * * * * * * */
			forwardProgram_ptr->Use();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//! Uniform bindings
			forwardProgram_ptr->SetUniform("Light.Position", LightPosition);
			forwardProgram_ptr->SetUniform("Light.Ambient", LightAmbient);
			forwardProgram_ptr->SetUniform("Light.Diffuse", LightDiffuse);
			forwardProgram_ptr->SetUniform("Light.Specular", LightSpecular);

			forwardProgram_ptr->SetUniform("mvp", MVPMatrix);

			//! Drawing
			for(unsigned int n=0; n < renderQ_ptr->size(); n++)
			{
				if(static_cast<scene::Mesh*>((*renderQ_ptr)[n]))
				{
					ModelMatrix = static_cast<scene::Mesh*>((*renderQ_ptr)[n])->GetModelMatrix();
					ModelMatrix = glm::mat4(1.0f) * RotationMatrix * ModelMatrix;
					MVPMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;
					forwardProgram_ptr->SetUniform("mvp", MVPMatrix);
					if(static_cast<scene::Mesh*>((*renderQ_ptr)[n])->GetMaterial()->HasTexture())
					{
						int current_tex_id = dynamic_cast<scene::Mesh*>((*renderQ_ptr)[n])->GetMaterial()->GetID();
						forwardProgram_ptr->SetUniformSampler("colorTex", *(Singleton<scene::MaterialManager>::Instance()->GetTexture(current_tex_id)), 5);
					}
					static_cast<scene::Mesh*>((*renderQ_ptr)[n])->Draw();
				}
			}

			forwardProgram_ptr->Unuse();
		}

		//! Draw AntTweakBar-GUI
		TwDraw();
		context_ptr->SwapBuffers();

		//! Check for context exiting
		if(context_ptr->IsExiting()){
			m_running = false;
		}
	}
}
