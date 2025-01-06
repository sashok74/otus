#pragma once
#include <string>
#include <memory>
#include <vector>
#include "GraphicPrimitive.hpp"

class Document
{
private:
    std::vector<std::shared_ptr<GraphicPrimitive>> m_primitives;

public:
    std::string name;
    void addPrimitive(std::shared_ptr<GraphicPrimitive> primitive)
    {
        m_primitives.push_back(std::move(primitive));
    }
    std::vector<std::shared_ptr<GraphicPrimitive>> getPrimitives() {return m_primitives;}
};

class Editor
{
private:
    std::unique_ptr<Document> m_document;

public:
    // Объявляем вложенные классы как public
    class IStorage
    {
    public:
        virtual void load(Document *) = 0;
        virtual void save(Document *) = 0;
        virtual ~IStorage() = default;
    };

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

    void createDocument(const std::string &name)
    {
        m_document = std::make_unique<Document>();
        m_document->name = name;
    }

    void load()
    {
        if (m_document)
            m_storage->load(m_document.get());
    }

    void save()
    {
        if (m_document)
            m_storage->save(m_document.get());
    }

    void show()
    {
        if (m_document)
            m_display->show(m_document.get());
    }

    Document *getDocument() const
    {
        return m_document.get();
    }
};
