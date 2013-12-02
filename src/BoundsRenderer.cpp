/*****************************************************************************
 * Simple, Fast Renderer (SFR)                                               *
 * CS249b                                                                    *
 * Matt Fichman                                                              *
 * February, 2011                                                            *
 *****************************************************************************/

#include "sfr/Common.hpp"
#include "sfr/BoundsRenderer.hpp"
#include "sfr/Camera.hpp"
#include "sfr/Transform.hpp"
#include "sfr/Mesh.hpp"
#include "sfr/Model.hpp"
#include "sfr/AssetTable.hpp"
#include "sfr/AttributeBuffer.hpp"
#include "sfr/IndexBuffer.hpp"
#include "sfr/Effect.hpp"
#include "sfr/World.hpp"

using namespace sfr;

BoundsRenderer::BoundsRenderer(Ptr<AssetTable> manager) {
    shader_ = manager->assetIs<sfr::Effect>("shaders/Flat");
    shader_->statusIs(Effect::LINKED); 
    Ptr<Mesh> cube = manager->assetIs<Mesh>("meshes/LightShapes.obj/Cube");

    unitCube_ = manager->assetIs<Mesh>("meshes/UnitCube");

    Ptr<MutableAttributeBuffer<Vector>> vbuf(new MutableAttributeBuffer<Vector>("position"));
    vbuf->elementEnq(Vector(1.000000, 1.000000, -1.000000)); // 0
    vbuf->elementEnq(Vector(1.000000, -1.000000, -1.000000)); // 1
    vbuf->elementEnq(Vector(-1.000000, -1.000000, -1.000000)); // 2
    vbuf->elementEnq(Vector(-1.000000, 1.000000, -1.000000)); // 3
    vbuf->elementEnq(Vector(1.000000, 1.000000, 1.000000)); // 4
    vbuf->elementEnq(Vector(1.000000, -1.000001, 1.000000)); // 5
    vbuf->elementEnq(Vector(-1.000000, -1.000000, 1.000000)); // 6
    vbuf->elementEnq(Vector(-1.000000, 1.000000, 1.000000)); // 7

    Ptr<IndexBuffer> ibuf(new IndexBuffer("buf"));
    ibuf->elementEnq(0); ibuf->elementEnq(1);
    ibuf->elementEnq(0); ibuf->elementEnq(3);
    ibuf->elementEnq(0); ibuf->elementEnq(4);
    ibuf->elementEnq(1); ibuf->elementEnq(2);
    ibuf->elementEnq(1); ibuf->elementEnq(5);
    ibuf->elementEnq(2); ibuf->elementEnq(3);
    ibuf->elementEnq(2); ibuf->elementEnq(6);
    ibuf->elementEnq(3); ibuf->elementEnq(7);
    ibuf->elementEnq(4); ibuf->elementEnq(5);
    ibuf->elementEnq(4); ibuf->elementEnq(7);
    ibuf->elementEnq(5); ibuf->elementEnq(6);
    ibuf->elementEnq(6); ibuf->elementEnq(7);

    unitCube_->attributeBufferIs("position", vbuf);
    unitCube_->indexBufferIs(ibuf);
    unitCube_->statusIs(Mesh::SYNCED);


    glUseProgram(shader_->id());
    model_ = glGetUniformLocation(shader_->id(), "modelMatrix");
    view_ = glGetUniformLocation(shader_->id(), "viewMatrix");
    projection_ = glGetUniformLocation(shader_->id(), "projectionMatrix");
    glUseProgram(0);
}

void BoundsRenderer::operator()(Ptr<World> world) {
    glUseProgram(shader_->id());
    glEnable(GL_DEPTH_TEST);
    world_ = world;
    Renderer::operator()(world_->root());
    glDisable(GL_DEPTH_TEST);
    glUseProgram(0);
}

void BoundsRenderer::operator()(Ptr<Model> model) {
    operator()(model->mesh());
}

void BoundsRenderer::operator()(Ptr<Mesh> mesh) {
    if (!mesh || !mesh->indexBuffer() || !world_ || !world_->camera()) {
        return;
    }

    Box bounds = mesh->bounds(); 
    Vector d = (bounds.max - bounds.min) / 2.0;
    Vector pos = (bounds.max + bounds.min) / 2.0;

    Matrix translate = Matrix::translate(pos);
    Matrix scale = Matrix::scale(d.x, d.y, d.z);  

    Matrix transform = worldTransform() * translate * scale;

    unitCube_->statusIs(Mesh::SYNCED);

    // Pass the model matrix to the vertex shader
    Ptr<Camera> camera = world_->camera();
    glUniformMatrix4fv(model_, 1, 0, transform);
    glUniformMatrix4fv(projection_, 1, 0, camera->projectionTransform());
    glUniformMatrix4fv(view_, 1, 0, camera->viewTransform());

    // Render the mesh
    Ptr<IndexBuffer> buffer = unitCube_->indexBuffer();
    glBindVertexArray(unitCube_->id());
    glDrawElements(GL_LINES, buffer->elementCount(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
