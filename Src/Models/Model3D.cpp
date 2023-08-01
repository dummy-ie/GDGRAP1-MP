#include "Model3D.hpp"

using namespace model;
using namespace glm;

// insert constructor variables into attributes
Model3D::Model3D(std::string modelPath, std::string texturePath, std::string normalPath, vec3 color, vec3 pos, vec3 rot, vec3 sca) : color(color), position(pos), rotation(rot), scale(sca)
{
    // std::string path = "Models/djSword.obj";
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> material;
    std::string warning, error;
    tinyobj::attrib_t attributes;
    bool success;

    if (!modelPath.empty())
    {
        success = tinyobj::LoadObj(
            &attributes,
            &shapes,
            &material,
            &warning,
            &error,
            modelPath.c_str());

        if (!warning.empty())
            std::cout << warning << std::endl;

        if (!error.empty())
            std::cout << error << std::endl;

        std::cout << "success: " << success << std::endl;
    }
    else
        std::cout << "Error empty model path!" << std::endl;

    if (success)
    {
        // std::cout << "attributes size: " << attributes. << std::endl;
        attributesSize = 3;

        if (!attributes.normals.empty())
            attributesSize += 3;

        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> bitangents;
        if (!attributes.texcoords.empty())
        {
            attributesSize += 8;

            for (size_t i = 0; i < shapes[0].mesh.indices.size(); i += 3)
            {
                tinyobj::index_t vData1 = shapes[0].mesh.indices[i];
                tinyobj::index_t vData2 = shapes[0].mesh.indices[i + 1];
                tinyobj::index_t vData3 = shapes[0].mesh.indices[i + 2];

                glm::vec3 v1 = glm::vec3(
                    attributes.vertices[vData1.vertex_index * 3],
                    attributes.vertices[vData1.vertex_index * 3 + 1],
                    attributes.vertices[vData1.vertex_index * 3 + 2]);

                glm::vec3 v2 = glm::vec3(
                    attributes.vertices[vData2.vertex_index * 3],
                    attributes.vertices[vData2.vertex_index * 3 + 1],
                    attributes.vertices[vData2.vertex_index * 3 + 2]);

                glm::vec3 v3 = glm::vec3(
                    attributes.vertices[vData3.vertex_index * 3],
                    attributes.vertices[vData3.vertex_index * 3 + 1],
                    attributes.vertices[vData3.vertex_index * 3 + 2]);

                glm::vec2 uv1 = glm::vec2(
                    attributes.texcoords[vData1.texcoord_index * 2],
                    attributes.texcoords[vData1.texcoord_index * 2 + 1]);

                glm::vec2 uv2 = glm::vec2(
                    attributes.texcoords[vData2.texcoord_index * 2],
                    attributes.texcoords[vData2.texcoord_index * 2 + 1]);

                glm::vec2 uv3 = glm::vec2(
                    attributes.texcoords[vData3.texcoord_index * 2],
                    attributes.texcoords[vData3.texcoord_index * 2 + 1]);

                glm::vec3 deltaPos1 = v2 - v1;
                glm::vec3 deltaPos2 = v3 - v1;

                glm::vec2 deltaUV1 = uv2 - uv1;
                glm::vec2 deltaUV2 = uv3 - uv1;

                float r = 1.f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
                glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
                glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

                tangents.push_back(tangent);
                tangents.push_back(tangent);
                tangents.push_back(tangent);

                bitangents.push_back(bitangent);
                bitangents.push_back(bitangent);
                bitangents.push_back(bitangent);
            }
        }   

        for (size_t i = 0; i < shapes.at(0).mesh.indices.size(); i++)
        {
            // std::cout << "for loop" << i << std::endl;
            tinyobj::index_t vData = shapes.at(0).mesh.indices.at(i);
            vertexData.push_back(attributes.vertices.at((vData.vertex_index * 3)));
            vertexData.push_back(attributes.vertices.at((vData.vertex_index * 3) + 1));
            vertexData.push_back(attributes.vertices.at((vData.vertex_index * 3) + 2));

            if (!attributes.normals.empty())
            {
                vertexData.push_back(attributes.normals.at((vData.normal_index * 3)));
                vertexData.push_back(attributes.normals.at((vData.normal_index * 3) + 1));
                vertexData.push_back(attributes.normals.at((vData.normal_index * 3) + 2));
            }

            if (!attributes.texcoords.empty())
            {
                vertexData.push_back(attributes.texcoords.at((vData.texcoord_index * 2)));
                vertexData.push_back(attributes.texcoords.at((vData.texcoord_index * 2) + 1));
            }

            vertexData.push_back(tangents.at(i).x);
            vertexData.push_back(tangents[i].y);
            vertexData.push_back(tangents[i].z);
            vertexData.push_back(bitangents[i].x);
            vertexData.push_back(bitangents[i].y);
            vertexData.push_back(bitangents[i].z);
        }
        std::cout << "end for loop" << std::endl;

        // initialize VAO and VBO
        glGenVertexArrays(1, &VAO);
        std::cout << "end gen array" << std::endl;
        glGenBuffers(1, &VBO);
        std::cout << "end gen buffers" << std::endl;
        // glGenBuffers(1, &VBO_UV);
        // glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        std::cout << "end bind array and buffer" << std::endl;

        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(GLfloat) * vertexData.size(),
            vertexData.data(),
            // attributes.vertices.data(),
            GL_DYNAMIC_DRAW);

        glVertexAttribPointer(
            0,
            3, // X Y Z
            GL_FLOAT,
            GL_FALSE,
            attributesSize * sizeof(GL_FLOAT),
            (void *)0);

        glEnableVertexAttribArray(0);
        std::cout << "end attrib pointer 0" << std::endl;

        if (!attributes.normals.empty())
        {
            GLintptr normalsPtr = 3 * sizeof(float);
            glVertexAttribPointer(
                1,
                3,
                GL_FLOAT,
                GL_FALSE,
                attributesSize * sizeof(GL_FLOAT),
                (void *)normalsPtr);
            glEnableVertexAttribArray(1);
            std::cout << "end normals attrib pointer" << std::endl;
        }

        if (!attributes.texcoords.empty())
        {
            GLintptr uvPtr = (attributesSize - 8) * sizeof(float);
            glVertexAttribPointer(
                2,
                2,
                GL_FLOAT,
                GL_FALSE,
                attributesSize * sizeof(GL_FLOAT),
                (void *)uvPtr);
            glEnableVertexAttribArray(2);
            std::cout << "end uv attrib pointer" << std::endl;

            GLintptr tangentPtr = (attributesSize - 6) * sizeof(float);
            glVertexAttribPointer(
                3,
                3,
                GL_FLOAT,
                GL_FALSE,
                attributesSize * sizeof(GL_FLOAT),
                (void *)tangentPtr);
            glEnableVertexAttribArray(3);

            GLintptr bitangentPtr = (attributesSize - 3) * sizeof(float);
            glVertexAttribPointer(
                4,
                3,
                GL_FLOAT,
                GL_FALSE,
                attributesSize * sizeof(GL_FLOAT),
                (void *)bitangentPtr);
            glEnableVertexAttribArray(4);
        }

        std::cout << "loaded model" << std::endl;
    }
    else
        std::cout << "Error loading object file!" << std::endl;

    if (!texturePath.empty())
    {
        // texture mapping
        int img_width, img_height, colorChannels;
        stbi_set_flip_vertically_on_load(true); // flip da image
        unsigned char *tex_bytes = stbi_load(texturePath.c_str(), &img_width, &img_height, &colorChannels, 0);

        // GLuint texture;
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        if (texturePath.find("png") != std::string::npos)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_bytes);
        else if (texturePath.find("jpg") != std::string::npos)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_bytes);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // generate MIPMAPS! (arent they the little image duplicates for when it gets far away??)
        glGenerateMipmap(GL_TEXTURE_2D);

        // Free loaded bytes
        stbi_image_free(tex_bytes);
        glEnable(GL_DEPTH_TEST);

        std::cout << "loaded texture" << std::endl;
    }

    if (!normalPath.empty())
    {
        // texture mapping
        int img_width, img_height, colorChannels;
        stbi_set_flip_vertically_on_load(true); // flip da image
        unsigned char *normTextureBytes = stbi_load(normalPath.c_str(), &img_width, &img_height, &colorChannels, 0);

        // GLuint texture;
        glGenTextures(1, &normalTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTexture);

        if (normalPath.find("png") != std::string::npos)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, normTextureBytes);
        else if (normalPath.find("jpg") != std::string::npos)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, normTextureBytes);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // generate MIPMAPS! (arent they the little image duplicates for when it gets far away??)
        glGenerateMipmap(GL_TEXTURE_2D);

        // Free loaded bytes
        stbi_image_free(normTextureBytes);
        glEnable(GL_DEPTH_TEST);

        std::cout << "loaded texture" << std::endl;
    }
}

Model3D::~Model3D()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Model3D::draw(GLuint &shaderProgram)
{
    glUseProgram(shaderProgram);

    // generate a transformation matrix based on the stored attributes for this model3d
    glm::mat4 identity_matrix4 = glm::mat4(1.f);
    glm::mat4 transformation_matrix = glm::translate(identity_matrix4, position);
    transformation_matrix = glm::scale(transformation_matrix, scale);
    transformation_matrix = glm::rotate(transformation_matrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    transformation_matrix = glm::rotate(transformation_matrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    transformation_matrix = glm::rotate(transformation_matrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));

    // send to the given shaderprogram
    unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));

    GLuint tex0Address = glGetUniformLocation(shaderProgram, "tex0");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(tex0Address, 0);

    if (glIsTexture(normalTexture))
    {
        GLuint norm_texAddress = glGetUniformLocation(shaderProgram, "norm_tex");
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTexture);
        glUniform1i(norm_texAddress, 1);
    }

    GLuint rgbaAddress = glGetUniformLocation(shaderProgram, "rgba");
    glUniform4fv(rgbaAddress, 1, value_ptr(vec4(color, 1.f)));

    // draw
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexData.size() / attributesSize);
}