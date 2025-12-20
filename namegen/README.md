# namegen 
namegen is a command-line based name generator that supports multiple output formats and different generation modes.

## Options

- `--count <number>`
  Generate how many names to generated

- `--period <number>`
  Select the era (1–7, 0=random)

- `--couple`
  Generate a couple

- `--family`
  Generate a family

- `--output-file <filename>`
  Prints to the desired file.
  Print format in use: `text`, `csv` or `json`

- `--help`
  Show help

- `--version` 
  Shows the program's version number and build date

- `--verbose` 
  Shows debug information

### examples

Example randomly selects names from the desired season (--pediod) and prints them on the screen in txt-format, and finally saves the file as test.txt

`namegen.exe --pediod 1 -format text --output-file test.txt`

The program uses the default of 1 for selecting names. You can change it by providing the flag --count <number>, or you can change it in the settings.

The example randomly selects names from the desired 2 season (--pediod) and prints them on the screen in CSV-format, as well as saves the file under the name test.csv

 `namegen.exe --pediod 2 -format csv --output-file test.csv`

The example selects season 3 and randomly generates 3 names, printing them on the screen in JSON format, and also saves them to a file named test.json

 `namegen.exe --pediod 3 --count 3 -format json --output-file test.json`


## File structure

namegen/ ├
── src/ │ 
├── main.c │ 
├── args.c │ 
├── generator.c │ 
├── output.c |
── include/ │
├── args.h │
├── generator.h │
├── output.h | 
├── namegen.cbp

## Build 

The project can be opened and compiled in Code::Blocks:

- Debug build: `bin/Debug/namegen`
- Release build: `bin/Release/namegen`


## Development

- Add new name lists and eras to the `generator.c` file
- Extend the `Args` structure with new flags
- Implement new output formats in the `output.c` file

## Done
- **0.4.18** = MAJOR.MINOR.PATCH
- **Nov 17 2025 19:56:00** build date and time, which are automatically added during the translation (`__DATE__` ja `__TIME__` macros).

