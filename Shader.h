/* @Author Utkarsha Khanal
Date:2019 

This is a very easy to use header file include this header file into your OpenGL Program. This header file loads the Shader program into your OpenGL Program.

Example to load the Shader are

Shader myShader(vertShaderPath, fragShaderPath); 
Shader myShader(vertShaderPath, fragShaderPath, geometryShaderPath);

You are free to do anything with this source code.

*/


#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

class Shader
{
	public:
		GLuint Program;
		//constructor 
		Shader(const GLchar* vertexPath,const GLchar* fragmentPath)
		{
			std::string vertexCode;
			std::string fragmentCode;
			std::ifstream vShaderFile;
			std::ifstream fShaderFile;
		
			vShaderFile.exceptions(std::ifstream::badbit);
			fShaderFile.exceptions(std::ifstream::badbit);
			try
			{
				vShaderFile.open(vertexPath);
				fShaderFile.open(fragmentPath);
				std::stringstream vShaderStream,fShaderStream;
			
				vShaderStream<<vShaderFile.rdbuf();
				fShaderStream<<fShaderFile.rdbuf();

				vShaderFile.close();
				fShaderFile.close();


				vertexCode=vShaderStream.str();
				fragmentCode=fShaderStream.str();

			}
			catch(std::ifstream::failure e)
			{
				std::cout<<"ERROR::SHADER::FILE_NOT_SUCCESSUFUL_READ"<<std::endl;
			}

			const GLchar* vShaderCode=vertexCode.c_str();
			const GLchar* fShaderCode=fragmentCode.c_str();

			GLuint vertex,fragment;
			GLint success;
			GLchar infoLog[512];

			vertex=glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex,1,&vShaderCode,NULL);
			glCompileShader(vertex);

			glGetShaderiv(vertex,GL_COMPILE_STATUS,&success);
			if(!success)
			{
				glGetShaderInfoLog(vertex,512,NULL,infoLog);
				std::cout<<"ERROR::SHADER::VERTEX::COMPILATION\n"<<infoLog<<std::endl;
			}

			fragment=glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment,1,&fShaderCode,NULL);
			glCompileShader(fragment);
			glGetShaderiv(fragment,GL_COMPILE_STATUS,&success);
			
			if(!success)
			{
				glGetShaderInfoLog(fragment,512,NULL,infoLog);
				std::cout<<"ERROR::SHADER::FRAGMENT::COMPILATION\n"<<infoLog<<std::endl;
			}

			this->Program=glCreateProgram();
			glAttachShader(this->Program,vertex);
			glAttachShader(this->Program,fragment);
			glLinkProgram(this->Program);


			glGetProgramiv(this->Program,GL_LINK_STATUS,&success);
			if(!success)
			{
				glGetProgramInfoLog(this->Program,512,NULL,infoLog);
				std::cout<<"ERROR::SHADER::PROGRAM::LINKING::FAILED\n"<<infoLog<<std::endl;
			}
			glDeleteShader(vertex);
			glDeleteShader(fragment);
		}

		Shader(const GLchar* vertPath, const GLchar* fragPath,const GLchar* geomPath)
		{
			std::string vertexCode;
			std::string fragCode;
			std::string geomCode;

			std::ifstream vShaderFile;
			std::ifstream fShaderFile;
			std::ifstream gShaderFile;

			vShaderFile.exceptions(std::ifstream::badbit);
			fShaderFile.exceptions(std::ifstream::badbit);
			gShaderFile.exceptions(std::ifstream::badbit);
			try
			{
				vShaderFile.open(vertPath);
				fShaderFile.open(fragPath);
				gShaderFile.open(geomPath);
				std::stringstream vShaderStream,fShaderStream,gShaderStream;
			
				vShaderStream<<vShaderFile.rdbuf();
				fShaderStream<<fShaderFile.rdbuf();
				gShaderStream<<gShaderFile.rdbuf();

				vShaderFile.close();
				fShaderFile.close();
				gShaderFile.close();


				vertexCode=vShaderStream.str();
				fragCode=fShaderStream.str();
				geomCode=gShaderStream.str();

			}
			catch(std::ifstream::failure e)
			{
				std::cout<<"ERROR::SHADER::FILE_NOT_SUCCESSUFUL_READ"<<std::endl;
			}

			const GLchar* vShaderCode=vertexCode.c_str();
			const GLchar* fShaderCode=fragCode.c_str();
			const GLchar* gShaderCode=geomCode.c_str();

			GLuint vertex,fragment,geometry;
			GLint success;
			GLchar infoLog[512];

			vertex=glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex,1,&vShaderCode,NULL);
			glCompileShader(vertex);
		
			glGetShaderiv(vertex,GL_COMPILE_STATUS,&success);
	
			if(!success)
			{
				glGetShaderInfoLog(vertex,512,NULL,infoLog);
				std::cout<<"ERROR::VERTEX::SHADER::COMPILATION"<<infoLog<<std::endl;
			}
			
			geometry=glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry,1,&gShaderCode,NULL);
			glCompileShader(geometry);

			glGetShaderiv(geometry,GL_COMPILE_STATUS,&success);
			if(!success)
			{
				glGetShaderInfoLog(geometry,512,NULL,infoLog);
				std::cout<<"ERROR::GEOMETRY::SHADER::COMPILATION"<<infoLog<<std::endl;
			}

			fragment=glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment,1,&fShaderCode,NULL);
			glCompileShader(fragment);
			
			glGetShaderiv(fragment,GL_COMPILE_STATUS,&success);

			if(!success)
			{
				glGetShaderInfoLog(fragment,512,NULL,infoLog);
				std::cout<<"ERROR::FRAGMENT::SHADER::COMPILATION"<<infoLog<<std::endl;
			}

			this->Program=glCreateProgram();
			glAttachShader(this->Program,vertex);
			glAttachShader(this->Program,geometry);
			glAttachShader(this->Program,fragment);
			glLinkProgram(this->Program);

			glGetShaderiv(this->Program,GL_LINK_STATUS,&success);
			if(!success)
			{
				glGetShaderInfoLog(this->Program,512,NULL,infoLog);
				std::cout<<"ERROR::SHADER::LINKING::FAILED"<<infoLog<<std::endl;
			}

			glDeleteShader(vertex);
			glDeleteShader(geometry);
			glDeleteShader(fragment);


				
		}

		void Use()
		{
			glUseProgram(this->Program);
		} 
};
#endif


