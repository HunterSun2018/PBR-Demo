#pragma once
#include "application.hpp"

class Demo : public Application
{
public:
    static std::shared_ptr<Demo>
    create();

    virtual ~Demo() {}

    virtual void set_gltf(std::string_view path) = 0;
};
