#ifndef CONTROLS_H_
#define CONTROLS_H_

void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
void GLFWCALL OnMouseButton(int glfwButton, int glfwAction);
void GLFWCALL OnMousePos(int mouseX, int mouseY);
void GLFWCALL OnMouseWheel(int pos);

#endif