/*****************************************************************************
 * Simple, Fast Renderer (SFR)                                               *
 * CS249b                                                                    *
 * Matt Fichman                                                              *
 * February, 2011                                                            *
 *****************************************************************************/
#pragma once

#include "sfr/Common.hpp"
#include "sfr/Node.hpp"
#include "sfr/Program.hpp"

namespace sfr {

/* Geometric mesh node (with attached textures) */
class Model : public Node {
public:
    Model();
    Ptr<Material> material() const;
    Ptr<Mesh> mesh() const;
    Ptr<ModelProgram> program() const;
    Ptr<Model> clone() const;
    Ptr<Node> nodeClone() const;
    void* userData() const;

    void materialIs(Ptr<Material> material);
    void meshIs(Ptr<Mesh> mesh);
    void programIs(Ptr<ModelProgram> program);
    void userDataIs(void* data);

    virtual void operator()(Ptr<Functor> functor);

private:
    Ptr<Material> material_;
    Ptr<Mesh> mesh_;
    Ptr<ModelProgram> program_;
    void* userData_;
};

class ModelProgram : public Program {
public:
    ModelProgram(std::string const& name) : Program(name) {}

    GLint diffuseMap() const { return diffuseMap_; }
    GLint specularMap() const { return specularMap_; }
    GLint normalMap() const { return normalMap_; }
    GLint emissiveMap() const { return emissiveMap_; }
    GLint ambient() const { return ambient_; }
    GLint diffuse() const { return diffuse_; }
    GLint specular() const { return specular_; }
    GLint emissive() const { return emissive_; }
    GLint shininess() const { return shininess_; }
    GLint normalMatrix() const { return normalMatrix_; }
    GLint transform() const { return transform_; }
    GLint opacity() const { return opacity_; }

    virtual void onLink();
    virtual void onRender(Ptr<Model> model) {}

private:
    GLint diffuseMap_ = -1;
    GLint specularMap_ = -1;
    GLint normalMap_ = -1;
    GLint emissiveMap_ = -1;
    GLint ambient_ = -1;
    GLint diffuse_ = -1;
    GLint specular_ = -1;
    GLint emissive_ = -1;
    GLint shininess_ = -1;
    GLint opacity_ = -1;
    GLint normalMatrix_ = -1;
    GLint transform_ = -1;
};

}
