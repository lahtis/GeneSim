# GeneSim NameGen v2.0.0 (NewYear-Edition)

**GeneSim NameGen** is a high-performance command-line utility for generating historically accurate 19th-century Finnish personas. It features a robust deterministic engine, weighted social logic, and support for complex family structures.

---

## 🛠️ Official Command Line Interface

### Usage
`namegen [options]`

### Documentation
* **`--list-periods`, `-lp [fi|en]`** Show the 11 historical eras and detailed usage guidelines in Finnish or English.

### Generation & Logic
* **`--period <1-11>`** Select a specific historical period (1: oldest, 11: newest).
* **`--year <number>`** Set the simulation year (e.g., `--year 1880`).
* **`--weights`, `-w`** Use historical weights for name distribution (**enabled by default**).
* **`--no-weights`** Use uniform distribution (all names in the database have an equal chance).
* **`--count <number>`** Number of names, couples, or families to generate.
* **`--seed <number>`** Set a specific random seed for reproducible results.

### Generation Modes
* **`--single`** Generate single names (default).
* **`--couple`** Generate couples (linked male & female).
* **`--family`** Generate full families (parents and children).
* **`--gender <male|female|both>`** Force gender selection for generated names.

### Advanced Options
* **`--middle-chance <0-100>`** Probability for a middle name (%).
* **`--shared-surname`** Force couples/families to share the same surname.
* **`--maiden-name`** Show maiden names for wives using the **"née"** prefix.
* **`--force-manual`** Disable automatic period-based logic.
* **`--age`, `-A`** Show birth years for generated names (requires `--year`).

### Output & Formatting
* **`--output <plain|csv|json>`** Choose the output format for the results.
* **`--stdout`** Force output to the console even if a file destination is set.
* **`--log [on|off]`** Enable or disable logging of generated data to `generator.log`.

### System
* **`--verbose`, `-v`** Show detailed loading, path validation, and generation debug info.
* **`--help`** Show the help menu.
* **`--version`** Show version information (`2.0.0 NewYear-Edition`).

---

## 📝 Migration Notes (Huomioitavaa)

* **Weighting:** The program now uses `--weights` by default, making name generation historically plausible (common names appear more frequently).
* **Logging:** You can now toggle file logging with `--log off` to prevent `generator.log` from growing too large.
* **Periods:** The new `--list-periods` feature is essential for navigating the 11 distinct historical eras supported in this version.

---

## ⚙️ Build & Maintenance

* **Database Standard:** v2.0.0 requires strict 7-column (First Names) and 11-column (Surnames/Occupations) CSV files.
* **Data Tools:** Use `gs_data_manager.py` to automatically fix and validate database column counts.
* **Testing:** Use `gs_integrity_check.py` to verify binary stability across all 11 periods.
* **JSON Engine:** Powered by `cJSON` for complex social class and occupation logic.
* **Architecture:** Optimized C99 code with 512-byte safety buffers for string formatting.
* **License:** Core engine under GPL v3.0 (Includes cJSON under MIT License)..

