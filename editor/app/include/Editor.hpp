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
    Document *m_document;  
    class EditorStorage
    {
    public:
        virtual void load(Document *) {};
        virtual void save(Document *) {};
    };
    class EditorDisplay
    {
    public:
        virtual void show(Document *) {};
    };
private:    
    std::shared_ptr<EditorStorage> m_storage;
    std::shared_ptr<EditorDisplay> m_display;

public:
    Editor(EditorStorage *storage, EditorDisplay *display)
        : m_storage(storage), m_display(display)
    {
    };
    ~Editor() = default;
    void createDocument(std::string name)
    {
        m_document = new Document();
        m_document->name = name;
    }

    void load(Document *doc)
    {
        m_storage->load(doc);
    }
    void save(Document *doc)
    {
        m_storage->save(doc);
    }
    void show(Document *doc)
    {
        m_display->show(doc);
    }
    Document * getDocument()
    {
        return m_document;
    }
};
