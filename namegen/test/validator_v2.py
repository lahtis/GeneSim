import os
import glob

def manage_genesim_data(base_path):
    print("=" * 60)
    print(f"GeneSim v2.0.0 Data Manager - Integrity & Auto-Fix")
    print(f"Target Path: {base_path}")
    print("=" * 60)
    
    # Technical Specifications for v2.0.0
    NAME_COLS = 7    # Columns for first and middle names
    SURNAME_COLS = 11 # Columns for surnames
    JOB_COLS = 11     # Columns for occupations
    
    def process_file(filepath, expected_cols, category):
        filename = os.path.basename(filepath)
        fixed_count = 0
        error_count = 0
        output_lines = []
        
        if not os.path.exists(filepath):
            return

        with open(filepath, 'r', encoding='utf-8') as f:
            lines = f.readlines()

        for i, line in enumerate(lines):
            clean_line = line.strip()
            
            # Skip empty lines or comments
            if not clean_line or clean_line.startswith("#"):
                output_lines.append(line)
                continue
            
            # Skip header (e.g. "1860" without comma)
            if i == 0 and clean_line.startswith("1860") and "," not in clean_line:
                output_lines.append(line)
                continue

            parts = clean_line.split(';')
            
            # --- FIXING LOGIC ---
            if len(parts) != expected_cols:
                if len(parts) < expected_cols:
                    while len(parts) < expected_cols:
                        parts.append("")
                    fixed_count += 1
                elif len(parts) > expected_cols:
                    parts = parts[:expected_cols]
                    fixed_count += 1
            
            # Clean internal whitespace
            parts = [p.strip() for p in parts]
            
            # --- VALIDATION LOGIC ---
            # Check if Name,Weight format exists in first column (if not a placeholder "-")
            if parts[0] != "-" and "," not in parts[0]:
                # We log this as a warning but don't auto-fix it
                error_count += 1
            
            output_lines.append(";".join(parts) + "\n")

        # Save fixed version
        with open(filepath, 'w', encoding='utf-8', newline='\n') as f:
            f.writelines(output_lines)

        # Reporting
        status = "[OK]" if (fixed_count == 0 and error_count == 0) else "[MODIFIED]"
        print(f"{status:12} {filename:30} | Rows: {len(output_lines):4} | Fixed: {fixed_count:3} | Issues: {error_count:3}")

    # Process by groups
    groups = [
        ("etunimet_*.csv", NAME_COLS, "FIRST NAMES"),
        ("keskinimet_*.csv", NAME_COLS, "MIDDLE NAMES"),
        ("sukunimet_*.csv", SURNAME_COLS, "SURNAMES"),
        ("ammatit_*.csv", JOB_COLS, "OCCUPATIONS")
    ]

    for pattern, cols, label in groups:
        print(f"\n--- {label} (Target: {cols} columns) ---")
        files = glob.glob(os.path.join(base_path, pattern))
        if not files:
            print(f"  No files found matching {pattern}")
        for f in files:
            process_file(f, cols, label)

if __name__ == "__main__":
    # Identify path based on the fi-FI localization structure
    script_dir = os.path.dirname(os.path.abspath(__file__))
    data_path = os.path.normpath(os.path.join(script_dir, "..", "bin", "Debug", "data", "fi-FI"))

    if not os.path.exists(data_path):
        print(f"CRITICAL ERROR: Path not found: {data_path}")
    else:
        manage_genesim_data(data_path)
        print("\n" + "=" * 60)
        print("DATA MANAGEMENT COMPLETE. All files standardized for v2.0.0.")
        print("=" * 60)