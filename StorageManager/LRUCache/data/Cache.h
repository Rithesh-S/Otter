#pragma once

#include <memory>
#include <fstream>
#include <cstdint>

struct Cache {
    uint32_t file_id;
    std::unique_ptr<std::fstream> file;

    Cache(uint32_t file_id, std::unique_ptr<std::fstream> file) : file_id(file_id), file(std::move(file)) {}
};