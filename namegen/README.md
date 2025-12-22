# NameGen v1.3.0 (Xmas-Edition)

NameGen is a powerful, command-line based historical name generator developed as part of the **GeneSim** project. It simulates Finnish naming conventions across seven different historical periods (1850–1920) and supports complex family structures.

## Features

- **Historical Accuracy:** Seven distinct eras (Periods 1–7) reflecting the evolution of Finnish names.
- **Family Logic:** Generate individuals, couples, or full families with realistic birth years and naming patterns.
- **Multilingual:** Built-in documentation in Finnish and English.
- **Flexible Output:** Supports Plain text, CSV, and JSON formats.

## Options

### Generation Modes
- `--single` : Generate single names (default).
- `--couple` : Generate a couple (Male & Female).
- `--family` : Generate a full family (Parents & Children).

### Filters & Logic
- `--period <1-7>` : Select the era (1=1850, 7=1920, 0=random).
- `--count <number>` : Number of units to generate.
- `--male` / `--female` : Force specific gender for single names.
- `--maiden-name` : Show maiden names for wives (e.g., *nee Laitinen*).
- `--shared-surname` : Force couples/families to share a surname.
- `--middle-chance <0-100>` : Probability for a middle name (%).
- `--age, -A` : Show birth years based on the selected period.

### Technical & Output
- `--output <plain|csv|json>` : Set output format.
- `--seed <number>` : Set random seed for reproducible results.
- `--stdout` : Force output to console.
- `--lp <fi|en>` : Show guidelines/help in the selected language.
- `--verbose` : Show detailed loading and generation info.
- `--version` : Show version, author, and GitHub info.
- `--help` : Show help.

## Examples

**1. Generate 10 male names from Period 1 (1850) and save to CSV:** `namegen.exe --period 1 --count 10 --male --output csv > names.csv`

**2. Generate a realistic family from Period 4 (1885) with ages:** `namegen.exe --period 4 --family --age`

**3. Generate 3 JSON-formatted couples from Period 3:** `namegen.exe --period 3 --count 3 --couple --output json`

**4. Show Finnish instructions:** `namegen.exe -lp fi`

## Build & Installation

The project is designed to be compiled with **GCC** or opened in **Code::Blocks**:

1. Open `namegen.cbp` in Code::Blocks.
2. Select `Release` build target.
3. Ensure the `data/` folder are in the same directory as the executable.

- **Debug build:** `bin/Debug/namegen.exe`
- **Release build:** `bin/Release/namegen.exe`

## Development & License

- **Author:** Tuomas Lähteenmäki 
- **Lisence:** GPLv3

## Development

- Add new name lists and eras to the `generator.c` file
- Extend the `Args` structure with new flags
- Implement new output formats in the `output.c` file
