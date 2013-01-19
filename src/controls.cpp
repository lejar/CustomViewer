// Include GLFW
#include <GL/glfw.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <AntTweakBar.h>

#include "controls.h"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}

// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 0, 5 ); 
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 15.0f;
float mouseSpeed = 0.005f;
bool prevrMouseDown = false;
bool prevmMouseDown = false;
bool GUIactive = false;


void computeMatricesFromInputs(){

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	bool rMouseDown = (glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
	bool mMouseDown = (glfwGetMouseButton(GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
	if(rMouseDown && prevrMouseDown && !mMouseDown)
	{
		// Get mouse position
		int xpos, ypos;
		glfwGetMousePos(&xpos, &ypos);

		// Reset mouse position for next frame
		glfwSetMousePos(1024/2, 768/2);

		// Compute new orientation
		horizontalAngle += mouseSpeed * float(1024/2 - xpos );
		verticalAngle   += mouseSpeed * float( 768/2 - ypos );

	}
	else if(rMouseDown && !prevrMouseDown && !GUIactive)
	{
		// Reset mouse position for next frame
		glfwSetMousePos(1024/2, 768/2);
		prevrMouseDown = true;
	}
	else
	{
		prevrMouseDown = false;
	}
	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle), 
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	
	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
	
	// Up vector
	glm::vec3 up = glm::cross( right, direction );

	// Move forward
	if (glfwGetKey( GLFW_KEY_UP ) == GLFW_PRESS || glfwGetKey('W') == GLFW_PRESS){
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( GLFW_KEY_DOWN ) == GLFW_PRESS || glfwGetKey('S') == GLFW_PRESS){
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( GLFW_KEY_RIGHT ) == GLFW_PRESS || glfwGetKey('D') == GLFW_PRESS){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( GLFW_KEY_LEFT ) == GLFW_PRESS || glfwGetKey('A') == GLFW_PRESS){
		position -= right * deltaTime * speed;
	}

	//handle moving straight up, down, left and right
	if(mMouseDown && prevmMouseDown && !rMouseDown)
	{
		// Get mouse position
		int xpos, ypos;
		glfwGetMousePos(&xpos, &ypos);

		// Reset mouse position for next frame
		glfwSetMousePos(1024/2, 768/2);

		// Compute new orientation
		if(float(1024/2 - xpos) > 0)
			position -= right*deltaTime*speed*5.0f;
		else if(float(1024/2 - xpos) < 0)
			position += right*deltaTime*speed*5.0f;

		if(float(768/2 - ypos) > 0)
			position += up*deltaTime*speed*5.0f;
		else if(float(768/2 - ypos) < 0)
			position -= up*deltaTime*speed*5.0f;

	}
	else if(mMouseDown && !prevmMouseDown && !GUIactive)
	{
		// Reset mouse position for next frame
		glfwSetMousePos(1024/2, 768/2);
		prevmMouseDown = true;
	}
	else
	{
		prevmMouseDown = false;
	}

	float FoV = initialFoV;
	
	//TODO: Zoom with moving the camera instead of changing FoV!!!
	//if(!GUIactive) 
	//	FoV -= 5*glfwGetMouseWheel();

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 300.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
								position+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}

void GLFWCALL OnMouseButton(int glfwButton, int glfwAction)
{
	//if(!prevrMouseDown)
    {
	    if( !TwEventMouseButtonGLFW(glfwButton, glfwAction) )   // Send event to AntTweakBar
	    {
	    	GUIactive = false;
	        // Event has not been handled by AntTweakBar
	        // Do something if needed.
	    }
	    else
	    	GUIactive = true;
	}
}

// Callback function called by GLFW when mouse has moved
void GLFWCALL OnMousePos(int mouseX, int mouseY)
{
    if( !TwEventMousePosGLFW(mouseX, mouseY) )  // Send event to AntTweakBar
    {
        // Event has not been handled by AntTweakBar
        // Do something if needed.
    }
}

// Callback function called by GLFW on mouse wheel event
void GLFWCALL OnMouseWheel(int pos)
{
    if( !TwEventMouseWheelGLFW(pos) )   // Send event to AntTweakBar
    {
    	GUIactive = false;
    }
    else
    	GUIactive = true;
}
