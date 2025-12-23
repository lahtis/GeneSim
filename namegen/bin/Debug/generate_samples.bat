@echo off
title GeneSim NameGen v1.3.0 Samples
color 0B

echo ==================================================
echo      GENESIM NAME GENERATOR - ESIMERKKIAJO
echo ==================================================
echo.

echo [VAIHE 1: Yksittaiset nimet eri aikakausilta]
echo Periodi 1 (1850):
namegen.exe --period 1 --count 2 --male
namegen.exe --period 1 --count 2 --female
echo.
echo Periodi 4 (1885):
namegen.exe --period 4 --count 2 --male
namegen.exe --period 4 --count 2 --female
echo.
echo --------------------------------------------------
echo [VAIHE 2: Pariskunta (Periodi 3)]
namegen.exe --period 3 --couple
echo.
echo --------------------------------------------------
echo [VAIHE 3: Kokonainen perhe (Periodi 5)]
namegen.exe --period 5 --family --maiden-name --middle-chance 46 --age
echo.
echo ==================================================
echo Esimerkkiaiho valmis!
echo Kayta komentoa 'namegen.exe --help' nahdaksesi kaikki valinnat.
echo ==================================================
pause