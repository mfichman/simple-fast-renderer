/*****************************************************************************
 * Simple, Fast Renderer (SFR)                                               *
 * CS249b                                                                    *
 * Matt Fichman                                                              *
 * February, 2011                                                            *
 *****************************************************************************/

#include "sfr/Common.hpp"
#include "sfr/AssetTable.hpp"
#include "sfr/AttributeBuffer.hpp"
#include "sfr/Camera.hpp"
#include "sfr/IndexBuffer.hpp"
#include "sfr/Material.hpp"
#include "sfr/Model.hpp"
#include "sfr/Mesh.hpp"
#include "sfr/Texture.hpp"
#include "sfr/TransparencyRenderer.hpp"
#include "sfr/Scene.hpp"

using namespace sfr;

TransparencyRenderer::TransparencyRenderer(Ptr<AssetTable> manager) {
    program_ = manager->assetIs<ModelProgram>("shaders/Transparency");
    program_->statusIs(Program::LINKED);
}

void TransparencyRenderer::onState() {
    if (state() == Renderer::ACTIVE) {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
    } else if (state() == Renderer::INACTIVE) {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ZERO);
        glDepthMask(GL_TRUE);
    } else {
        assert(!"Invalid state");
    }
}

void TransparencyRenderer::operator()(Ptr<Model> model) {
    // Skip non-transparent objects and objects w/o materials
    if (!model->material() || model->material()->opacity() >= 1.f) {
        return;
    }
    if (model->program()) {
        activeProgram_ = model->program();
        activeProgram_->statusIs(Program::LINKED);
    } else {
        activeProgram_ = program_;
    }
    glUseProgram(activeProgram_->id());

    // Allow user program to set custom uniforms
    activeProgram_->onRender(model);

    // Set the material parameters and render mesh
    operator()(model->material());
    operator()(model->mesh());
}

void TransparencyRenderer::operator()(Ptr<Mesh> mesh) {
    if (!mesh || !mesh->indexBuffer() || !scene() || !scene()->camera()) {
        return;
    }

    mesh->statusIs(Mesh::SYNCED);

    Ptr<Camera> camera = scene()->camera();
    Matrix normalMatrix = (camera->viewMatrix() * worldMatrix()).inverse().transpose();
    GLfloat temp[9] = {
        (GLfloat)normalMatrix[0], (GLfloat)normalMatrix[1], (GLfloat)normalMatrix[2],
        (GLfloat)normalMatrix[4], (GLfloat)normalMatrix[5], (GLfloat)normalMatrix[6],
        (GLfloat)normalMatrix[8], (GLfloat)normalMatrix[9], (GLfloat)normalMatrix[10]
    };

    // Pass the matrices to the vertex shader
    Matrix const worldViewProjectionMatrix = camera->viewProjectionMatrix() * worldMatrix();
    glUniformMatrix3fv(activeProgram_->normalMatrix(), 1, 0, temp);    
    glUniformMatrix4fv(activeProgram_->transform(), 1, 0, worldViewProjectionMatrix.mat4f());

    // Render the mesh
    Ptr<IndexBuffer> buffer = mesh->indexBuffer();
    glBindVertexArray(mesh->id());
    glDrawElements(GL_TRIANGLES, buffer->elementCount(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void TransparencyRenderer::operator()(Ptr<Material> material) {
    glUniform3fv(activeProgram_->diffuse(), 1, material->diffuseColor().vec4f());
    glUniform1f(activeProgram_->opacity(), material->opacity());

    switch (material->blendMode()) {
    case Material::ALPHA: glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
    case Material::ADDITIVE: glBlendFunc(GL_SRC_ALPHA, GL_ONE); break;
    default: assert(!"error: unknown blend mode");
    }

    glActiveTexture(GL_TEXTURE0);
    operator()(material->diffuseMap());
}

void TransparencyRenderer::operator()(Ptr<Texture> texture) {
    if (texture) {
        glBindTexture(GL_TEXTURE_2D, texture->id());
    }
} 

