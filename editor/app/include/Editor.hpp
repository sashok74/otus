#pragma once
#include <string>
#include <memory>

class Document
{
public:
    std::string name;
};

class Editor
{
private:
    std::unique_ptr<Document> m_document;

public:
    // Объявляем вложенные классы как public
    class EditorStorage
    {
    public:
        virtual void load(Document *) = 0;
        virtual void save(Document *) = 0;
        virtual ~EditorStorage() = default;
    };

    class EditorDisplay
    {
    public:
        virtual void show(Document *) = 0;
        virtual ~EditorDisplay() = default;
    };

private:
    std::shared_ptr<EditorStorage> m_storage;
    std::shared_ptr<EditorDisplay> m_display;

public:
    Editor(std::shared_ptr<EditorStorage> storage, std::shared_ptr<EditorDisplay> display)
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
