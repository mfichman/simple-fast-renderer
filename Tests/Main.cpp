#include <SFR/Common.hpp>
#include <SFR/DeferredRenderTarget.hpp>
#include <SFR/ResourceManager.hpp>
#include <SFR/Mesh.hpp>
#include <SFR/Camera.hpp>
#include <SFR/Light.hpp>
#include <SFR/DeferredRenderer.hpp>
#include <SFR/Material.hpp>
#include <SFR/Transform.hpp>
#include <SFML/Window.hpp>
#include <stdexcept>
#include <iostream>

using namespace SFR;

void run() {
    /* Initialize the window */
    sf::Window window(sf::VideoMode(1200, 800, 32), "Window");

    /* Load OpenGL extensions and check for required features */
    GLint error = glewInit();
    if (GLEW_OK != error) {
        throw std::runtime_error((char*)glewGetErrorString(error));
    }
    if (!GLEW_VERSION_3_0) {
        throw std::runtime_error("This program requires OpenGL 3.0");
    }

    Ptr<SFR::ResourceManager> manager(new SFR::ResourceManager);
    Ptr<SFR::DeferredRenderer> renderer(new SFR::DeferredRenderer(manager.ptr()));
    Ptr<SFR::Effect> effect = manager->effectNew("Shaders/Material");

    Ptr<SFR::Material> material(new SFR::Material("Test"));
    material->textureIs("diffuse", manager->textureNew("Textures/MetalDiffuse.png"));
    material->textureIs("normal", manager->textureNew("Textures/MetalNormal.png"));
    material->textureIs("specular", manager->textureNew("Textures/MetalSpecular.png"));
    
    Ptr<SFR::Mesh> mesh = manager->meshNew("Meshes/SmoothSphere.obj");
    mesh->materialIs(material.ptr());
    mesh->effectIs(effect.ptr());


    Ptr<SFR::Transform> light0(new SFR::Transform);
    light0->childNew(new SFR::Light);
    light0->positionIs(SFR::Vector(0.f, 0.f, 2.f));

    Ptr<SFR::Transform> light1(new SFR::Transform);
    light1->childNew(new SFR::Light);
    light1->positionIs(SFR::Vector(0.f, 2.f, 0.f));


    Ptr<SFR::Transform> camera(new SFR::Transform);
    camera->childNew(new SFR::Camera);

    Ptr<SFR::Transform> other(new SFR::Transform);

    Ptr<SFR::Transform> root(new SFR::Transform);
    root->childNew(light0.ptr());
    root->childNew(light1.ptr());
    root->childNew(mesh.ptr());
    root->childNew(camera.ptr());

    glViewport(0, 0, window.GetWidth(), window.GetHeight());

    float x = -5.f;
    float z = 2.f;

    sf::Clock clock;

    /* Run the event loop */
    while (window.IsOpened()) {
        float elapsedTime = clock.GetElapsedTime();
        clock.Reset();

        sf::Event evt;
        while (window.GetEvent(evt)) {
            switch (evt.Type) {
            case sf::Event::Closed:
                window.Close();
                break;
            case sf::Event::Resized:
                glViewport(0, 0, evt.Size.Width, evt.Size.Height);
                break;
            default:
                break;
            }
        }

        if (window.GetInput().IsKeyDown(sf::Key::Left)) {
            x -= 2.f * elapsedTime;
        } 
        if (window.GetInput().IsKeyDown(sf::Key::Right)) {
            x += 2.f * elapsedTime;
        }
        if (window.GetInput().IsKeyDown(sf::Key::Up)) {
            z -= 2.f * elapsedTime;
        } 
        if (window.GetInput().IsKeyDown(sf::Key::Down)) {
            z += 2.f * elapsedTime;
        }

        light0->positionIs(SFR::Vector(0.f, 0.f, z));

        camera->transformIs(SFR::Matrix::look(
            SFR::Vector(5., 0., x),
            SFR::Vector(0.f, 0.f, 0.f),
            SFR::Vector(0.f, 1.f, 0.f)));

        root(renderer.ptr());

        window.Display();
    }
}

int main(int argc, char** argv) {

    try {
        run();
    } catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
}