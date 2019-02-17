#include "Camera.hpp"
#include <stdio.h>
#include <math.h>
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

Camera::Camera() :
       
        u(0.0f, 1.0f, 0.0f),
        v(0.0f, 0.0f, 1.0f),
        n(1.0f, 0.0f, 0.0f),
        pos(0.0f, 0.0f, 0.0f),
        mDirection(0.0f, 0.0f, 0.0f),
        mModelViewMat(1.0),
        mPerspectiveMat(1.0)
    {
    }

    void Camera::rotateAboutU(float angle)
    {
           v = glm::rotate(v, angle, u);
           n = glm::rotate(n, angle, u);
           mModelViewMat[0] = glm::vec4(u.x, v.x, n.x, 0.0);
           mModelViewMat[1] = glm::vec4(u.y, v.y, n.y, 0.0);
           mModelViewMat[2] = glm::vec4(u.z, v.z, n.z, 0.0);
    }

    void Camera::rotateAboutV(float angle)
    {
        u = glm::rotate(u, angle, v);
        n = glm::rotate(n, angle, v);
        mModelViewMat[0] = glm::vec4(u.x, v.x, n.x, 0.0);
        mModelViewMat[1] = glm::vec4(u.y, v.y, n.y, 0.0);
        mModelViewMat[2] = glm::vec4(u.z, v.z, n.z, 0.0);
    }

    void Camera::rotateAboutN(float angle)
    {
       u = glm::rotate(u, angle, n);
       v = glm::rotate(v, angle, n);
       mModelViewMat[0] = glm::vec4(u.x, v.x, n.x, 0.0);
       mModelViewMat[1] = glm::vec4(u.y, v.y, n.y, 0.0);
       mModelViewMat[2] = glm::vec4(u.z, v.z, n.z, 0.0);
    }

    void Camera::translate(const glm::vec3 &direction)
    {
      mDirection += direction;
      pos = glm::vec3(-glm::dot(n, mDirection), -glm::dot(u, mDirection),
                      -glm::dot(v, mDirection));
      mModelViewMat[3] = glm::vec4(pos, 1.0);
    }

    glm::mat4 Camera::getModelViewMatrix()
    {
      mModelViewMat[0] = glm::vec4(u.x, v.x, n.x, 0.0);
      mModelViewMat[1] = glm::vec4(u.y, v.y, n.y, 0.0);
      mModelViewMat[2] = glm::vec4(u.z, v.z, n.z, 0.0);
      mModelViewMat[3] = glm::vec4(pos, 1.0);
      return glm::inverse(mModelViewMat);
    }

    glm::mat4 Camera::getPerspectiveMatrix()
    {
      return mPerspectiveMat;
    }

    void Camera::lookAt(const glm::vec3 &eye, const glm::vec3 &target,
                        const glm::vec3 &up)
    {
        v = glm::normalize(eye - target);
        n = glm::normalize(glm::cross(up, v));
        u = glm::cross(v, n);
        mDirection = eye;
        pos = glm::vec3(-glm::dot(n, eye), -glm::dot(u, eye),
                        -glm::dot(v, eye));

        mModelViewMat[0] = glm::vec4(u.x, v.x, n.x, 0.0);
        mModelViewMat[1] = glm::vec4(u.y, v.y, n.y, 0.0);
        mModelViewMat[2] = glm::vec4(u.z, v.z, n.z, 0.0);
        mModelViewMat[3] = glm::vec4(pos, 1.0);
    }

    void Camera::perspective(const float &fov, const float &aspect,
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

    glm::mat4 Camera::getMVP()
    {
      return mPerspectiveMat*glm::inverse(mModelViewMat);
    }
 
    void Camera::reset()
    {
      mDirection = glm::vec3(0.0, 0.0, -3.0);
      glm::vec3 target = glm::vec3(0.0, 0.0, 0.0);
      glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
      v = glm::normalize(mDirection - target);
      n = glm::normalize(glm::cross(up, v));
      u = glm::cross(v, n);
      pos = glm::vec3(-glm::dot(n, mDirection), -glm::dot(u, mDirection)
                      , -glm::dot(v, mDirection));
    }
