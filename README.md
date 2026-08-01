# OSI (Operating System Infrastructure)

A modern, high-performance C++17 utility and infrastructure library providing cross-platform system abstractions, thread management, async procedure combination, and built-in unit testing.

---

## 🌟 Key Features

* **Modern C++17**: Standard compliance (`-std=c++17`) with **zero external Boost dependencies**.
* **Cross-Platform OS Abstractions**: Operating system env vars, system time/clocks, process path/deploy management (`envs/`).
* **Concurrency & Thread Pools**: Task queue runners, asynchronous timers (`stopwatch`), and thread-safe data queues (`threadpool/`, `formation/concurrent/`).
* **Async Flow Composition**: Stateful asynchronous event combination (`procedure/event_comb.hpp`).
* **Built-in Unit Test Framework (`UTT`)**: Lightweight macro-driven test suite runner (`utt/`).

---

## 📁 Repository Structure

| Module | Description |
| :--- | :--- |
| **`envs/`** | OS environment flags, path resolution (`deploys`), system clock/time (`nowadays`), process info (`exeinfo`). |
| **`threadpool/`** | Thread pool task runners (`queue_runner`), asynchronous stopwatch timer (`stopwatch`). |
| **`procedure/`** | Asynchronous event combinators (`event_comb.hpp`). |
| **`formation/`** | Thread-safe concurrent queues (`slimqless2`), ternary state, movable containers. |
| **`utils/`** | String manipulation (`strutils`), UUID generation (`uuid`), call stack trace (`call_stack`), auto-removing folders. |
| **`app_info/`** | Assertion macros (`assert.hpp`), tracing (`trace.hpp`), and error handling (`error_handle.hpp`). |
| **`primitive/`** | Custom bytecode virtual machine & interpreter engine (`data_ref`, `data_block`, `registers`, `memory_space`, `executor`, `simulator`). |
| **`utt/`** & **`utt_cases/`** | Unit testing framework (`utt`) and suite of test cases across all modules. |

---

## 🚀 Building & Testing

`osi` uses [`autocompile`](https://github.com/hzj-jie/autocompile) and `maketree` for parallel directory-level build orchestration.

### 1. Bootstrapping a Fresh Checkout
Initialize `Makefile`s across all target directories:

```bash
maketree -f /path/to/autocompile/bootstrap
```

### 2. Building the Project
Build all headers, precompiled headers (PCH), and test binaries:

```bash
maketree
```

### 3. Running Unit Tests
Execute the main test runner:

```bash
./utt/utt
```

---

## 📄 License
Released under the MIT License.
