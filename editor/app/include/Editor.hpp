#pragma once
#include <string>
#include <memory>
#include <map>
#include "GraphicPrimitive.hpp"

/**
 * @class Document
 * @brief Класс, представляющий документ, содержащий графические примитивы.
 */
class Document
{
private:
    std::map<int, std::shared_ptr<GraphicPrimitive>> m_primitives;

public:
    std::string name;
    /**
     * @brief Добавляет графический примитив в документ.
     * @param primitive Умный указатель на графический примитив, который нужно добавить.
     */
    void addPrimitive(std::shared_ptr<GraphicPrimitive> primitive)
    {
        m_primitives.insert(std::make_pair(m_primitives.size(), primitive));
    }
    
    void removePrimitive(int index)
    {
        m_primitives.erase(index);
    }

    /**
     * @brief Получает список графических примитивов в документе.
     * @return Вектор умных указателей на графические примитивы.
     * @note Возвращаемый вектор содержащий умные указатели на объекты GraphicPrimitive.
     */
    std::map<int, std::shared_ptr<GraphicPrimitive>> getPrimitives() { return m_primitives; }
};

/**
 * @class Editor
 * @brief Класс, представляющий редактор, который управляет документом.
 * @note минимальная реализаця для демонстрации dpi
 */
class Editor
{
private:
    /**
     * @brief Указатель на документ, которым управляет редактор.
     * @note Указатель на объект Document, который управляется редактором.
     * здесь может быть контейнер для хранения документов, если редактор должен управлять несколькими документами.
     * возможно они будут разного типа.
     */
    std::unique_ptr<Document> m_document;

public:
    /**
    * @class IStorage 
    * @brief Интерфейс для хранения документов.
    * @note Интерфейс для хранения документов, который должен быть реализован конкретным классом.
     */
    class IStorage
    {
    public:
        virtual void load(Document *) = 0;
        virtual void save(Document *) = 0;
        virtual ~IStorage() = default;
    };
    /**
     * @class IDisplay 
     * @brief Интерфейс для отображения документов.
     * @note Интерфейс для отображения документов, который должен быть реализован конкретным классом.
     */ 
    class IDisplay
    {
    public:
        virtual void show(Document *) = 0;
        virtual ~IDisplay() = default;
    };

private:
    std::shared_ptr<IStorage> m_storage;
    std::shared_ptr<IDisplay> m_display;

public:
    Editor(std::shared_ptr<IStorage> storage, std::shared_ptr<IDisplay> display)
        : m_storage(std::move(storage)), m_display(std::move(display)) {}
    /**
     * @brief Создает новый документ с заданным именем.
     * @param name Имя документа.
     * note Создает новый документ с заданным именем.
     */
    void createDocument(const std::string &name)
    {
        m_document = std::make_unique<Document>();
        m_document->name = name;
    }
    /**
     * @brief Загружает документ из хранилища.
     * @note Загружает документ из хранилища, используя указатель на объект Document.
     */
    void load()
    {
        if (m_document)
            m_storage->load(m_document.get());
    }
    /**
     * @brief Сохраняет документ в хранилище.
     * @note Сохраняет документ в хранилище, используя указатель на объект Document.
     */
    void save()
    {
        if (m_document)
            m_storage->save(m_document.get());
    }
    /**
     * @brief Отображает документ.
     * @note Отображает документ, используя указатель на объект Document.
     */
    void show()
    {
        if (m_document)
            m_display->show(m_document.get());
    }

    /**
     * @brief Возвращает указатель на документ.
     * @return Указатель на объект Document.
     */
    Document *getDocument() 
    {
        return m_document.get();
    }
};
