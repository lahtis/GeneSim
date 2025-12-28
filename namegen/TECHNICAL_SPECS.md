# Technical Specifications: GeneSim NameGen v2.0.0 (NewYear-Edition)

This document details the internal architecture, database requirements, and technical logic of the **GeneSim NameGen v2.0.0**.

---

## 1. Data Architecture & Constants
The engine is built in C and utilizes a decoupled data model. The executable relies on external CSV and JSON files for its logic.

### File Paths (v2.0.0 Localization)
* `bin/Debug/data/fi-FI/`: Finnish data localization root.
* `social_logic.json`: Occupation and social class probability mappings.
* `generator.log`: Output log.

---

## 2. Database Schema Requirements (7/11 Standard)
The parser utilizes strict column-count validation. Data files must adhere to these counts to be loaded correctly.

### First & Middle Names — 7 Columns
* **Format:** `Name,Weight;P1;P2;P3;P4;P5;P6`
* **Validation:** Must contain exactly 6 semicolons.

### Surnames & Occupations — 11 Columns
* **Format:** `Name,Weight;P1;P2;P3;P4;P5;P6;P7;P8;P9;P10`
* **Validation:** Must contain exactly 10 semicolons.

---

## 3. Automation & Quality Assurance (QA) Tools
To maintain the high standards of the v2.0.0 engine, a dedicated Python-based testing and maintenance suite has been developed. These applications are essential for ensuring both data integrity and functional stability.

### 3.1 Data Integrity & Auto-Fix Tool (`gs_data_manager.py`)
Also known as **Validator v2**, this application is used to maintain the CSV database.
* **Auto-Correction:** If a row has an incorrect number of columns, the tool automatically pads or trims it to the 7/11 standard.
* **Format Audit:** Scans for missing weight delimiters (`,`) and ensures UTF-8 encoding.
* **Usage:** Should be run after any manual edit to the CSV files.

### 3.2 Master Integrity Suite (`gs_integrity_check.py`)
This is the official testing application for the compiled C-binary.
* **Full Integration Test:** Automatically cycles through all 11 historical periods to check for logic errors or crashes.
* **Output Validation:** Rigorously tests if the program correctly produces **JSON**, **CSV**, and **Plain Text** formats.
* **Determinism Check:** Confirms that the `--seed` parameter correctly locks the random number generator.

---

## 4. Core Logic Engines
### Distribution Model (`--weights`)
* **Weighted (Default):** Selection is based on historical frequency and period-specific probability.

### Temporal Logic (`--period`, `--year`)
* **Period 1-11:** Each period switches the active weight column in the database and adjusts the occupation pool from `social_logic.json`.

---

## 5. Build Requirements
* **Compiler:** GCC (recommended).
* **Dependencies:** **cJSON** library (MIT License).
* **License:** GPL v3.0 (Core Engine).

---
*Last Updated: 2025-12-28*

