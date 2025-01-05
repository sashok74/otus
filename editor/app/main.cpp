#include "include/IStorage.hpp"
#include "include/IDisplay.hpp"
#include "include/Editor.hpp"

int main()
{
    MainStorage storage;
    MainDisplay display;
    Editor editor(&storage, &display);
    editor.createDocument("Document1");
    editor.load(editor.getDocument());
    editor.save(editor.getDocument());
    editor.show(editor.getDocument());

    return 0;
}