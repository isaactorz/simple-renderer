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

  initLights();

	glutMainLoop();
	return 0;           
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
