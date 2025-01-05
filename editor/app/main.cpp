#include "include/IStorage.hpp"
#include "include/IDisplay.hpp"
#include "include/Editor.hpp"
#include <memory>

int main()
{
    std::shared_ptr<MainStorage> storage = std::make_shared<MainStorage>();
    std::shared_ptr<MainDisplay> display = std::make_shared<MainDisplay>();
    Editor editor(storage, display);
    editor.createDocument("Document1");
    editor.load(editor.getDocument());
    editor.save(editor.getDocument());
    editor.show(editor.getDocument());

    return 0;
}