#pragma once
#include <iostream>
#include "Editor.hpp"

/**
 * @class MainStorage
 * @brief Пример реализации хранилища для редактора.
 */
class MainStorage : public Editor::IStorage
{
public:
    /**
     * @brief загружает документ.
     * @param doc указатель на документ, который нужно загрузить.
     */
    void load(Document *doc) override
    {
        if (doc)
        {
            std::cout << "Document load name: " << doc->name << std::endl;
        }
    }

    /**
     * @brief сохраняет документ.
     * здесь возможно реализовать сохранение документа в файл или базу данных.
     * 
     * @param doc указатель на документ, который нужно сохранить.
     */
    void save(Document *doc) override
    {
        if (doc)
        {
            std::cout << "Document save name: " << doc->name << std::endl;
        }
    }
};
    