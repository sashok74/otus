#pragma once
#include <string>
#include <memory>
#include <iostream>
#include "Editor.hpp"

class MainDisplay: public Editor::EditorDisplay
{
public:
    void show(Document *doc) override
    {
        std::cout << "Document show name: " << doc->name << std::endl;
    }
};