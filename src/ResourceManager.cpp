/*****************************************************************************
 * Simple, Fast Renderer (SFR)                                               *
 * CS249b                                                                    *
 * Matt Fichman                                                              *
 * February, 2011                                                            *
 *****************************************************************************/

#include "SFR/Common.hpp"
#include "SFR/ResourceManager.hpp"
#include "SFR/WavefrontLoader.hpp"
#include "SFR/TextureLoader.hpp"
#include "SFR/Mesh.hpp"
#include "SFR/Material.hpp"
#include "SFR/Texture.hpp"
#include "SFR/Transform.hpp"
#include "SFR/EffectLoader.hpp"
#include "SFR/Effect.hpp"
#include <algorithm>

using namespace SFR;

ResourceManager::ResourceManager() {
    meshLoader_ = new WavefrontLoader(this);
    effectLoader_ = new EffectLoader;
    textureLoader_ = new TextureLoader;
    notifieeNew(meshLoader_.ptr());
    notifieeNew(effectLoader_.ptr());
    notifieeNew(textureLoader_.ptr());
}

Mesh* ResourceManager::meshNew(const std::string& name) {
    Mesh* mesh = mesh_[name].ptr();
    if (!mesh) {
        std::cout << "Loading " << name << std::endl;
        mesh_[name] = mesh = new Mesh(name);
        for (size_t i = 0; i < notifiee_.size(); i++) {
            notifiee_[i]->onMeshNew(mesh);
        }
    }
    return mesh;
}

Material* ResourceManager::materialNew(const std::string& name) {
    Material* material = material_[name].ptr();
    if (!material) {
        material_[name] = material = new Material(name);
        
        std::cout << "Loading " << name << std::endl;
        for (size_t i = 0; i < notifiee_.size(); i++) {
            notifiee_[i]->onMaterialNew(material);
        }
    }
    return material;
}

Texture* ResourceManager::textureNew(const std::string& name) {
    Texture* texture = texture_[name].ptr();
    if (!texture) {
        texture_[name] = texture = new Texture(name);
        for (size_t i = 0; i < notifiee_.size(); i++) {
            notifiee_[i]->onTextureNew(texture);
        }
    }
    return texture;
}

Transform* ResourceManager::nodeNew(const std::string& name) {
    Transform* node = node_[name].ptr();
    if (!node) {
        node_[name] = node = new Transform();
        node_[name]->nameIs(name);
        for (size_t i = 0; i < notifiee_.size(); i++) {
            notifiee_[i]->onNodeNew(node);
        }
    }
    return node;
}


Effect* ResourceManager::effectNew(const std::string& name) {
    Effect* effect = effect_[name].ptr();
    if (!effect) {
        effect_[name] = effect = new Effect(name);
        for (size_t i = 0; i < notifiee_.size(); i++) {
            notifiee_[i]->onEffectNew(effect);
        }
    }
    return effect;
}

Mesh* ResourceManager::mesh(const std::string& name) const {
    std::map<std::string, Ptr<Mesh> >
        ::const_iterator i = mesh_.find(name);
    if (i == mesh_.end()) {
        return 0;
    } else {
        return i->second.ptr();
    }
}

Material* ResourceManager::material(const std::string& name) const {
    std::map<std::string, Ptr<Material> >
        ::const_iterator i = material_.find(name);
    if (i == material_.end()) {
        return 0;
    } else {
        return i->second.ptr();
    }
}

Texture* ResourceManager::texture(const std::string& name) const {
    std::map<std::string, Ptr<Texture> >
        ::const_iterator i = texture_.find(name);
    if (i == texture_.end()) {
        return 0;
    } else {
        return i->second.ptr();
    }
}

Transform* ResourceManager::node(const std::string& name) const {
    std::map<std::string, Ptr<Transform> >
        ::const_iterator i = node_.find(name);
    if (i == node_.end()) {
        return 0;
    } else {
        return i->second.ptr();
    }
}

Effect* ResourceManager::effect(const std::string& name) const {
    std::map<std::string, Ptr<Effect> >
        ::const_iterator i = effect_.find(name);
    if (i == effect_.end()) {
        return 0;
    } else {
        return i->second.ptr();
    }
}

void ResourceManager::notifieeNew(Notifiee* notifiee) {
    if (notifiee) {
        notifiee_.push_back(notifiee);
    }
}

void ResourceManager::notifieeDel(Notifiee* notifiee) {
    std::remove(notifiee_.begin(), notifiee_.end(), notifiee);
}
