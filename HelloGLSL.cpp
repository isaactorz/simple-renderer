#include <OpenGL/gl3.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "obj_loader.hpp"
#include "Camera2.hpp"
#include <stdio.h>
#include <vector>
#include <iostream>
#include <string>
#include <fstream> 
#include <sstream>

struct {
  enum {
    DIFFUSE,
    AMBIENT,
    SPECULAR 
  } primative;
} state;

struct {
  glm::vec4 La;
  glm::vec4 Ld;
  glm::vec4 Ls;
  glm::vec4 global_ambient;
  glm::vec4 light_position;
} LightProperties;

struct {
  glm::vec4 Ka;
  glm::vec4 Kd;
  glm::vec4 Ks;
  GLfloat shininess;
} MaterialProperties;

std::vector<glm::vec3> vertices;
std::vector<glm::vec2> uvs;
std::vector<glm::vec3> normals;
std::vector<glm::vec3> gourad_normals;
std::vector<glm::vec3> flat_normals;
// for GLSL
GLuint vsID, fsID, pID, VBO, VAO, VNO, EBO;

GLint transformLocation,viewLocation,lightDirLocation,
  matDiffuseLocation,lightDiffuseLocation,matSpecLocation,
  lightSpecLocation,matAmbientLocation,lightAmbientLocation,
  shininessLocation,globalAmbientLocation;

Camera2 camera;

GLenum shade_model;

glm::vec3 color;

float mNear, mFar;

void readShaderFile(const GLchar* shaderPath, std::string& shaderCode)
{

	std::ifstream shaderFile;
	
	// ensures ifstream objects can throw exceptions:
	shaderFile.exceptions(std::ifstream::badbit);
	try
	{
		// Open files
		shaderFile.open(shaderPath);
		std::stringstream shaderStream;

		// Read file's buffer contents into streams
		shaderStream << shaderFile.rdbuf();
				
		// close file handlers
		shaderFile.close();
		
		// Convert stream into GLchar array
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

}

void setShaders()
{
	char *vs = NULL, *fs = NULL;
	
	std::string vertShaderString; //empty string
	std::string fragShaderString; //empty string

	vsID = glCreateShader(GL_VERTEX_SHADER);
  if (vsID == 0) printf("Error creating vertex shader\n");

	fsID = glCreateShader(GL_FRAGMENT_SHADER);
  if (fsID == 0) printf("Error creating frag shader\n");

	readShaderFile("vertexshader2.txt", vertShaderString);
	readShaderFile("fragshader2.txt", fragShaderString);

	const GLchar * pVertShaderSource = vertShaderString.c_str();
	const GLchar * pFragShaderSource = fragShaderString.c_str();

	glShaderSource(vsID, 1, &pVertShaderSource, NULL);
	glShaderSource(fsID, 1, &pFragShaderSource, NULL);

  GLint compile = 5;

	glCompileShader(vsID);
  glGetShaderiv(vsID, GL_COMPILE_STATUS, &compile);
  if (compile == GL_FALSE)
  {
  // it did not.
  printf("Error compiling\n");
  int iLogLength = 0;
  int iMaxLength = 0;
  glGetShaderiv(vsID, GL_INFO_LOG_LENGTH, &iMaxLength);
  char* buffer = new char[iMaxLength];
  glGetShaderInfoLog(vsID, iMaxLength, &iLogLength, buffer);
  if(iLogLength > 0) {
    printf("%s\n", buffer);
  }
  delete[] buffer; 
  }

	glCompileShader(fsID);
  glGetShaderiv(fsID, GL_COMPILE_STATUS, &compile);
  if (compile == GL_FALSE)
  {
    // it did not.
    printf("Error compiling\n");
    int iLogLength = 0;
    int iMaxLength = 0;
    glGetShaderiv(fsID, GL_INFO_LOG_LENGTH, &iMaxLength);
    char* buffer = new char[iMaxLength];
    glGetShaderInfoLog(fsID, iMaxLength, &iLogLength, buffer);
    if(iLogLength > 0) {
      printf("%s\n", buffer);
    }
    delete[] buffer; 
  }

	pID = glCreateProgram();
  if(pID == 0) printf("There was an error creating the program\n");
	glAttachShader(pID, vsID);
	glAttachShader(pID, fsID);

	glLinkProgram(pID);
  GLint debug = 5;
  glGetProgramiv(pID, GL_LINK_STATUS, &debug);
  if (debug == GL_FALSE)
  {
      // it did not.
      printf("Error linking\n");
      int iLogLength = 0;
      int iMaxLength = 0;
      glGetProgramiv(pID, GL_INFO_LOG_LENGTH, &iMaxLength);
      char* buffer = new char[iMaxLength];
      glGetProgramInfoLog(pID, iMaxLength, &iLogLength, buffer);
      if(iLogLength > 0) {
        printf("%s\n", buffer);
      }
      delete[] buffer; 
  }
	glUseProgram(pID);

	// get uniform input 
	transformLocation = glGetUniformLocation(pID, "MVP");
  if (transformLocation < 0) printf("Error no MVP found\n");

	viewLocation = glGetUniformLocation(pID, "N");
  if (viewLocation < 0) printf("Error no N found\n");

	matDiffuseLocation = glGetUniformLocation(pID, "Kd");
  if (matDiffuseLocation < 0) printf("Error no Kd found\n");

	matSpecLocation = glGetUniformLocation(pID, "Ks");
  if (matSpecLocation < 0) printf("Error no Ks found\n");

	matAmbientLocation = glGetUniformLocation(pID, "Ka");
  if (matAmbientLocation < 0) printf("Error no Ka found\n");

	shininessLocation = glGetUniformLocation(pID, "shininess");
  if (shininessLocation < 0) printf("Error no Ka found\n");

  lightDiffuseLocation = glGetUniformLocation(pID, "Ld");
  if (lightDiffuseLocation < 0) printf("Error no Ld found\n");

  lightSpecLocation = glGetUniformLocation(pID, "Ls");
  if (lightSpecLocation < 0) printf("Error no Ls found\n");

  lightAmbientLocation = glGetUniformLocation(pID, "La");
  if (lightSpecLocation < 0) printf("Error no La found\n");

  lightDirLocation = glGetUniformLocation(pID, "l_dir");
  if (lightDirLocation < 0) printf("Error no l_dir found\n");

  globalAmbientLocation = glGetUniformLocation(pID, "Ga");
  if(globalAmbientLocation < 0) printf("error no ambient found\n");
  glDeleteShader(vsID);
  glDeleteShader(fsID);
}

void toggleLightSource(int rgb)
{
  switch(state.primative) {
  case state.DIFFUSE:
    if(rgb == 3){
      LightProperties.Ld.r += 0.1;
      glLightfv(GL_LIGHT0, GL_DIFFUSE, glm::value_ptr(LightProperties.Ld));
    }
    else if(rgb == 4) {
      LightProperties.Ld.g += 0.1;
      glLightfv(GL_LIGHT0, GL_DIFFUSE, glm::value_ptr(LightProperties.Ld));
    }
    else if(rgb == 5) {
      LightProperties.Ld.b += 0.1;
      glLightfv(GL_LIGHT0, GL_DIFFUSE, glm::value_ptr(LightProperties.Ld));
    }
    else if(rgb == 6) {
      LightProperties.Ld.r -= 0.1;
      glLightfv(GL_LIGHT0, GL_DIFFUSE, glm::value_ptr(LightProperties.Ld));
    }
    else if(rgb == 7) {
      LightProperties.Ld.g -= 0.1;
      glLightfv(GL_LIGHT0, GL_DIFFUSE, glm::value_ptr(LightProperties.Ld));
    }
    else if(rgb == 8) {
      LightProperties.Ld.b -= 0.1;
      glLightfv(GL_LIGHT0, GL_DIFFUSE, glm::value_ptr(LightProperties.Ld));
    }
    break;
  case state.AMBIENT:
    if(rgb == 3){
      LightProperties.La.r += 0.1;
      glLightfv(GL_LIGHT0, GL_AMBIENT, glm::value_ptr(LightProperties.La));
    }
    else if(rgb == 4) {
      LightProperties.La.g += 0.1;
      glLightfv(GL_LIGHT0, GL_AMBIENT, glm::value_ptr(LightProperties.La));
    }
    else if(rgb == 5) {
      LightProperties.La.b += 0.1;
      glLightfv(GL_LIGHT0, GL_AMBIENT, glm::value_ptr(LightProperties.La));
    }
    else if(rgb == 6) {
      LightProperties.La.r -= 0.1;
      glLightfv(GL_LIGHT0, GL_AMBIENT, glm::value_ptr(LightProperties.La));
    }
    else if(rgb == 7) {
      LightProperties.La.g -= 0.1;
      glLightfv(GL_LIGHT0, GL_AMBIENT, glm::value_ptr(LightProperties.La));
    }
    else if(rgb == 8) {
      LightProperties.La.b -= 0.1;
      glLightfv(GL_LIGHT0, GL_AMBIENT, glm::value_ptr(LightProperties.La));
    }
    break;
  case state.SPECULAR:
    if(rgb == 3){
      LightProperties.Ls.r += 0.1;
      glLightfv(GL_LIGHT0, GL_SPECULAR, glm::value_ptr(LightProperties.Ls));
    }
    else if(rgb == 4) {
      LightProperties.Ls.g += 0.1;
      glLightfv(GL_LIGHT0, GL_SPECULAR, glm::value_ptr(LightProperties.Ls));
    }
    else if(rgb == 5) {
      LightProperties.Ls.b += 0.1;
      glLightfv(GL_LIGHT0, GL_SPECULAR, glm::value_ptr(LightProperties.Ls));
    }
    else if(rgb == 6) {
      LightProperties.Ls.r -= 0.1;
      glLightfv(GL_LIGHT0, GL_SPECULAR, glm::value_ptr(LightProperties.Ls));
    }
    else if(rgb == 7) {
      LightProperties.Ls.g -= 0.1;
      glLightfv(GL_LIGHT0, GL_SPECULAR, glm::value_ptr(LightProperties.Ls));
    }
    else if(rgb == 8) {
      LightProperties.Ls.b -= 0.1;
      glLightfv(GL_LIGHT0, GL_SPECULAR, glm::value_ptr(LightProperties.Ls));
    }
    break;
  }
}

void display1(void)
{
	glUseProgram(pID);

	glValidateProgram(pID);
	GLint validate = 0;
	glGetProgramiv(pID, GL_VALIDATE_STATUS, &validate);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glShadeModel(shade_model);
	glBindVertexArray(VAO);
  glEnable(GL_DEPTH_TEST);
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE,
                     glm::value_ptr(camera.getMVP()));
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE,
                     glm::value_ptr(camera.getModelViewMatrix()));
  glUniform4f(matDiffuseLocation, MaterialProperties.Kd.r,
              MaterialProperties.Kd.g, MaterialProperties.Kd.b, 1.0);
  glUniform4f(matSpecLocation, MaterialProperties.Ks.r,
              MaterialProperties.Ks.g, MaterialProperties.Ks.b, 1.0);
  glUniform4f(matAmbientLocation, MaterialProperties.Ka.r,
              MaterialProperties.Ka.g, MaterialProperties.Ka.b, 1.0);
  glUniform4f(lightDiffuseLocation, LightProperties.Ld.r,
              LightProperties.Ld.g, LightProperties.Ld.b, 1.0);
  glUniform4f(lightSpecLocation, LightProperties.Ls.r,
              LightProperties.Ls.g, LightProperties.Ls.b, 1.0);
  glUniform4f(lightAmbientLocation, LightProperties.La.r,
              LightProperties.La.g, LightProperties.La.b, 1.0);
  glUniform4f(lightDirLocation, LightProperties.light_position.x,
              LightProperties.light_position.y,
              LightProperties.light_position.z, 0.0);
  glUniform4f(globalAmbientLocation, LightProperties.global_ambient.r,
              LightProperties.global_ambient.g,
              LightProperties.global_ambient.b, 1.0);
  glUniform1f(shininessLocation, MaterialProperties.shininess);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glBindVertexArray(0);

  glutPostRedisplay();
	glutSwapBuffers();
}


void renderModel()
{
  for (int i = 0; i < (int) vertices.size() ; i+=3)
    {
      glm::vec3 point1 = vertices[i];
      glm::vec3 point2 = vertices[i+1];
      glm::vec3 point3 = vertices[i+2];

      glm::vec3 normal1 = normals[i];
      glm::vec3 normal2 = normals[i+1];
      glm::vec3 normal3 = normals[i+2];

      glBegin(GL_TRIANGLES);
      glNormal3fv(glm::value_ptr(normal1));
      glVertex3fv(glm::value_ptr(point1));
      glNormal3fv(glm::value_ptr(normal2));
      glVertex3fv(glm::value_ptr(point2));
      glNormal3fv(glm::value_ptr(normal3));
      glVertex3fv(glm::value_ptr(point3));
      glEnd(); 
    }
}

void display2()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glShadeModel(shade_model);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
    glMultMatrixf(glm::value_ptr(camera.getPerspectiveMatrix()));
  glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
  glMultMatrixf(glm::value_ptr(camera.getModelViewMatrix()));
  //glColor3f(color.r, color.g, color.b);
  glEnable(GL_DEPTH_TEST);
  renderModel();
  glutPostRedisplay();
	glFlush();
	glutSwapBuffers();
}

void bindNewBufferObj()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
		
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3),
               &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, VNO);
  glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3),
              &normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); 
}

void adjustPerspective(const float &near, const float &far)
{
  camera.perspective(glm::radians(45.0), 1.0, near, far);
}

void toggleLights(int flag)
{
  if(flag == 0){
    LightProperties.La = glm::vec4(0.0);
    LightProperties.Ls = glm::vec4(0.0);
    LightProperties.Ld = glm::vec4(0.0);
  }
  else {
    LightProperties.Ld = glm::vec4(1.0, 1.0, 1.0, 1.0);
    LightProperties.La = glm::vec4(0.2, 0.2, 0.2, 1.0);
    LightProperties.Ls = glm::vec4(1.0, 1.0, 1.0, 1.0);
  }
}

void adjustLight()
{
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT,
                 glm::value_ptr(LightProperties.global_ambient));
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == 'q' || key == 'Q'){
    exit(0);
  }
	if (key == ' '){ camera.reset(); } 
  if (key == 's'){ camera.rotateAboutN(glm::radians(5.0));}
  if (key == 'w'){ camera.rotateAboutN(glm::radians(-5.0));}
  if (key == 'r'){ camera.rotateAboutV(glm::radians(5.0));}
  if (key == 'e'){ camera.rotateAboutV(glm::radians(-5.0));}
  if (key == 'x'){ camera.translate(glm::vec3(0.0, 0.0, 1.0));}
  if (key == 'z'){ camera.translate(glm::vec3(0.0, 0.0, -1.0));}
  if (key == '9'){ glEnable(GL_LIGHT0); toggleLights(1);}
  if (key == '0'){ glDisable(GL_LIGHT0); toggleLights(0);}
        if (key == 'a'){ camera.rotateAboutU(glm::radians(5.0));}
        if (key == 'd'){ camera.rotateAboutU(glm::radians(-5.0));}
        if (key == 't'){ LightProperties.global_ambient.r += 0.1; adjustLight();}
        if (key == 'g'){ LightProperties.global_ambient.g += 0.1; adjustLight();}
        if (key == 'b'){ LightProperties.global_ambient.b += 0.1; adjustLight();}
        if (key == 'y'){ LightProperties.global_ambient.r -= 0.1; adjustLight();}
        if (key == 'h'){ LightProperties.global_ambient.g -= 0.1; adjustLight();}
        if (key == 'n'){ LightProperties.global_ambient.b -= 0.1; adjustLight();}
        if (key == 'j'){ shade_model = GL_FLAT; normals = flat_normals; bindNewBufferObj();}
        if (key == 'k'){ shade_model = GL_SMOOTH; normals = gourad_normals; bindNewBufferObj(); }
        if (key == 'A'){ state.primative = state.AMBIENT;}
        if (key == 'S'){ state.primative = state.SPECULAR;}
        if (key == 'D'){ state.primative = state.DIFFUSE;}
        if (key == '3'){ toggleLightSource(3);}
        if (key == '4'){ toggleLightSource(4);}
        if (key == '5'){ toggleLightSource(5);}
        if (key == '6'){ toggleLightSource(6);}
        if (key == '7'){ toggleLightSource(7);}
        if (key == '8'){ toggleLightSource(8);}
}

void initBufferObject(void)
{

	glGenBuffers(1, &VBO);
  glGenBuffers(1, &VNO);

	// setup VAO
  bindNewBufferObj();
	
	// Use depth buffering for hidden surface elimination
	glEnable(GL_DEPTH_TEST);

	// initialize MVP 
  camera.perspective(glm::radians(45.0), 1.0, 0.1, 100.0);

  camera.lookAt(glm::vec3(0.0, 0.0, -5.0),
                glm::vec3(0.0, 0.0, 0.0),
                glm::vec3(0.0, 0.1, 0.0));
}

void initLights(void)
{
  MaterialProperties.Ks = glm::vec4(1.0, 1.0, 1.0, 1.0);
  MaterialProperties.Ka = glm::vec4(1.0, 1.0, 1.0, 1.0);
  MaterialProperties.Kd = glm::vec4(1.0, 1.0, 1.0, 1.0);
  MaterialProperties.shininess = 5.0;

  GLfloat shininess[] = { 5.0 };
  LightProperties.Ld = glm::vec4(1.0, 1.0, 1.0, 1.0);
  LightProperties.La = glm::vec4(0.1, 0.1, 0.1, 1.0);
  LightProperties.Ls = glm::vec4(1.0, 1.0, 1.0, 1.0);
  LightProperties.light_position = glm::vec4(1.0, 1.0, 1.0, 1.0);

  LightProperties.global_ambient = glm::vec4(0.0, 0.0, 0.0, 0.0);

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(shade_model);

  glMaterialfv(GL_FRONT, GL_AMBIENT, glm::value_ptr(MaterialProperties.Ka));
  glMaterialfv(GL_FRONT, GL_DIFFUSE, glm::value_ptr(MaterialProperties.Kd));
  glMaterialfv(GL_FRONT, GL_SPECULAR, glm::value_ptr(MaterialProperties.Ks));
  glMaterialfv(GL_FRONT, GL_SHININESS, shininess); 

  glLightModelfv(GL_LIGHT_MODEL_AMBIENT,
                 glm::value_ptr(LightProperties.global_ambient));
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

  glLightfv(GL_LIGHT0, GL_POSITION,
            glm::value_ptr(LightProperties.light_position));
  glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.3);
  glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, glm::value_ptr(LightProperties.La));
  glLightfv(GL_LIGHT0, GL_DIFFUSE, glm::value_ptr(LightProperties.Ld));
  glLightfv(GL_LIGHT0, GL_SPECULAR, glm::value_ptr(LightProperties.Ls));

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);

  color = glm::vec3(1.0, 0.0, 0.0);
  shade_model = GL_SMOOTH;
  mNear = 0.1;
  mFar = 100.0;

	glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Programmable Pipeline");
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display1);
  printf("shader version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
  printf("opengl version: %s\n", glGetString(GL_VERSION));
  loadOBJ("models/bunny.obj", vertices, uvs, gourad_normals, flat_normals);
  normals = gourad_normals;
	initBufferObject();
	setShaders();

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Fixed Pipeline");
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display2);

  initLights();

	glutMainLoop();
	return 0;           
}
