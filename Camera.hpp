#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera
{
private:

    glm::vec3 u;
    glm::vec3 v;
    glm::vec3 n;
    glm::vec3 pos;
    glm::vec3 mDirection;
    glm::mat4 mModelViewMat;
    glm::mat4 mPerspectiveMat;

public:

    Camera();

    void rotateAboutU(float angle);

    void rotateAboutV(float angle);

    void rotateAboutN(float angle);

    void translate(const glm::vec3 &direction);

    void lookAt(const glm::vec3 &eye, const glm::vec3 &target, const glm::vec3 &up);

    void perspective(const float &fov, const float &aspect,
                   const float &near, const float &far);

    glm::mat4 getModelViewMatrix();

    glm::mat4 getPerspectiveMatrix();

    glm::mat4 getMVP();
    
    void reset();
};
