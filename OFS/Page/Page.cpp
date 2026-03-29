#include "Page.h"

Page::Page(uint16_t page_no) { 
    std::memset(this, 0, sizeof(Page));
    this -> page_no = page_no;
}

bool Page::isFull() { return record_count == MAX_RECORDS; }

bool Page::contains(uint32_t id) { return findRecordIndex(id) != -1; }

bool Page::hasDeletion() {
    for(size_t i = 0; i < record_count; i++) 
        if(!slots[i].active) return true;
    return false;
}

bool Page::removeData(uint32_t target_id) { 
    int low = 0, high = record_count - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (slots[mid].record_id == target_id && slots[mid].active) {
            slots[mid].active = false;
            return true;
        }
        if (slots[mid].record_id < target_id) low = mid + 1;
        else high = mid - 1;
    }
    return false;
}

int Page::findRecordIndex(uint32_t target_id) { 
    int low = 0, high = record_count - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (slots[mid].record_id == target_id && slots[mid].active) 
            return slots[mid].index;
        if (slots[mid].record_id < target_id) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

bool Page::readData(DataNode& record, uint32_t target_id) {
    int index = findRecordIndex(target_id);
    if(index == -1) return false;
    else {
        record = records[index];
        return true;
    }
}
    
bool Page::writeData(DataNode &record) {
    if (record_count >= 30) {
        if(hasDeletion()) clean();
        else return false;
    };

    int low = 0, high = record_count - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (slots[mid].record_id == record.getId()) {
            if (slots[mid].active) return false; 

            records[slots[mid].index] = record; 
            slots[mid].active = true;
            return true;
        }
        
        if (slots[mid].record_id > record.getId()) high = mid - 1;
        else low = mid + 1;
    }

    int pos = low;
    if (pos < record_count) std::memmove(&slots[pos + 1], &slots[pos], (record_count - pos) * sizeof(Slot));

    records[record_count] = record;

    slots[pos].record_id = record.getId();
    slots[pos].index = record_count;
    slots[pos].active = true;
    
    record_count++;
    return true;
}

void Page::clean() {
    std::vector<DataNode> liveRecords;
    std::vector<Slot> liveSlots;

    for (int i = 0; i < record_count; i++) {
        if (slots[i].active) {
            liveRecords.push_back(records[slots[i].index]);
            liveSlots.push_back(slots[i]);
        }
    }

    uint32_t old_id = this -> page_no;
    std::memset(this, 0, sizeof(Page));
    this -> page_no = old_id;

    for (size_t i = 0; i < liveRecords.size(); i++) {
        records[i] = liveRecords[i];
        slots[i] = liveSlots[i];
        slots[i].index = i;
        slots[i].active = true;
    }
    
    record_count = liveRecords.size();
}