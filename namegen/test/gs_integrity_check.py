import os
import subprocess
import json
import csv
import io
import sys

def run_namegen(args):
    """Suorittaa namegen.exe:n ja varmistaa polunhallinnan."""
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.normpath(os.path.join(script_dir, ".."))
    
    possible_paths = [
        os.path.join(project_root, "bin", "Debug", "namegen.exe"),
        os.path.join(project_root, "bin", "Release", "namegen.exe"),
        os.path.join(project_root, "bin", "namegen.exe"),
        os.path.join(script_dir, "namegen.exe")
    ]
    
    exe_path = next((p for p in possible_paths if os.path.exists(p)), None)
    if not exe_path:
        return None, "Executable not found"

    try:
        cmd = [exe_path] + args
        result = subprocess.run(
            cmd, 
            capture_output=True, 
            text=True, 
            encoding="utf-8", 
            cwd=os.path.dirname(exe_path),
            timeout=5
        )
        return result.stdout, result.returncode
    except Exception as e:
        return None, str(e)

def validate_json(output):
    try:
        lines = output.strip().split('\n')
        for line in lines:
            if line.strip():
                json.loads(line)
        return True
    except:
        return False

def validate_csv(output, expected_cols=5):
    try:
        f = io.StringIO(output.strip())
        reader = csv.reader(f, delimiter=';')
        rows = list(reader)
        return len(rows) > 0 and len(rows[0]) >= expected_cols
    except:
        return False

if __name__ == "__main__":
    print("=" * 65)
    print("GeneSim NameGen v2.0.0 - MASTER INTEGRITY SUITE")
    print("=" * 65)

    # 1. TESTATAAN DOKUMENTAATIO (--list-periods)
    print("\n[PHASE 1] Testing Documentation (Help Strings)...")
    for lang in ["fi", "en"]:
        out, code = run_namegen(["--list-periods", lang])
        if code == 0 and len(out.strip()) > 100:
            print(f"  [OK] --list-periods {lang}: Received {len(out)} chars of documentation.")
        else:
            print(f"  [FAIL] --list-periods {lang}: Documentation missing or too short.")

    # 2. TESTATAAN DETERMINISMI (Siemen 1233)
    print("\n[PHASE 2] Testing RNG Determinism...")
    out1, _ = run_namegen(["--family", "--count", "1", "--seed", "1233"])
    out2, _ = run_namegen(["--family", "--count", "1", "--seed", "1233"])
    if out1 and out1 == out2:
        print("  [OK] Seed consistency verified. Identical outputs confirmed.")
    else:
        print("  [FAIL] Non-deterministic output! RNG is not following the seed.")

    # 3. TESTATAAN KAIKKI AIKAKAUDET (Periods 1-11)
    print("\n[PHASE 3] Testing All Historical Periods (1-11)...")
    for p in range(1, 12):
        print(f"  Period {p:02d}: ", end="", flush=True)
        
        # Testataan perus-teksti
        txt_out, code = run_namegen(["--period", str(p), "--count", "1"])
        t_status = "OK" if (code == 0 and txt_out) else "FAIL"

        # Testataan JSON
        js_out, _ = run_namegen(["--period", str(p), "--family", "--output", "json"])
        j_status = "OK" if (js_out and validate_json(js_out)) else "FAIL"

        # Testataan CSV
        csv_out, _ = run_namegen(["--period", str(p), "--count", "2", "--output", "csv"])
        c_status = "OK" if (csv_out and validate_csv(csv_out)) else "FAIL"

        print(f"TEXT: {t_status} | JSON: {j_status} | CSV: {c_status}")

    print("\n" + "=" * 65)
    print("TESTING COMPLETE. Final report generated.")
    print("=" * 65)