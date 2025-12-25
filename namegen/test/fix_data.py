import os

# Määritetään oikea polu
data_path = os.path.normpath(os.path.join(os.path.dirname(__file__), "..", "bin", "Debug", "data", "FI"))
filename = os.path.join(data_path, "ammatit_lapset_poika.csv")

if not os.path.exists(filename):
    print(f"VIRHE: Tiedostoa ei löydy polusta: {filename}")
    exit()

with open(filename, 'r', encoding='utf-8') as f:
    lines = f.readlines()

new_lines = []
for i, line in enumerate(lines):
    # Poistetaan rivinvaihdot ja tyhjät välit lopusta
    clean_line = line.strip()
    if not clean_line: continue
    
    parts = clean_line.split(';')
    
    # Pakotetaan sarakemäärä tasan 11:een (10 puolipistettä)
    if len(parts) < 11:
        while len(parts) < 11:
            parts.append("-,0")
    elif len(parts) > 11:
        parts = parts[:11]
    
    # Rakennetaan rivi uudelleen ja varmistetaan yksi rivinvaihto
    new_lines.append(";".join(parts) + "\n")

with open(filename, 'w', encoding='utf-8', newline='\n') as f:
    f.writelines(new_lines)

print(f"Valmis! {filename} on nyt korjattu.")
print(f"Kaikilla riveillä on nyt tasan 11 saraketta.")