#include "Executor.h"
#include "../../CLIComponent/CLInterface.h"
#include "../../StorageManager/StorageManager.h"

Executor::Executor(Expr* root, StorageManager* sm, CLInterface* cli) : 
    root(root), sm(sm), cli(cli) { plan = std::make_unique<QueryPlan>(); }

void Executor::execute() { 
    createPlan(root); 
    try {
        executePlan();
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}

void Executor::createPlan(Expr* node) {
    if(!node) {
        plan.get() -> success = false;
        plan.get() -> errorMsg = "AST Build Failed!";
        return;
    }

    if(Literal* literal = dynamic_cast<Literal*>(node)) {
        if(literal -> type == TokenType::ID) plan.get() -> id = std::stoul(literal -> data);
        else if(literal -> type == TokenType::DATA) plan.get() -> data = literal -> data;
        return;
    }
    
    if(Unary* unary = dynamic_cast<Unary*>(node)) {
        if(unary -> type == TokenType::SEARCH) plan.get() -> type = QueryType::SEARCH;
        else if(unary -> type == TokenType::DELETE) plan.get() -> type = QueryType::DELETE;
        createPlan(unary -> left.get());
    }
    
    if(Binary* binary = dynamic_cast<Binary*>(node)) {
        if(binary -> type == TokenType::INSERT) plan.get() -> type = QueryType::INSERT;
        else if(binary -> type == TokenType::UPDATE) plan.get() -> type = QueryType::UPDATE;
        createPlan(binary -> left.get());
        createPlan(binary -> right.get());
    }
}

void Executor::executePlan() {
    std::string id;
    if(plan.get() -> success) { 
        switch (plan.get() -> type) {
            case QueryType::INSERT:
                id = std::to_string(plan.get() -> id);
                sm -> writeRecord(plan.get() -> id, plan.get() -> data);
                cli -> printFormat({ id, "" }, QueryType::INSERT);
                break;
            case QueryType::UPDATE:
                id = std::to_string(plan.get() -> id);
                sm -> updateRecord(plan.get() -> id, plan.get() -> data);
                cli -> printFormat({ id, "" }, QueryType::UPDATE);
                break;
            case QueryType::SEARCH:
                cli -> printFormat(sm -> readRecord(plan.get() -> id), QueryType::SEARCH);
                break;
            case QueryType::DELETE:
                id = std::to_string(plan.get() -> id);
                sm -> deleteRecord(plan.get() -> id);
                cli -> printFormat({ id, "" }, QueryType::DELETE);
                break;
            default:
                throw std::runtime_error("\033[31mERROR: Unexpected Query.\033[0m");
                break;
        }
    } else throw std::runtime_error(plan.get() -> errorMsg);
}