#pragma once
#include <iostream>
#include "editor.hpp"

class MainStorage : public Editor::EditorStorage
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