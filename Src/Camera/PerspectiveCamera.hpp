#ifndef PERSPECTIVE_CAMERA_HPP
#define PERSPECTIVE_CAMERA_HPP

#include "Camera.hpp"

namespace gd
{
    using namespace glm;
    class PerspectiveCamera : public Camera
    {
    private:
        float fov;
        float height;
        float width;

    public:
        PerspectiveCamera(float fov, float height, float width, vec3 pos = vec3(0.f, 0.f, -10.f), vec3 rot = vec3(0.f), vec3 dir = vec3(0.f));
        mat4 generateProjectionMatrix();
    };

    PerspectiveCamera::PerspectiveCamera(float fov, float height, float width, vec3 pos, vec3 rot, vec3 dir) : fov(fov), height(height), width(width), Camera(pos, rot, dir) {}

    mat4 PerspectiveCamera::generateProjectionMatrix()
    {
        return perspective(radians(fov), height / width, 0.1f, 300.f);
    }
    
} // namespace gd

#endif // !PERSPECTIVE_CAMERA_HPP