//
// Created by alex on 12/5/24.
//

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include "Logger.h"

namespace Bcg {
    using json = nlohmann::json;
    class Config {
    public:
        static bool load_config(std::string filepath) {
            std::ifstream ifs(filepath);
            if (!ifs) {
                // Handle error: config file not found
                LOG_ERROR(fmt::format("Could not open {}", filepath));
                return false;
            }

            LOG_INFO(fmt::format("Config::load_config: {}", filepath));
            // Parse the JSON
            json config_json;
            ifs >> config_json;

            // Use SetConfig to store the configuration globally
            set_config(config_json);
            return true;
        }

        static void set_config(const json &j) {
            config_ = j;
        }

        static std::string get_string(const std::string &path) {
            // Implement a method to navigate the JSON object by path (e.g. "font.path")
            return navigate<std::string>(path);
        }

        static float get_float(const std::string &path) {
            return navigate<float>(path);
        }

        static int get_int(const std::string &path) {
            return navigate<int>(path);
        }

    private:
        template<typename T>
        static T navigate(const std::string &path) {
            // Simple parsing by splitting on '.' for nested keys
            // E.g., "font.path" -> ["font","path"]
            std::istringstream iss(path);
            std::string token;
            const json *current = &config_;
            while (std::getline(iss, token, '.')) {
                if (!current->contains(token)) {
                    throw std::runtime_error("Config key not found: " + path);
                }
                current = &(*current)[token];
            }
            return current->get<T>();
        }

        static json config_;
    };
}

#endif //CONFIGFILE_H
