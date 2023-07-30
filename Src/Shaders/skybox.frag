#version 330 core

out vec4 FragColor;

in vec3 texCoord;

uniform samplerCube skybox;
uniform bool useThirdPersonCamera;

void main(){

    FragColor = texture(skybox, texCoord);

    if (!useThirdPersonCamera) // cheap ww
        FragColor *= vec4(0.f, 1.f, 0.f, 1.f); // simple nightvision
}