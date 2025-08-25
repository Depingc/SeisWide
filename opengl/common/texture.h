#ifndef TEXTURE_H
#define TEXTURE_H

// Load a .BMP file using our custom loader
GLuint loadBMP_custom(const char * imagepath);

//// Since GLFW 3, glfwLoadTexture2D() has been removed. You have to use another texture loading library, 
//// or do it yourself (just like loadBMP_custom and loadDDS)
//// Load a .TGA file using GLFW's own loader
//GLuint loadTGA_glfw(const char * imagepath);

// Load a .DDS file using GLFW's own loader
GLuint loadDDS(const char * imagepath);

class texture
{
public:
	texture();
	~texture();
};

class texture1
{
public:
	texture1();
	~texture1();
};

class texture2
{
public:
	texture2();
	~texture2();
};


#endif