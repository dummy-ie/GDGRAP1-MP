#version 330 core

out vec4 FragColor;

in vec3 texCoord;

uniform samplerCube skybox;

void main(){

    FragColor = texture(skybox, texCoord);
    // FragColor *= vec4(0.f, 1.f, 0.f, 1.f); // simple nightvision
}