import subprocess
import os

def run_test(command, label):
    try:
        # Käytetään encoding='utf-8' varmistamaan ääkköset
        result = subprocess.run(command, shell=True, capture_output=True, text=True, encoding='utf-8')
        if result.returncode == 0:
            print(f"    [{label}] OK")
            # Tulostetaan vain ensimmäinen rivi esimerkkinä, jos tuloste on pitkä
            lines = result.stdout.strip().split('\n')
            if lines:
                print(f"      Sample: {lines[0]}")
            return result.stdout
        else:
            print(f"    [ERROR] Command failed: {command}")
            return None
    except Exception as e:
        print(f"    [CRITICAL] Error: {e}")
        return None

if __name__ == "__main__":
    print("Starting NameGen v1.3.0 Full Integration Test (Formats, Genders, Families)...\n")
    
    executable = "namegen.exe"
    
    if not os.path.exists(executable):
        print(f"Critical Error: {executable} not found!")
    else:
        for p in range(1, 8):
            print(f"\n--- TESTING PERIOD {p} ---")
            
            # 1. Perusrakenteet (Text)
            run_test(f"{executable} --period {p} --count 1 --male", "MALE (TEXT)")
            run_test(f"{executable} --period {p} --couple", "COUPLE (TEXT)")
            run_test(f"{executable} --period {p} --family", "FAMILY (TEXT)")
            
            # 2. JSON-formaatti
            # Testataan perheen generointi JSON-muodossa
            json_out = run_test(f"{executable} --period {p} --family --output json", "FAMILY (JSON)")
            if json_out and json_out.strip().startswith("[") and json_out.strip().endswith("]"):
                print("      Validation: JSON structure looks valid (Array found).")

            # 3. CSV-formaatti
            # Testataan yksittäisten nimien generointi CSV-muodossa
            csv_out = run_test(f"{executable} --period {p} --count 3 --output csv", "BATCH (CSV)")
            if csv_out and "," in csv_out:
                print("      Validation: CSV structure looks valid (Commas found).")
            
    print("\nTesting completed.")