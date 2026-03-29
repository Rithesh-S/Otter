#pragma once

#include "../../Page/Page.h"

struct BufferFrame {
    private:
        bool is_dirty = false;
        uint16_t file_id;
        uint16_t page_no;
        Page page;

    public:
        BufferFrame() = default;
        BufferFrame(uint16_t file_id, uint16_t page_no, Page page) : file_id(file_id), page_no(page_no), page(page) {};

        Page* getPage() { return &page; }
        bool isDirty() { return is_dirty; }
        void makeDirty() { is_dirty = true; }
        std::pair<uint16_t, uint16_t> getFileIdAndPageNo() { return { file_id, page_no }; }
};