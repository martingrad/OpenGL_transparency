// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

// OpenGL includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM includes
#include <glm/glm.hpp>
using namespace glm;

// AntTweakBar includes
#include <AntTweakBar.h>

// Source includes
#include "../extern/OpenGL_Graphics_Engine/include/Scene.h"

// Functions
bool initOpenGL(void);
bool initScene(void);
void initAntTweakBar(void);

// Controls
void magicTwMouseButtonWrapper(GLFWwindow *, int, int, int);
void magicTwMouseHoverWrapper(GLFWwindow *, double, double);
void myFunction(void *clientData);

void updateTweakBar(void);
void updateLightPosition();

// Variables
GLFWwindow* window;
Scene* scene;
Mesh* lightMesh;
TwBar* tweakbar;
glm::vec3 lightPosition = glm::vec3(0.0, 0.0, -1.0);
int frame = 0;

// Constants
#define WIDTH 1024
#define HEIGHT 768

int main(void)
{
	if(!initOpenGL())
	{
		return -1;
	}

	printf("Supported GLSL version is %s.\n", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
	
	// Initialize the scene
	initScene();

	// Initialize the AntTweakBar window 
	initAntTweakBar();

	// Render the window
	do
	{
		// Clear the screen and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		updateTweakBar();

		updateLightPosition();

		scene->render(window);

		// Render the AntTweakBar (after the meshes)
		TwDraw();

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		frame++;
	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Remove AntTweakBar
    TwTerminate();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

bool initOpenGL(void)
{
	// Initialise GLFW
	if(!glfwInit())
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Projekt okänt", NULL, NULL);
	if( window == NULL )
	{
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if(glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return false;
	}
	return true;
}

bool initScene(void)
{
	// Initialize scene
	scene = new Scene();

	int texHeight = 1024;
	int texWidth  = 1024;

	// Create and add deer mesh
	Mesh* deerMesh = new Mesh();
	deerMesh->initShaders("extern/OpenGL_Graphics_Engine/shaders/vertexshader.glsl", "shaders/fragmentshader.glsl");
	deerMesh->initOBJ("assets/models/deer_bl_exp.obj");
	deerMesh->setColorMap("assets/textures/white.png", texHeight, texWidth);
	deerMesh->setMaterialProperties(0.9, 0.10, 5.0);	// diffuse and specular coeff, specular power
	deerMesh->setPosition(0.5, -2.5, -2.0);
	deerMesh->setTexture("assets/textures/ao_deer_inv.png", "aoMap", GL_TEXTURE4, 4, 512, 512);
	deerMesh->addVec3Uniform("lightPos_ws", glm::value_ptr(lightPosition));
	scene->addMesh(deerMesh);

	// Create and add sphere at light source position
	lightMesh = new Mesh();
	lightMesh->initShaders("shaders/lightSourceVertexShader.glsl", "shaders/lightSourceFragmentShader.glsl");
	lightMesh->initOBJ("extern/OpenGL_Graphics_Engine/assets/sphere.obj");
	lightMesh->setMaterialProperties(1.0, 0.0, 40.0);	// diffuse and specular coeff, specular power
	lightMesh->setPosition(lightPosition.x, lightPosition.y, lightPosition.z);
	lightMesh->scaleObject(0.05);
	scene->addMesh(lightMesh); // TODO: Find out what's wrong with the position

	return true;
}

/****************************** <AntTweakBar> *********************************/

float testVariable = 10.0f;
/**
 *   Initialize the AntTweakBar window and add its variables
**/
void initAntTweakBar(void)
{
    // Scale the font, since AntTweakBar doesn't like retina displays
    TwDefine(" GLOBAL fontscaling=2 ");

    // Initialize AntTweakBar
    TwInit(TW_OPENGL_CORE, NULL);       // for core profile

    // Set the size of the graphic window
    TwWindowSize(WIDTH * 1.96, HEIGHT * 1.96);			// for mac retina 13
    // TwWindowSize(WIDTH * 1.99, HEIGHT * 1.99);			// for mac retina 15

    // // Create a new tweak bar (by calling TWNewBar) and set its size
    tweakbar = TwNewBar("Emma");
    TwDefine("Emma size='400 700'");

    /**
    * Add variables to the tweak bar
    **/
    TwAddVarRW( tweakbar,           		// my tweak bar
            	"That's Me",          		// name of my variable
            	TW_TYPE_FLOAT,      		// tweak bar type
            	&testVariable,       		// my variable
           		"min=0 max=2 step=0.05 help=':D'" 
           		);

    TwAddVarRW( tweakbar,           		// my tweak bar
            	"Martin",        			// name of my variable
            	TW_TYPE_FLOAT,      		// tweak bar type
            	&testVariable,       		// my variable
           		" group='Stockholm' label='Martin' min=0 max=2 step=0.05 help='man' "
           		);

    TwAddButton( tweakbar, 
    			 "comment1",
    			 &myFunction,
    			 NULL,
    			 " label='Life is like a box a chocolates' "
    			 ); 
	
	glfwSetMouseButtonCallback(window, magicTwMouseButtonWrapper);
    glfwSetCursorPosCallback(window, magicTwMouseHoverWrapper);

}

void myFunction(void *clientData)
{
	std::cout << "Hej på mig igen " << std::endl;	
}

void magicTwMouseButtonWrapper(GLFWwindow* window, int button, int action, int mods)
{
 	TwEventMouseButtonGLFW(button, action);
}

void magicTwMouseHoverWrapper(GLFWwindow * window, double x, double y)
{
    TwEventMousePosGLFW(x * 2, y * 2);
}

void updateTweakBar(void)
{
	// Call the setFunctions for those uniform variables 
	// that you want to be updated! 
}

/****************************** </AntTweakBar> *********************************/

void updateLightPosition(void)
{
	lightPosition.z = -1.0 + (4.0 * std::sin(0.01 * frame));
	lightPosition.x = (3.0 * std::cos(0.01 * frame));
	lightMesh->setPosition(lightPosition.x, lightPosition.y, lightPosition.z);
	// std::cout << lightPosition.z << std::endl;
}
