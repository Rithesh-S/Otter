#include "Transaction.h"

bool Transaction::isActive = false;
TStatus Transaction::status = TStatus::IDLE;

bool Transaction::begin() {
    if(isActive) return false;
    isActive = true;
    status = TStatus::PROGRESS;
    return true;
}

bool Transaction::isFailed() { return status == TStatus::PROGRESS; }

void Transaction::commit() {
    isActive = false;
    status = TStatus::IDLE;
}