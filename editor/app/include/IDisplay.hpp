#pragma once
#include <iostream>
#include "Editor.hpp"

/**
 * @class MainDisplay
 * @brief Конкретная реализация интерфейса IDisplay для отображения примитивов документа.
 * 
 * Этот класс предоставляет реализацию метода show для отображения имен примитивов,
 * содержащихся в объекте Document.
 * 
 * @method void show(Document *doc)
 * @brief Отображает имена примитивов в данном документе.
 * 
 * Здесь возможно реализовать более сложную логику отображения, например, графический интерфейс.
 * другой вариант - вывод в консоль.
 * 
 * @param doc Указатель на объект Document, примитивы которого должны быть отображены.
 *            Если документ равен нулю, метод ничего не делает.
 */
class MainDisplay : public Editor::IDisplay
{
public:
    void show(Document *doc) override
    {
        if (doc)
        {
             for (const auto &[_,primitive] : doc->getPrimitives())
            {
                std::cout << "display " <<  primitive->m_name << "\n";
            }
        }
    }
};