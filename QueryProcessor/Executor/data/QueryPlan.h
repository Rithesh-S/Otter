#pragma once

#include <cstdint>
#include <string>

enum class QueryType { INSERT, SEARCH, UPDATE, DELETE, INVALID };

struct QueryPlan {
    QueryType type = QueryType::INVALID;
    uint32_t id = 0;
    std::string data = "";
    bool success = true;
    std::string errorMsg = "";
};