#include "ftxui/component/component.hpp"
#include "cpu.hpp"

using namespace ftxui;

Component CPUTab::getTab() {
    return Renderer([&] {
        return hbox();
    });
}