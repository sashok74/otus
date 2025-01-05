#pragma once
#include <iostream>
#include "editor.hpp"

class MainDisplay : public Editor::EditorDisplay
{
public:
    void show(Document *doc) override
    {
        if (doc)
        {
            std::cout << "Document show name: " << doc->name << std::endl;
        }
    }
};