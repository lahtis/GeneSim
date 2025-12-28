# Changelog: GeneSim NameGen

All notable changes to this project will be documented in this file. This project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [2.0.0] - 2025-12-28 (NewYear-Edition)

### Added
- **Multi-Persona Modes:** Introduced `--couple` and `--family` generation logic.
- **Social Logic Engine:** Integration of `cJSON` to handle occupation and social class weighting via `social_logic.json`.
- **Deterministic Seeding:** Added `--seed` support for 100% reproducible generation results.
- **Extended Metadata:** Support for 11-column surname database and 7-column name database.
- **Historical Weighting:** Added `--weights` (default) and `--no-weights` toggles for frequency-based name selection.
- **Documentation Command:** New `--list-periods` (`-lp`) flag to view era guidelines in Finnish or English.
- **Enhanced Logging:** Toggleable logging system with `--log [on|off]` to track seeds and outputs.
- **Temporal Logic:** Added `--year` and `--age` flags for dynamic birth year calculation based on simulation time.
- **Maiden Name Logic:** Automatic "née" (o.s.) prefixing for married female personas.

### Changed
- **Database Schema:** Upgraded surname CSV from 7 to 11 columns to accommodate period and regional metadata.
- **Memory Management:** Hardened string buffers to 512 bytes to prevent overflows in complex family strings.
- **Default Behavior:** Historical weighting is now active by default to improve realism.
- **Period Range:** Expanded historical periods from 7 to 11 distinct eras.
- **Project Structure:** Migrated to a modular `src/` directory with dedicated modules for `logger`, `output`, and `utils`.

### Fixed
- **Age Logic:** Corrected the formula where birth years were incorrectly offset in family mode.
- **Gender Consistency:** Fixed a bug where patronymic suffixes did not always match the father's name gender.
- **CSV Parsing:** Improved robustness when encountering empty lines or trailing semicolons in data files.

---

## [1.3.1] - 2025-12-23

### Added
- Smart middle name generation logic.
- Internationalized documentation (English support).
- Basic command-line parser for period and count.

### Shelved (Moved to Backlog)
- Job titles (later implemented in v2.0.0).
- Social class distinctions (later implemented in v2.0.0).
- Custom output formatting strings.

---

## [1.2.0] - 2025-12-20

### Added
- Initial support for historical period filtering.
- Basic CSV loading for names and surnames.
- Basic gender-based filtering.

---

*For more information on the current 2.0.0 release, please refer to the `TECHNICAL_SPECS.md`.*