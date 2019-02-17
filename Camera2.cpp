#include "Camera2.hpp"
#include <stdio.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

Camera2::Camera2() :
       
        u(0.0f, 1.0f, 0.0f),
        v(0.0f, 0.0f, 1.0f),
        n(1.0f, 0.0f, 0.0f),
        pos(0.0f, 0.0f, 0.0f),
        mDirection(0.0f, 0.0f, 0.0f),
        mModelViewMat(1.0),
        mPerspectiveMat(1.0)
    {
    }

    void Camera2::rotateAboutU(float angle)
    {
           v = glm::rotate(v, angle, u);
           n = glm::rotate(n, angle, u);
    }

    void Camera2::rotateAboutV(float angle)
    {
        u = glm::rotate(u, angle, v);
        n = glm::rotate(n, angle, v);
    }

    void Camera2::rotateAboutN(float angle)
    {
       u = glm::rotate(u, angle, n);
       v = glm::rotate(v, angle, n);
    }

    void Camera2::translate(const glm::vec3 &direction)
    {
      mDirection += direction;
      pos = glm::vec3(-glm::dot(n, mDirection), -glm::dot(u, mDirection)
                      , -glm::dot(v, mDirection));
    }

    glm::mat4 Camera2::getModelViewMatrix()
    {
      mModelViewMat[0] = glm::vec4(n.x, u.x, v.x, 0.0);
      mModelViewMat[1] = glm::vec4(n.y, u.y, v.y, 0.0);
      mModelViewMat[2] = glm::vec4(n.z, u.z, v.z, 0.0);
      mModelViewMat[3] = glm::vec4(pos, 1.0);
      return glm::inverse(mModelViewMat);
    }

    void Camera2::lookAt(const glm::vec3 &eye, const glm::vec3 &target
                        , const glm::vec3 &up)
    {
        v = glm::normalize(eye - target);
        n = glm::normalize(glm::cross(up, v));
        u = glm::cross(v, n);
        mDirection = eye;
        pos = glm::vec3(-glm::dot(n, eye), -glm::dot(u, eye), -glm::dot(v, eye));
    }

    void Camera2::perspective(const float &fov, const float &aspect,
                         const float &near, const float &far)
    {
      float top = near*tan(fov/2);
      float bottom = -top;
      float right = top*aspect;
      float left = -right;

      mPerspectiveMat[0] = glm::vec4(2*near/(right - left), 0, 0, 0);
      mPerspectiveMat[1] = glm::vec4(0, 2*near/(top-bottom), 0, 0);
      mPerspectiveMat[2] = glm::vec4(0, 0, -(far+near)/(far-near),-1);
      mPerspectiveMat[3] = glm::vec4(0, 0,-(2*far*near)/(far-near), 0);
      }
 
    glm::mat4 Camera2::getPerspectiveMatrix()
    {
      return mPerspectiveMat;
    }

    glm::mat4 Camera2::getMVP()
    {
      return mPerspectiveMat * getModelViewMatrix();
    }

    void Camera2::reset()
    {
      mDirection = glm::vec3(0.0, 0.0, -3.0);
      glm::vec3 target = glm::vec3(0.0, 0.0, 0.0);
      glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
      v = glm::normalize(mDirection - target);
      n = glm::normalize(glm::cross(up, v));
      u = glm::cross(v, n);
      pos = glm::vec3(-glm::dot(n, mDirection), -glm::dot(u, mDirection)
                      , -glm::dot(v, mDirection));
      perspective(glm::radians(45.0), 1, 0.1, 100.0);
    }
