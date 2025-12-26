# namegen 
namegen is a command-line based name generator that supports multiple output formats and different generation modes.

## Options

- `--period <number>`
Select the era (1–7, 0=random)

- `--count <number>`
Generate how many names to generated

- `--seed <number>`
Set random seed for reproducible results

- `--single`
Generate single names (default)

- `--couple`
Generate a couple

- `--family`
Generate a family

- `--male`
Force male names

- `--female`
Force female names

- `--middle-chance <0-100>`
Probability for a middle name (%)

 - `--shared-surname`
Force couples to share a surname

 - `--maiden-name`
Show maiden names for wives (nee.)

 - `--force-manual`
Disable automatic period-based logic

- `--output <mode>`
 Output format: plain, csv or json

- `--stdout`
Force output to console even if file is set

 - `--age, -A`
 Show birth years for generated names

- `--help`
  Show help

 - `--verbose`
 Show detailed loading and generation info

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

The example selects season 3 (--pediod 3) and randomly generates 4 names, printing them on the screen in CSV format, and also saves them to a file named output

 `namegen.exe --pediod 3 --count 4 -format csv`

## Build 

The project can be opened and compiled in Code::Blocks:

- Debug build: `bin/Debug/namegen`
- Release build: `bin/Release/namegen`

## Development

- Add new name lists and eras to the `generator.c` file
- Extend the `Args` structure with new flags
- Implement new output formats in the `output.c` file