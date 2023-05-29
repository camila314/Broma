#pragma once

#include "state.hpp"
#include <algorithm>
#include <iostream>

namespace broma {
    inline void sort_class(Class cls, Root& root, std::vector<Class>& output) {
        root.classes.erase(std::remove(root.classes.begin(), root.classes.end(), cls), root.classes.end());
        for (auto name : cls.depends) {
            if (root[name])
                sort_class(*root[name], root, output);
        }
        output.push_back(cls);
    }

    inline void post_process(Root& root) {
        std::vector<Class> out;

        while (root.classes.size())
            sort_class(root.classes[0], root, out);

        root.classes = out;
    }
}