#pragma once

#include <filesystem>
#include <string>

class FileSystem {
public:
    static std::string getPath(const std::string &relativePath) {
        // Get the path of the executable
        std::filesystem::path executablePath = std::filesystem::current_path();
        return (executablePath.parent_path() / "../SpaceScene/" / relativePath).string();
    }
};
