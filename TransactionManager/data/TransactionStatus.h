#pragma once

enum class TStatus {
    SEARCH,UPDATE,
    DELETE,INSERT,

    BEGIN,COMMIT,
    CHECKPOINT,ROLLBACK
};