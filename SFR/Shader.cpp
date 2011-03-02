/*****************************************************************************
 * Simple, Fast Renderer (SFR)                                               *
 * CS249b                                                                    *
 * Matt Fichman                                                              *
 * February, 2011                                                            *
 *****************************************************************************/

#include "SFR/Shader.hpp"

using namespace SFR;

Shader::Shader(GLenum type, const std::string& name) {
    name_ = name;
    id_ = 0;
    status_ = DIRTY;
    type_ = type;

    id_ = glCreateShader(type_);
}

Shader::~Shader() {
    glDeleteShader(id_);
}

const std::string& Shader::name() const {
    return name_;
}

GLenum Shader::type() const {
    return type_;
}

const std::string& Shader::source() const {
    return source_;
}

Shader::Status Shader::status() const {
    return status_;
}

GLuint Shader::id() const {
    return id_;
}

void Shader::sourceIs(const std::string& source) {
    if (source == source_) {
        return;
    }
    source_ = source;
    statusIs(DIRTY);

    for (size_t i = 0; i < notifiee_.size(); i++) {
        notifiee_[i]->onSource();
    }
}

void Shader::notifieeNew(Notifiee* notifiee) {
    if (notifiee) {
        notifiee_.push_back(notifiee);
    }
}

void Shader::statusIs(Status status) {
    if (status_ == status) {
        return;
    }
    if (COMPILED == status) {
        const GLchar* strings = source_.c_str();
        GLint lengths = source_.length();
        glShaderSource(id_, 1, &strings, &lengths);
        glCompileShader(id_);
    }
    status_ = status;

    for (size_t i = 0; i < notifiee_.size(); i++) {
        notifiee_[i]->onStatus();
    }
}