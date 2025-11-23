# namegen

*namegen on komentorivipohjainen nimi­generaattori, joka tukee useita tulostusmuotoja ja eri generointitiloja.

namegen/ ├── src/ │ ├── main.c │ ├── args.c │ ├── generator.c │ ├── output.c ├── include/ │ ├── args.h │ ├── generator.h │ ├── output.h ├── namegen.cbp

** Build **

Projektin voi avata ja kääntää Code::Blocksissa:

- Debug build: `bin/Debug/namegen`
- Release build: `bin/Release/namegen`

** Käyttö **

### Vaihtoehdot

- `--help`
  Näyttää ohjeen

- `--version`
  Näyttää versionumeron

- `--count N`
  Generoi N kappaletta nimiä

- `--period N`
  Valitsee aikakauden (1–7, 0=random)

- `--couple`
  Generoi parin

- `--family`
  Generoi perheen

- `--output MODE`
  Tulostusmuoto: `plain`, `csv`, `json`

### Esimerkkejä

./namegen --count 3 --output plain
./namegen --family --output csv
./namegen --couple --period 2 --output json

## 🛠️ Kehitys

- Lisää uusia nimi­listoja ja aikakausia `generator.c`-tiedostoon
- Laajenna `Args`-rakennetta uusilla lipuilla
- Toteuta uusia tulostusmuotoja `output.c`-tiedostossa

### Version

`--version` näyttää ohjelman versionumeron sekä build‑päivämäärän ja 

esimerkki
namegen --version

tulostus
namegen version 0.4.18 (built Nov 17 2025 19:56:00)

- **0.4.18** = MAJOR.MINOR.PATCH
- **Nov 17 2025 19:56:00** = build‑päivämäärä ja kellonaika, jotka tulevat automaattisesti käännöksen yhteydessä (`__DATE__` ja `__TIME__` makroista).

