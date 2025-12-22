import subprocess
import json
import csv
import io

def run_namegen(args):
    """Executes namegen.exe with the provided arguments."""
    # Ensure namegen.exe is in the same folder or provide full path
    cmd = ["./namegen.exe"] + args
    result = subprocess.run(cmd, capture_output=True, text=True, encoding="utf-8")
    return result.stdout

def test_json_validity():
    print("Testing JSON output validity...")
    output = run_namegen(["--family", "--count", "1", "--output", "json", "--age", "--seed", "1233"])
    
    try:
        lines = output.strip().split('\n')
        for line in lines:
            if line.strip():
                data = json.loads(line)
                print(f"  [OK] Valid JSON: {data['first']} {data['last']} (b. {data['birth']})")
    except Exception as e:
        print(f"  [ERROR] JSON parsing failed: {e}")
        print(f"  Raw output: {output}")

def test_csv_structure():
    print("\nTesting CSV structure...")
    output = run_namegen(["--family", "--count", "1", "--output", "csv", "--age", "--seed", "1233"])
    
    f = io.StringIO(output)
    reader = csv.reader(f)
    rows = list(reader)
    
    if len(rows) > 0:
        print(f"  [OK] CSV generated {len(rows)} rows.")
        # Checks for 5 columns: First, Middle, Last, Maiden, Birth
        if len(rows[0]) == 5:
            print("  [OK] Column count matches (5 columns).")
        else:
            print(f"  [ERROR] Unexpected column count: {len(rows[0])}")
    else:
        print("  [ERROR] CSV output is empty.")

def test_seed_determinism():
    print("\nTesting Seed determinism (Seed: 1233)...")
    out1 = run_namegen(["--family", "--count", "1", "--output", "text", "--age", "--seed", "1233"])
    out2 = run_namegen(["--family", "--count", "1", "--output", "text", "--age", "--seed", "1233"])
    
    if out1 == out2:
        print("  [OK] Outputs are identical. Determinism verified!")
    else:
        print("  [ERROR] Non-deterministic output detected for identical seeds.")

if __name__ == "__main__":
    test_json_validity()
    test_csv_structure()
    test_seed_determinism()
