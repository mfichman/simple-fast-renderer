/*****************************************************************************
 * Simple, Fast Renderer (SFR)                                               *
 * CS249b                                                                    *
 * Matt Fichman                                                              *
 * February, 2011                                                            *
 *****************************************************************************/
#pragma once

#include "sfr/Common.hpp"
#include "sfr/Node.hpp"
#include "sfr/Quaternion.hpp"
#include "sfr/Matrix.hpp"
#include "sfr/Vector.hpp"
#include "sfr/Iterator.hpp"

namespace sfr {

/* Rotation, translation, scaling node with children. */
class Transform : public Node {
public:
    Transform(std::string const& name);

    Matrix const& transform() const;
    Vector position() const;
    Quaternion rotation() const;
    Iterator<Node> children();
    std::string const& name() const;

    void transformIs(Matrix const& transform);
    void positionIs(Vector const& position);
    void rotationIs(Quaternion const& rotation);

    template <typename T, typename ...Arg>
    Ptr<T> childIs(Arg... arg) {
        Ptr<T> child(new T(arg...));
        children_.push_back(child);
        return child;
    }

    void childIs(Ptr<Node> child);
    void childDel(Ptr<Node> child);

    void operator()(Ptr<Functor> functor);

private:
    std::vector<Ptr<Node>> children_;
    Matrix transform_;
    std::string name_;
};


}