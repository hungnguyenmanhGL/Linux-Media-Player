#!/usr/bin/env python3
import subprocess
import os
import sys
import re
import shutil

EXECUTABLE_PATH = "./LinuxMediaPlayer"  
BUNDLE_DEST_DIR = "./libs"   

def get_direct_dependencies(file_path):
    # text=True decodes output as strings.
    # check=False prevents an exception if ldd returns non-zero (e.g., for warnings).
    ldd_result = subprocess.run(
        ['ldd', file_path],
        capture_output=True,
        text=True,
        check=False
    )

    # Regex pattern to find lines like "libfoo.so => /path/to/libfoo.so (0x...)"
    # \s+ matches one or more whitespace characters
    # (.+?) captures any characters non-greedily (the library path)
    # \s+\(0x[0-9a-fA-F]+\) matches the memory address part (e.g., (0x123abc))
    # \$ indicates end of string (to exclude "not found" messages)
    pattern = re.compile(r'^\s*[^=]+\s*=>\s*(.+?)\s+\(0x[0-9a-fA-F]+\)$')

    dependencies = []
    for line in ldd_result.stdout.splitlines(): # Split output into lines
        match = pattern.match(line)
        if match:
            lib_path = match.group(1).strip() # Extract the captured path
            if os.path.isfile(lib_path):
                dependencies.append(lib_path)
    return dependencies

print(f"Colllecting dependencies of '{EXECUTABLE_PATH}'...")
direct_deps_list = get_direct_dependencies(EXECUTABLE_PATH)

# Print each direct dependency
# if direct_deps_list:
#     for dep in direct_deps_list:
#         print(f"{dep}")
# else:
#     print("  (No dependencies found or all are filtered system libs)")

CORE_SYSTEM_LIBS = {
    'libc.so.6',
    'libm.so.6',
    'libgcc_s.so.1',
    'libdl.so.2',
    'libpthread.so.0',
    'ld-linux-x86-64.so.2', # The dynamic linker itself
}

all_dependencies = set()
processed_files = set()
files_to_process = [EXECUTABLE_PATH]

while files_to_process: 
    current_file = files_to_process.pop(0)
    if current_file in processed_files:
        continue

    processed_files.add(current_file)
    direct_deps_for_current_file = get_direct_dependencies(current_file)

    for dep_path in direct_deps_for_current_file:
        lib_filename = os.path.basename(dep_path)
        if lib_filename in CORE_SYSTEM_LIBS:
            continue

        if dep_path not in all_dependencies:
            all_dependencies.add(dep_path)      
            files_to_process.append(dep_path) 

# for dep in sorted(list(all_dependencies)):
#     print(dep)
# print(f"Total {len(all_dependencies)} files.")

copied_count = 0
patched_count = 0
try:
    os.makedirs(BUNDLE_DEST_DIR, exist_ok=True) # Create destination directory if doesn't exist
    print(f"Checking destination directory '{BUNDLE_DEST_DIR}' exists. Creating if not...")
except OSError as e:
    print(f"Error creating destination directory '{BUNDLE_DEST_DIR}': {e}", file=sys.stderr)
    sys.exit(1)

# copy all files to destination folder
for dep_path in sorted(list(all_dependencies)):
    try:
        lib_filename = os.path.basename(dep_path) # Get filename
        dest_path = os.path.join(BUNDLE_DEST_DIR, lib_filename) # Get full destination path

        shutil.copy2(dep_path, dest_path) # Copy the file, preserving metadata
        print(f"  Copied: {lib_filename}")
        copied_count += 1

    except FileNotFoundError:
        print(f"Warning: Source library not found during copy: {dep_path}. Skipping.", file=sys.stderr)
    except PermissionError:
        print(f"Error: Permission denied when copying {dep_path} to {dest_path}. Check permissions or run script with sudo if necessary.", file=sys.stderr)
    except Exception as e:
        print(f"An unexpected error occurred while copying {dep_path}: {e}", file=sys.stderr)

    # BLOCK: use patchelf to add RPATH property for transitive dependencies 
    patchelf_cmd = ['patchelf', '--set-rpath', '$ORIGIN', dest_path]
    patchelf_result = subprocess.run(
        patchelf_cmd,
        capture_output=True, 
        text=True,
        check=False          # Do not raise exception on non-zero exit code (patchelf might warn)
    )
    if patchelf_result.returncode == 0:
        print(f"  Patched RPATH for: {lib_filename}")
        patched_count += 1
    elif "not an ELF executable" in patchelf_result.stderr:
        print(f"  Skipping patch: {lib_filename} is not an ELF shared object. Stderr: {patchelf_result.stderr.strip()}", file=sys.stderr)
    else:
        print(f"  Warning: Failed to patch RPATH for {lib_filename}. "
                f"Stderr: {patchelf_result.stderr.strip()}", file=sys.stderr)
    # END OF BLOCK

print(f"Successfully copied {copied_count} dependencies.")   