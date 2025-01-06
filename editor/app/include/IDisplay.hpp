#pragma once
#include <iostream>
#include "Editor.hpp"

class MainDisplay : public Editor::IDisplay
{
public:
    void show(Document *doc) override
    {
        if (doc)
        {
             for (const auto &primitive : doc->getPrimitives())
            {
                std::cout << "display " <<  primitive->m_name << "\n";
            }
        }
    }
};