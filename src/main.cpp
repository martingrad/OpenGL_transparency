// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

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
void updateTweakBar(void);

// Controls
void magicTwMouseButtonWrapper(GLFWwindow *, int, int, int);
void magicTwMouseHoverWrapper(GLFWwindow *, double, double);
void toggleDisplacement(void *clientData);
void toggleShowTriangles(void *clientData);
void toggleBezier(void *clientData);

// Variables
GLFWwindow* window;
Scene* scene;
TwBar* tweakbar;

Mesh* tessellatedMesh;

// AntTweakBar variables
float tessScale 		= 1.0;
float dispScale 		= 1.0;
int dispEnabled 		= 0.0;
int trianglesEnabled 	= 0.0;
int bezierEnabled		= 0.0;

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

		scene->render(window);

		// Render the AntTweakBar (after the meshes)
		TwDraw();

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
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
	window = glfwCreateWindow( 1024, 768, "Dynamic Tessellation", NULL, NULL);
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

	// Create and add a mesh to the scene
	tessellatedMesh = new Mesh();
	// tessellatedMesh->initShaders("shaders/vertexshader.glsl", "shaders/fragmentshader.glsl");
    tessellatedMesh->initShaders( 	"shaders/tessellation/vertexshader.glsl",
                                 	"shaders/tessellation/tessellationcontrolshader.glsl",
                                    "shaders/tessellation/tessellationevaluationshader.glsl",
                                    "shaders/tessellation/geometryshader.glsl",
                                    "shaders/tessellation/fragmentshader.glsl" );
    
	tessellatedMesh->initOBJ("extern/OpenGL_Graphics_Engine/assets/susanne.obj");
	tessellatedMesh->setDispMap("assets/textures/dispMap.png", texHeight, texWidth);
	tessellatedMesh->setNormMap("assets/textures/normMap.png", texHeight, texWidth);
	tessellatedMesh->setColorMap("assets/textures/bunny_tex.png", texHeight, texWidth);
	tessellatedMesh->setMaterialProperties(0.5, 0.5, 40.0);	// diffuse and specular coeff, specular power
	tessellatedMesh->setPosition(0.0, 0.0, -10.0);
	tessellatedMesh->scaleObject(5.0);
	tessellatedMesh->addFloatUniform("tessScale", 1.0);
	tessellatedMesh->addFloatUniform("dispScale", 1.0);
	tessellatedMesh->addIntegerUniform("dispEnabled", dispEnabled);
	tessellatedMesh->addIntegerUniform("trianglesEnabled", trianglesEnabled);
	tessellatedMesh->addIntegerUniform("bezierEnabled", bezierEnabled);
	scene->addMesh(tessellatedMesh);
}

/****************************** <AntTweakBar> *********************************/

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
    tweakbar = TwNewBar("Properties");
    TwDefine("Properties size='400 700'");

    /**
    * Add variables to the tweak bar
    **/
    TwAddVarRW( tweakbar,           		// my tweak bar
            	"Tessellation",        // name of my variable
            	TW_TYPE_FLOAT,      		// tweak bar type
            	&tessScale,       			// my variable
           		"min=0.05 max=5 step=0.05 help=':D'" 
           		);

    TwAddVarRW( tweakbar,           		// my tweak bar
            	"Displacement",        			// name of my variable
            	TW_TYPE_FLOAT,      		// tweak bar type
            	&dispScale,       			// my variable
           		"min=0 max=5 step=0.05 help='displacement scale'"
           		);

    TwAddButton( tweakbar,
    			 "show/hide triangles",
    			 &toggleShowTriangles,
    			 NULL,
    			 " group='Toggle' label='Toggle triangles' "
    			 );

    TwAddButton( tweakbar,
    			 "comment1",
    			 &toggleDisplacement,
    			 NULL,
    			 " group='Toggle' label='Toggle displacement' "
    			 );

    TwAddButton( tweakbar,
    			 "bezier",
    			 &toggleBezier,
    			 NULL,
    			 " group='Toggle' label='Toggle smoothing' "
    			 ); 
	
	glfwSetMouseButtonCallback(window, magicTwMouseButtonWrapper);
    glfwSetCursorPosCallback(window, magicTwMouseHoverWrapper);

}

void toggleDisplacement(void *clientData)
{
	dispEnabled = !dispEnabled;
	tessellatedMesh->updateIntegerUniform("dispEnabled", dispEnabled);
}

void toggleBezier(void *clientData)
{
	bezierEnabled = !bezierEnabled;
	tessellatedMesh->updateIntegerUniform("bezierEnabled", bezierEnabled);
}

void toggleShowTriangles(void *clientData)
{
	trianglesEnabled = !trianglesEnabled;
	tessellatedMesh->updateIntegerUniform("trianglesEnabled", trianglesEnabled);
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
	tessellatedMesh->updateFloatUniform("tessScale", tessScale);
	tessellatedMesh->updateFloatUniform("dispScale", dispScale);
}

/****************************** </AntTweakBar> *********************************/