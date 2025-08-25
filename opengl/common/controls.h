#ifndef CONTROLS_H
#define CONTROLS_H

void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();

class controls
{
public:
	controls();
	~controls();
};

class control
{
public:
	control();
	~control();
};

#endif