import os
import glob

def validate_genesim_full(base_path):
    print(f"--- GeneSim v1.3.1 Data Integrity Check ---")
    
    # Määritykset JSON-speksin mukaan
    NAME_COLS = 7       # max_game_year: 7
    SURNAME_COLS = 11   # max_game_year: 11
    JOB_COLS = 11       # max_game_year: 11

    def check_file(filepath, expected_cols):
        filename = os.path.basename(filepath)
        errors = 0
        
        with open(filepath, 'r', encoding='utf-8') as f:
            lines = f.readlines()
            for i, line in enumerate(lines):
                line = line.strip()
                # 1. Hyppää tyhjät ja kommentit
                if not line or line.startswith("#"): continue
                
                # 2. Hyppää otsikkorivi (tunnistetaan jos se alkaa vuosiluvulla ilman pilkkua)
                if i == 0 and line.startswith("1860") and "," not in line:
                    continue

                parts = line.split(';')
                
                # Tarkistetaan sarakemäärä
                if len(parts) != expected_cols:
                    print(f"[VIRHE]    {filename:35} Rivi {i+1:4}: Sarakkeita {len(parts)}/{expected_cols}")
                    errors += 1
                
                # Tarkistetaan Nimi,Paino formaatti (vain datariveille)
                elif "," not in parts[0] and parts[0] != "-":
                     # Sallitaan jos ensimmäinen osa on pelkkä nimi, mutta tarkistetaan onko se virhe
                     pass

        if errors == 0:
            print(f"[OK]       {filename:35} ({len(lines)} riviä)")

    # Tiedostoryhmät
    print("\n--- ETUNIMET JA KESKINIMET (7 saraketta) ---")
    for f in glob.glob(os.path.join(base_path, "etunimet_*.csv")) + glob.glob(os.path.join(base_path, "keskinimet_*.csv")):
        check_file(f, NAME_COLS)

    print("\n--- SUKUNIMET (11 saraketta) ---")
    for f in glob.glob(os.path.join(base_path, "sukunimet_*.csv")):
        check_file(f, SURNAME_COLS)

    print("\n--- AMMATIT (11 saraketta) ---")
    for f in glob.glob(os.path.join(base_path, "ammatit_*.csv")):
        check_file(f, JOB_COLS)

# Polun tunnistus (test-kansiosta käsin)
script_dir = os.path.dirname(os.path.abspath(__file__))
data_path = os.path.normpath(os.path.join(script_dir, "..", "bin", "Debug", "data", "FI"))

if __name__ == "__main__":
    validate_genesim_full(data_path)