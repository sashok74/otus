#pragma once
#include <iostream>
#include "Editor.hpp"

class MainStorage : public Editor::IStorage
{
public:
    void load(Document *doc) override
    {
        if (doc)
        {
            std::cout << "Document load name: " << doc->name << std::endl;
        }
    }

    void save(Document *doc) override
    {
        if (doc)
        {
            std::cout << "Document save name: " << doc->name << std::endl;
        }
    }
};