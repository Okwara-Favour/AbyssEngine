#include <SFML/Graphics.hpp>
#include <thread>
#include "Editor.h"

int main() {
    Editor editor;
    editor.Run();
    editor.CloseTabs();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::exit(0);
}