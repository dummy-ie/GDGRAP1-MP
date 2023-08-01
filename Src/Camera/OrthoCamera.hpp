#ifndef ORTHOGRAPHIC_CAMERA_HPP
#define ORTHOGRAPHIC_CAMERA_HPP

#include "Camera.hpp"

namespace gd
{
    using namespace glm;
    class OrthoCamera : public Camera
    {
    public:
        OrthoCamera(vec3 pos = vec3(0.f, 0.f, -10.f), vec3 rot = vec3(0.f), vec3 dir = vec3(0.f));
        mat4 generateProjectionMatrix();
    };
    
    OrthoCamera::OrthoCamera(vec3 pos, vec3 rot, vec3 dir) : Camera(pos, rot, dir) {}

    mat4 OrthoCamera::generateProjectionMatrix()
    {
        return ortho(-(600.f / 4.f), 600.f / 4.f, 600.f / 4.f, -(600.f / 4.f), -200.f, 200.f); // fixed values for now
    }
} // namespace gd


#endif // !ORTHOGRAPHIC_CAMERA_HPP