import subprocess
import os

def run_test(command, label):
    try:
        result = subprocess.run(command, shell=True, capture_output=True, text=True, encoding='utf-8')
        if result.returncode == 0:
            print(f"    [{label}]")
            lines = result.stdout.strip().split('\n')
            for line in lines:
                print(f"      - {line}")
        else:
            print(f"    [ERROR] Command failed: {command}")
    except Exception as e:
        print(f"    [CRITICAL] Error: {e}")

if __name__ == "__main__":
    print("Starting NameGen v1.3.0 Full System Test (Genders, Couples, Families)...\n")
    
    if not os.path.exists("namegen.exe"):
        print("Critical Error: namegen.exe not found!")
    else:
        print("--- Testing Periods 1-7 ---")
        for p in range(1, 8):
            print(f"\n[PERIOD {p}]")
            
            # Individual tests
            run_test(f"namegen.exe --period {p} --count 1 --male", "MALE")
            run_test(f"namegen.exe --period {p} --count 1 --female", "FEMALE")
            
            # Couple test (Expected: Male and Female with same surname)
            run_test(f"namegen.exe --period {p} --couple", "COUPLE")
            
            # Family test (Expected: Father, Mother, and children)
            run_test(f"namegen.exe --period {p} --family", "FAMILY")
            
    print("\nTesting completed.")