#ifndef SHADER_H
#define SHADER_H

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

class shader
{
public:
	shader();
	~shader();
};

#endif
