# 🦦 Otter DB

**A persistent, disk-based NoSQL storage engine with B-Tree indexing and WAL recovery.**

Otter DB is a high-performance storage kernel built from the ground up in C++17. It manages its own binary storage format, implements a page-aligned indexing system, and ensures data integrity through a custom Write-Ahead Log (WAL).

---

## 🚀 Key Features

* **Disk-Resident B-Tree:** High-performance indexing with  lookup, using 4096-byte page alignment to match hardware sectors.
* **Crash Resilience (WAL):** Uses a Write-Ahead Log with **CRC32 checksums** to automatically recover from system failures or interrupted transactions.
* **Space Reclamation:** A specialized `InsertionQueue` recycles disk offsets from deleted records, preventing storage bloat.
* **LRU Buffer Pool:** Manages file streams and data frames in memory to minimize expensive Disk I/O.
* **SQL-like CLI:** Features a full query processing pipeline including a Lexer, recursive-descent Parser, and AST-based Executor.

---

## 🏗️ Architecture & Internals

### 1. Storage Layer (OFS)

Data is persisted in 128-byte fixed-width frames (`DataNode`).

* **Payload:** 4-byte ID + 124-byte data content.
* **Chunking:** Data is spread across multiple `.bin` files. The engine uses an **LRU Cache** to manage file handles, allowing it to scale beyond OS file descriptor limits.

### 2. Indexing Layer

The B-Tree is the "brain" of Otter DB.

* **Alignment:** Nodes are strictly padded to **4096 bytes**, ensuring every index read is a single physical page operation.
* **Reclamation:** When a record is deleted, the index entry is removed and the physical offset is returned to the `InsertionQueue` for the next `INSERT`.

### 3. Query Pipeline

Otter DB follows a classic compiler architecture:

1. **Lexer:** Tokenizes input strings into commands, IDs, and Literals.
2. **Parser:** Validates grammar and builds an **Abstract Syntax Tree (AST)**.
3. **Executor:** Translates the AST into a `QueryPlan` for the Storage Manager.

---

## ⌨️ Query Language Reference

| Command | Syntax | Description |
| --- | --- | --- |
| **INSERT** | `INSERT 101 'Data';` | Persists a new record (Max 124 chars). |
| **SEARCH** | `SEARCH 101;` | Retrieves data via B-Tree index. |
| **UPDATE** | `UPDATE 101 'New';` | Performs an in-place binary update. |
| **DELETE** | `DELETE 101;` | Removes index and recycles disk space. |
| **EXIT** | `EXIT;` | Shuts down the engine gracefully. |

---

## 🛠️ Build & Installation

### Prerequisites

* **Compiler:** GCC (MinGW-w64 / UCRT64) with C++17 support.
* **OS:** Windows (Required for Virtual Terminal/ANSI color support).

### Building

You can use the provided `run.bat` script:

```bash
./run.bat

```

---

## 📈 Technical Specs

* **B-Tree Order ():** 204
* **Node Size:** 4096 Bytes
* **Data Record Size:** 128 Bytes
* **WAL Frame:** 1034 Bytes
* **Integrity Check:** CRC-32-IEEE

---

## 📄 License

This project is licensed under the MIT License.

---

### 💡 Pro-Tip

*For the best visual experience, run the CLI in a terminal that supports ANSI colors (like Windows Terminal or VS Code Integrated Terminal).*

---