/*****************************************************************************
 * Simple, Fast Renderer (SFR)                                               *
 * CS249b                                                                    *
 * Matt Fichman                                                              *
 * February, 2011                                                            *
 *****************************************************************************/


#include "sfr/Common.hpp"
#include "sfr/Particles.hpp"

using namespace sfr;

#define OFFSET(field) ((void*)&(((Particle*)0)->field))
#define SIZE(field) (sizeof((((Particle*)0)->field)))

Particles::Particles() {
    status_ = DIRTY;
    time_ = 0;
    buffer_.reset(new MutableAttributeBuffer<Particle>(""));
    glGenVertexArrays(1, &id_);
}

Particles::~Particles() {
    glDeleteVertexArrays(1, &id_);
}

void Particles::particleEnq(Particle const& particle) {
    status_ = DIRTY;
    buffer_->elementEnq(particle);
}

void Particles::particleIs(GLuint index, Particle const& particle) {
    status_ = DIRTY;
    buffer_->elementIs(index, particle);
}

Particle const& Particles::particle(GLuint index) const {
    return buffer_->element(index);
}

void Particles::textureIs(Ptr<Texture> texture) {
    texture_ = texture;
}

void Particles::statusIs(Status status) {
    if (status == status_) {
        return;
    }
    status_ = status;
    if (SYNCED == status) {
        syncHardwareBuffer();
    }
}

void Particles::defAttribute(Attribute id, GLuint size, void* offset) {
    GLuint stride = sizeof(Particle);
    glEnableVertexAttribArray(id);
    glVertexAttribPointer(id, size / sizeof(GLfloat), GL_FLOAT, 0, stride, offset);

}

void Particles::timeIs(float time) {
    time_ = time;
}

void Particles::timeInc(float time) {
    timeIs(time+time_);
}

void Particles::syncHardwareBuffer() {
    // Update the VAO/VBO containing the particle data
    buffer_->statusIs(AttributeBuffer::SYNCED);

    glBindVertexArray(id_);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_->id());
    defAttribute(POSITION, SIZE(position), OFFSET(position));
    defAttribute(VELOCITY, SIZE(velocity), OFFSET(velocity));
    defAttribute(TIME, SIZE(time), OFFSET(time));
    defAttribute(SIZE, SIZE(size), OFFSET(size));
    defAttribute(GROWTH, SIZE(growth), OFFSET(growth));
    defAttribute(ROTATION, SIZE(rotation), OFFSET(rotation));
    defAttribute(ALPHA, SIZE(alpha), OFFSET(alpha));
    defAttribute(LIFE, SIZE(life), OFFSET(life));
    defAttribute(SPIN, SIZE(spin), OFFSET(spin));
    glBindVertexArray(0);
}

void Particles::operator()(Ptr<Node::Functor> functor) {
    functor->operator()(std::static_pointer_cast<Particles>(shared_from_this()));
}
