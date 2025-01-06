#include "include/IStorage.hpp"
#include "include/IDisplay.hpp"
#include "include/Editor.hpp"
#include <memory>

int main()
{
    PrimitiveFactory factory;
    std::shared_ptr<MainStorage> storage = std::make_shared<MainStorage>();
    std::shared_ptr<MainDisplay> display = std::make_shared<MainDisplay>();
    Editor editor(storage, display);
    editor.createDocument("Document1");
    editor.load();
    Document *doc = editor.getDocument();
    auto circle = factory.createPrimitive("Circle", 10.0);
    auto line = factory.createPrimitive("Line", 20.0); 
    doc->addPrimitive(circle);
    doc->addPrimitive(line);
    editor.show();
    editor.save();
    
    return 0;
}