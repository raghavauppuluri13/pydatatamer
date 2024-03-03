#pragma once
#include "data_tamer_parser.hpp"
#include <iostream>
#include <variant>
#include <vector>
#define EXTRACT_INDEX(s)                                                       \
    std::stoi((s).substr((s).find('[') + 1, (s).find(']') - (s).find('[') - 1))

bool array_name_eq(const std::string &input, const std::string &other) {
    size_t pos = input.find('[');
    return input.substr(0, pos) == other;
}

struct Proprioception {
    std::array<double, 2> qpos;
    std::array<double, 2> qvel;
    std::array<double, 2> qpos_d;
    std::array<double, 2> qeffort;
    std::array<double, 2> torq_bias;

    Proprioception() {}

    inline void update(std::string name,
                       const DataTamerParser::VarNumber &data) {
        int index = EXTRACT_INDEX(name);

        if (array_name_eq(name, "qpos")) {
            qpos[index] = std::get<double>(data);
        } else if (array_name_eq(name, "qvel")) {
            qvel[index] = std::get<double>(data);
        } else if (array_name_eq(name, "qpos_d")) {
            qpos_d[index] = std::get<double>(data);
        } else if (array_name_eq(name, "qeffort")) {
            qeffort[index] = std::get<double>(data);
        } else if (array_name_eq(name, "torq_bias")) {
            torq_bias[index] = std::get<double>(data);
        } else {
            std::cerr << "unknown series name: " << name << std::endl;
            throw std::runtime_error("Unknown series!");
        }
    }
};
