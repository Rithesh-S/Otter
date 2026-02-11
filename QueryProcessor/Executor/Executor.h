#pragma once

#include <iostream>
#include <memory>
#include "./data/QueryPlan.h"
#include "../Parser/Node/ASTNode.h"

class CLInterface;
class StorageManager;

class Executor {
    private:
        Expr* root = nullptr;
        StorageManager* sm = nullptr;
        CLInterface* cli = nullptr;
        std::unique_ptr<QueryPlan> plan = nullptr;

        void createPlan(Expr* node);
        void executePlan();

    public:
        Executor(Expr* root, StorageManager* sm, CLInterface* cli);
        ~Executor() = default;

        void execute();
};