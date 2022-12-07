// 3D Graphics and Animation - Main Template
// This uses Visual Studio Code - https://code.visualstudio.com/docs/cpp/introvideos-cpp
// Two versions available -  Win64 and Apple MacOS - please see notes
// Last changed August 2022

//#pragma comment(linker, "/NODEFAULTLIB:MSVCRT")

//#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include<windows.h>
using namespace std;

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h> // Extend OpenGL Specfication to version 4.5 for WIN64 and versions 4.1 for Apple (note: two different files).

#include <imgui/imgui.h>			  // Load GUI library - Dear ImGui - https://github.com/ocornut/imgui
#include <imgui/imgui_impl_glfw.h>	  // Platform ImGui using GLFW
#include <imgui/imgui_impl_opengl3.h> // Platform new OpenGL - aka better than 3.3 core version.

#include <GLFW/glfw3.h> // Add GLFW - library to launch a window and callback mouse and keyboard - https://www.glfw.org

#define GLM_ENABLE_EXPERIMENTAL	 // Enable all functions include non LTS
#include <glm/glm.hpp>			 // Add helper maths library - GLM 0.9.9.9 - https://github.com/g-truc/glm - for example variables vec3, mat and operators.
#include <glm/gtx/transform.hpp> // Help us with transforms
using namespace glm;
 
//#include <tinygltf/tiny_gltf.h> // Model loading library - tiny gltf - https://github.com/syoyo/tinygltf
//#include "src/stb_image.hpp" // Add library to load images for textures

//#include "src/Mesh.hpp" // Simplest mesh holder and OBJ loader - can update more - from https://github.com/BennyQBD/ModernOpenGLTutorial

#include "src/Pipeline.hpp"		// Setup pipeline and load shaders.
#include "src/Content.hpp"		// Setup content loader and drawing functions - https://github.com/KhronosGroup/glTF - https://github.com/syoyo/tinygltf 
#include "src/Debugger.hpp"		// Setup debugger functions.



// Main fuctions
void startup();
void update();
void render();
void ui();
void endProgram();

// HELPER FUNCTIONS OPENGL
void hintsGLFW();
//string readShader(string name);
//void checkErrorShader(GLuint shader);
inline void errorCallbackGLFW(int error, const char *description){cout << "Error GLFW: " << description << "\n";};
void debugGL();

void APIENTRY openGLDebugCallback(GLenum source,
								  GLenum type,
								  GLuint id,
								  GLenum severity,
								  GLsizei length,
								  const GLchar *message,
								  const GLvoid *userParam);
GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)

// Setup all the message loop callbacks - do this before Dear ImGui
// Callback functions for the window and interface devices
void onResizeCallback(GLFWwindow *window, int w, int h);
void onKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void onMouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void onMouseMoveCallback(GLFWwindow *window, double x, double y);
void onMouseWheelCallback(GLFWwindow *window, double xoffset, double yoffset);

// VARIABLES
GLFWwindow *window; 								// Keep track of the window
auto windowWidth = 800;								// Window width					
auto windowHeight =800;								// Window height
auto running(true);							  		// Are we still running our main loop
mat4 projMatrix;							 		// Our Projection Matrix
vec3 cameraPosition = vec3(0.0f, 2.0f, 0.0f);		// Where is our camera
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);			// Camera front vector
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);				// Camera up vector

int jump = 0;
int windToggle = 0;
int portalsToggle = 0;

GLfloat ka = 1.0;
glm::vec3 ia = glm::vec3(0.3f, 0.3f, 0.3f); // Ambient lighting
glm::vec3 id = glm::vec3(0.9f, 0.9f, 0.9f); // Diffuse Lighting
glm::vec3 is = glm::vec3(0.9f, 0.9f, 0.9f); // Specular Lighting

auto aspect = (float)windowWidth / (float)windowHeight;	// Window aspect ration
auto fovy = 45.0f;									// Field of view (y axis)
bool keyStatus[1024];								// Track key strokes
auto currentTime = 0.0f;							// Framerate
auto deltaTime = 0.0f;								// time passed
auto lastTime = 0.0f;								// Used to calculate Frame rate

Pipeline pipeline;									// Add one pipeline plus some shaders.
Content rocks_light;
Content rocks_medium;
Content rocks_dark;
Content leaves;
Content roots;
Content floor_1;
Content floor_2;
Content floor_3;
Content floor_4;
Content terrain;
Content roof_1;
Content roof_decor;
Content roof_top;
Content columns;
Content portals;
Content inside_path;
Content light_source;
Content knight_body;
Content knight_helm_boots;
Content knight_sword;
Content shield;


Debugger debugger;									// Add one debugger to use for callbacks ( Win64 - openGLDebugCallback() ) or manual calls ( Apple - glCheckError() ) 

vec3 modelPosition;									// Model position
vec3 modelRotation;									// Model rotation

vec3 lightRotation;
vec3 knightRotation;
vec3 knightPosition;




int main()
{
	cout << endl << "===" << endl << "3D Graphics and Animation - Running..." << endl;

	if (!glfwInit()) // Check if GLFW is working, if not then stop all
	{
		cout << "Could not initialise GLFW...";
		return -1;
	} 

	glfwSetErrorCallback(errorCallbackGLFW); // Setup a function callback to catch and display all GLFW errors.
	hintsGLFW();							 // Setup glfw with various hints.

	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor()); // Grab reference to monitor
	// windowWidth = mode->width; windowHeight = mode->height; //fullscreen
	// window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), glfwGetPrimaryMonitor(), NULL); // fullscreen

	// Create our Window
	const auto windowTitle = "My 3D Graphics and Animation OpenGL Application"s;
	window = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), NULL, NULL);
	if (!window) // Test if Window or OpenGL context creation failed
	{
		cout << "Could not initialise GLFW...";
		glfwTerminate();
		return -1;
	} 

	glfwSetWindowPos(window, 10, 10); // Place it in top corner for easy debugging.
	glfwMakeContextCurrent(window);	  // making the OpenGL context current

	// GLAD: Load OpenGL function pointers - aka update specs to newest versions - plus test for errors.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD...";
		glfwMakeContextCurrent(NULL);
		glfwTerminate();
		return -1;
	}

	glfwSetWindowSizeCallback(window, onResizeCallback);	   // Set callback for resize
	glfwSetKeyCallback(window, onKeyCallback);				   // Set Callback for keys
	glfwSetMouseButtonCallback(window, onMouseButtonCallback); // Set callback for mouse click
	glfwSetCursorPosCallback(window, onMouseMoveCallback);	   // Set callback for mouse move
	glfwSetScrollCallback(window, onMouseWheelCallback);	   // Set callback for mouse wheel.
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);	// Set mouse cursor Fullscreen
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// Set mouse cursor FPS fullscreen.

	// Setup Dear ImGui and add context	-	https://blog.conan.io/2019/06/26/An-introduction-to-the-Dear-ImGui-library.html
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO(); //(void)io;
								  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui::StyleColorsLight(); // ImGui::StyleColorsDark(); 		// Setup Dear ImGui style

	// Setup Platform/Renderer ImGui backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const auto glsl_version = "#version 410";
	ImGui_ImplOpenGL3_Init(glsl_version);

	#if defined(__WIN64__)
		debugGL(); // Setup callback to catch openGL errors.	V4.2 or newer
	#elif(__APPLE__)
		glCheckError(); // Old way of checking for errors. Newest not implemented by Apple. Manually call function anywhere in code to check for errors.
	#endif

	glfwSwapInterval(1);			 // Ony render when synced (V SYNC) - https://www.tomsguide.com/features/what-is-vsync-and-should-you-turn-it-on-or-off
	glfwWindowHint(GLFW_SAMPLES, 2); // Multisampling - covered in lectures - https://www.khronos.org/opengl/wiki/Multisampling

	startup(); // Setup all necessary information for startup (aka. load texture, shaders, models, etc).

	cout << endl << "Starting main loop and rendering..." << endl;	

	do{											 // run until the window is closed
		auto currentTime = (float)glfwGetTime(); // retrieve timelapse
		deltaTime = currentTime - lastTime;		 // Calculate delta time
		lastTime = currentTime;					 // Save for next frame calculations.

		glfwPollEvents(); 						// poll callbacks

		update(); 								// update (physics, animation, structures, etc)
		render(); 								// call render function.
		ui();									// call function to render ui.

		#if defined(__APPLE__)
			glCheckError();				// Manually checking for errors for MacOS, Windows has a callback.
		#endif

		glfwSwapBuffers(window); 		// swap buffers (avoid flickering and tearing)

		running &= (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE); // exit if escape key pressed
		running &= (glfwWindowShouldClose(window) != GL_TRUE);
	} while (running);

	endProgram(); // Close and clean everything up...

	cout << "\nPress any key to continue...\n";
	cin.ignore(); cin.get(); // delay closing console to read debugging errors.

	return 0;
}


void hintsGLFW(){
	
	auto majorVersion = 3; auto minorVersion = 3; // define OpenGL version - at least 3.3 for bare basic NEW OpenGL

	#if defined(__WIN64__)	
		majorVersion = 4; minorVersion = 5;					// Recommended for Windows 4.5, but latest is 4.6 (not all drivers support 4.6 version).
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // Create context in debug mode - for debug message callback
	#elif(__APPLE__)
		majorVersion = 4; minorVersion = 1; 				// Max support for OpenGL in MacOS
	#endif

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 	// https://www.glfw.org/docs/3.3/window_guide.html
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
}

void endProgram()
{
	// Clean ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwMakeContextCurrent(NULL); 	// destroys window handler
	glfwTerminate();				// destroys all windows and releases resources.
}

void startup()
{
	// Output some debugging information
	cout << "VENDOR: " << (char *)glGetString(GL_VENDOR) << endl;
	cout << "VERSION: " << (char *)glGetString(GL_VERSION) << endl;
	cout << "RENDERER: " << (char *)glGetString(GL_RENDERER) << endl;	

	cout << endl << "Loading content..." << endl;	

	rocks_light.LoadGLTF("assets/rocks-1.gltf");
	rocks_medium.LoadGLTF("assets/rocks-2.gltf");
	rocks_dark.LoadGLTF("assets/rocks-3.gltf");
	roots.LoadGLTF("assets/roots.gltf");
	leaves.LoadGLTF("assets/leaves.gltf");

	floor_1.LoadGLTF("assets/floor.gltf");
	floor_2.LoadGLTF("assets/floor-2.gltf");
	floor_3.LoadGLTF("assets/floor-3.gltf");
	floor_4.LoadGLTF("assets/floor-4.gltf");

	

	terrain.LoadGLTF("assets/terrain.gltf");

	roof_1.LoadGLTF("assets/roof-1.gltf");
	roof_decor.LoadGLTF("assets/roof-decor.gltf");
	roof_top.LoadGLTF("assets/roof-top.gltf");

	columns.LoadGLTF("assets/columns.gltf");

	portals.LoadGLTF("assets/portals.gltf");

	inside_path.LoadGLTF("assets/inside-path.gltf");



	knight_body.LoadGLTF("assets/knight-body.gltf");
	knight_helm_boots.LoadGLTF("assets/knight-helm-boots.gltf");
	knight_sword.LoadGLTF("assets/knight-sword.gltf");
	shield.LoadGLTF("assets/shield.gltf");


	pipeline.CreatePipeline();
	pipeline.LoadShaders("shaders/vs_model.glsl", "shaders/fs_model.glsl");

	// Start from the centre
	modelPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	modelRotation = glm::vec3(0.0f, 0.0f, 0.0f);

	lightRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	knightRotation = glm::vec3(0.0f, 180.0f, 0.0f);
	knightPosition = glm::vec3(-25.0f, 1.35f, 0.0f);


	light_source.LoadGLTF("assets/light-source.gltf");

	// A few optimizations.
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Get the correct size in pixels - E.g. if retina display or monitor scaling
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

	// Calculate proj_matrix for the first time.
	aspect = (float)windowWidth / (float)windowHeight;
	projMatrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
}

void update()
{
	if (keyStatus[GLFW_KEY_LEFT]) modelRotation.y += 0.05f;
	if (keyStatus[GLFW_KEY_RIGHT]) modelRotation.y -= 0.05f;
	if (keyStatus[GLFW_KEY_UP]) modelRotation.x += 0.05f;
	if (keyStatus[GLFW_KEY_DOWN]) modelRotation.x -= 0.05f;

	if (keyStatus[GLFW_KEY_R]) pipeline.ReloadShaders();

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

vec4 lightPos = vec4(1.0f, 1.0f, 1.0f, 1.0f);

void render()
{
	glViewport(0, 0, windowWidth, windowHeight);
	
	// glUniform4f(glGetUniformLocation(pipeline.pipe.program, "lightPosition"), lightPos.x, lightPos.y, lightPos.z, 1.0);
	

	// Clear colour buffer
	glm::vec4 inchyraBlue = glm::vec4(0.345f, 0.404f, 0.408f, 1.0f);
	glm::vec4 backgroundColor = inchyraBlue;
	glClearBufferfv(GL_COLOR, 0, &backgroundColor[0]);

	// Clear deep buffer
	static const GLfloat one = 1.0f;
	glClearBufferfv(GL_DEPTH, 0, &one);

	// Enable blend
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Use our shader programs
	glUseProgram(pipeline.pipe.program);

	// Setup camera
	glm::mat4 viewMatrix = glm::lookAt(cameraPosition,				 // eye
									   cameraPosition + cameraFront, // centre
									   cameraUp);					 // up

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.20f, 0.2f));

	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);

	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "ia"), ia.r, ia.g, ia.b, 1.0);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "ka"), ka);
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "id"), id.r, id.g, id.b, 1.0);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "kd"), 1.0f);
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "is"), is.r, is.g, is.b, 1.0);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "ks"), 1.0f);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "shininess"), 32.0f);
	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "proj_matrix"), 1, GL_FALSE, &projMatrix[0][0]);

	terrain.DrawModel(terrain.vaoAndEbos, terrain.model);


	

	columns.DrawModel(columns.vaoAndEbos, columns.model);

	rocks_light.DrawModel(rocks_light.vaoAndEbos, rocks_light.model);
	rocks_medium.DrawModel(rocks_medium.vaoAndEbos, rocks_medium.model);
	rocks_dark.DrawModel(rocks_dark.vaoAndEbos, rocks_dark.model);
	leaves.DrawModel(leaves.vaoAndEbos, leaves.model);
	
	
	floor_1.DrawModel(floor_1.vaoAndEbos, floor_1.model);
	floor_2.DrawModel(floor_2.vaoAndEbos, floor_2.model);
	floor_3.DrawModel(floor_3.vaoAndEbos, floor_3.model);
	floor_4.DrawModel(floor_4.vaoAndEbos, floor_4.model);
	roof_1.DrawModel(roof_1.vaoAndEbos, roof_1.model);
	roof_decor.DrawModel(roof_decor.vaoAndEbos, roof_decor.model);
	roof_top.DrawModel(roof_top.vaoAndEbos, roof_top.model);
	roots.DrawModel(roots.vaoAndEbos, roots.model);
	
	
	inside_path.DrawModel(inside_path.vaoAndEbos, inside_path.model);

	if(portalsToggle == 1) {
		portals.DrawModel(portals.vaoAndEbos, portals.model);
	}

	// --------------
	// NIGHT
	// --------------

	if(windToggle == 1) {
		knightPosition.x -= 0.1;
	}

	glm::mat4 knightMatrix = glm::mat4(1.0);
	
	knightMatrix = glm::rotate(knightMatrix, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	knightMatrix = glm::rotate(knightMatrix, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	knightMatrix = glm::translate(knightMatrix, knightPosition);
	knightMatrix = glm::rotate(knightMatrix, knightRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	knightMatrix = glm::rotate(knightMatrix, knightRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	knightMatrix = glm::scale(knightMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
	

	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &knightMatrix[0][0]);

	knight_body.DrawModel(knight_body.vaoAndEbos, knight_body.model);
	knight_helm_boots.DrawModel(knight_helm_boots.vaoAndEbos, knight_helm_boots.model);
	knight_sword.DrawModel(knight_sword.vaoAndEbos, knight_sword.model);
	shield.DrawModel(shield.vaoAndEbos, shield.model);
	
	// --------------
	// ROTATING LIGHT
	// --------------
	// glm::mat4 columnsMatrix = glm::mat4(1.0);
	
	// columnsMatrix = glm::rotate(columnsMatrix, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	// columnsMatrix = glm::rotate(columnsMatrix, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	// columnsMatrix = glm::translate(columnsMatrix, glm::vec3(0.0f, 0.0f, 0.0f));

	// columnsMatrix = glm::rotate(columnsMatrix, lightRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	// columnsMatrix = glm::rotate(columnsMatrix, lightRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	// columnsMatrix = glm::rotate(columnsMatrix, lightRotation.z += 0.01f, glm::vec3(0.0f, 0.0f, 1.0f));

	// columnsMatrix = glm::translate(columnsMatrix, glm::vec3(0.0f, 40.0f, 0.0f));
	// columnsMatrix = glm::scale(columnsMatrix, glm::vec3(2.0f, 2.0f, 2.0f));

	// glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &columnsMatrix[0][0]);

	// light_source.DrawModel(light_source.vaoAndEbos, light_source.model);
	
	
	glm::mat4 lightM = glm::mat4(1.0f);

	lightM = glm::rotate(lightM, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	lightM = glm::rotate(lightM, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	lightM = glm::translate(lightM, glm::vec3(0.0f, 0.0f, 0.0f));

	lightM = glm::rotate(lightM, lightRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	lightM = glm::rotate(lightM, lightRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	lightM = glm::rotate(lightM, lightRotation.z += 0.01f, glm::vec3(0.0f, 0.0f, 1.0f));

	lightM = glm::translate(lightM, glm::vec3(0.0f, 40.0f, 0.0f));

	glm::vec4 rotatedLightPos = lightM * lightPos;

	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "lightPosition"), rotatedLightPos.x, rotatedLightPos.y, rotatedLightPos.z, 1.0);

	
	
	#if defined(__APPLE__)
		glCheckError();
	#endif
}

void ui()
{
	ImGuiIO &io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration; 
	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
	window_flags |= ImGuiWindowFlags_NoSavedSettings; 
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing; 
	window_flags |= ImGuiWindowFlags_NoNav;

	const auto PAD = 10.0f;
	const ImGuiViewport *viewport = ImGui::GetMainViewport();
	ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
	ImVec2 work_size = viewport->WorkSize;
	ImVec2 window_pos, window_pos_pivot;
	window_pos.x = work_pos.x + work_size.x - PAD;
	window_pos.y = work_pos.y + work_size.y - PAD;
	window_pos_pivot.x = 1.0f;
	window_pos_pivot.y = 1.0f;

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	window_flags |= ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	bool *p_open = NULL;
	if (ImGui::Begin("Info", nullptr, window_flags)) {
		ImGui::Text("About: 3D Graphics and Animation 2022"); // ImGui::Separator();
		ImGui::Text("Performance: %.3fms/Frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Pipeline: %s", pipeline.pipe.error?"ERROR":"OK");
	}
	ImGui::End();

	// Rendering imgui
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void onResizeCallback(GLFWwindow *window, int w, int h)
{
	windowWidth = w;
	windowHeight = h;

	// Get the correct size in pixels
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

	if (windowWidth > 0 && windowHeight > 0)
	{ // Avoid issues when minimising window - it gives size of 0 which fails division.
		aspect = (float)w / (float)h;
		projMatrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
	}
}

void onKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(action == 1) {
		if (key == 77) {
			cout << "Toggling Wind!" << endl;
			if(windToggle == 1) {
				windToggle = 0;
			} else {
				windToggle = 1;
			}
		} else if (key == 80) { // KEY P
			cout << "Toggling Portals!" << endl;
			if(portalsToggle == 1) {
				portalsToggle = 0;
			} else {
				portalsToggle = 1;
			}
		}
	}
	if (action == GLFW_PRESS)
		keyStatus[key] = true;
		if(key == 68) {
			cameraPosition.x += 0.5;
		}
		else if(key == 87) {
			cameraPosition.z -= 0.5;
		}
		else if (key == 65) {
			cameraPosition.x -= 0.5;
		} 
		else if (key == 83) {
			cameraPosition.z += 0.5;
		}
		else if (key == 83) {
			cameraPosition.z += 0.5;
		}
		else if (key == 89) {
			cameraPosition.y += 0.5;
		}
		else if (key == 72) {
			cameraPosition.y -= 0.5;
		}

		else if (key == 73) { // KEY I
			knightPosition.x += 0.2;
		}
		else if (key == 74) { // KEY J
			knightPosition.z -= 0.2;
		}
		else if (key == 75) { // KEY K
			knightPosition.x -= 0.2;
		}
		else if (key == 76) { // KEY L
			knightPosition.z += 0.2;
		}
		
		else if (key == 32) {
			jump += 1;
			knightPosition.y += 0.5;
		}
	else if (action == GLFW_RELEASE)
		keyStatus[key] = false;
		

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void onMouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
}

void onMouseMoveCallback(GLFWwindow *window, double x, double y)
{
	int mouseX = static_cast<int>(x);
	int mouseY = static_cast<int>(y);
	cameraUp.y += mouseY;
	// cameraFront.z += mouseX;
	// cout << "Mouse move: " << mouseX << ", " << mouseY << endl;
}

void onMouseWheelCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	int yoffsetInt = static_cast<int>(yoffset);
}

void APIENTRY openGLDebugCallback(GLenum source,
								  GLenum type,
								  GLuint id,
								  GLenum severity,
								  GLsizei length,
								  const GLchar *message,
								  const GLvoid *userParam)  // Debugger callback for Win64 - OpenGL versions 4.2 or better.
{
	debugger.OpenGLDebugCallback(source, type, id, severity, length, message, userParam);
}

void debugGL() // Debugger function for Win64 - OpenGL versions 4.2 or better.
{
	// Enable Opengl Debug
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback((GLDEBUGPROC)openGLDebugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
}

GLenum glCheckError_(const char *file, int line) // Debugger manual function for Apple - OpenGL versions 3.3 to 4.1.
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) debugger.GlGetError(errorCode, file, line);

	return errorCode;
}
