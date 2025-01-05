#pragma once
#include <string>
#include <memory>
#include "Editor.hpp"

class MainStorage: public Editor::EditorStorage
{   
public:
    void load(Document *doc) override
    {
        std::cout << "Document load name: " << doc->name << std::endl;
    }
    void save(Document *doc) override
    {
        std::cout << "Document save name: " << doc->name << std::endl;
    }
};
