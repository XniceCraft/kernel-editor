#pragma once

#include "ftxui/component/component.hpp"

class BaseTab {
    public:
        virtual ftxui::Component getTab() = 0;
};