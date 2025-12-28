import os

# Updated data path to reflect the 'fi-FI' localization directory
# This script ensures every row in your CSV matches the 11-column technical spec
data_path = os.path.normpath(os.path.join(os.path.dirname(__file__), "..", "bin", "Debug", "data", "fi-FI"))
filename = os.path.join(data_path, "ammatit_lapset_poika.csv")

if not os.path.exists(filename):
    print(f"CRITICAL ERROR: Data file not found at: {filename}")
    exit()

print(f"Opening data source: {filename}")

with open(filename, 'r', encoding='utf-8') as f:
    lines = f.readlines()

new_lines = []
TARGET_COLUMNS = 11

for i, line in enumerate(lines):
    clean_line = line.strip()
    if not clean_line:
        continue # Skip empty lines to prevent parser errors in C
    
    # Split by semicolon delimiter
    parts = clean_line.split(';')
    
    # Adjust column count to reach exactly 11 columns
    if len(parts) < TARGET_COLUMNS:
        while len(parts) < TARGET_COLUMNS:
            parts.append("") # Pad with empty values
    elif len(parts) > TARGET_COLUMNS:
        parts = parts[:TARGET_COLUMNS] # Truncate excess columns
    
    # Trim whitespace from individual fields to ensure clean string matching in C
    parts = [p.strip() for p in parts]
    
    # Join parts back together and ensure Unix line endings
    new_lines.append(";".join(parts) + "\n")

# Write the sanitized data back to the file
with open(filename, 'w', encoding='utf-8', newline='\n') as f:
    f.writelines(new_lines)

print(f"Successfully processed {len(new_lines)} lines.")
print(f"Format Status: Standardized to {TARGET_COLUMNS} columns (10 semicolons per row).")