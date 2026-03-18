import os

def aggregate_files(output_file='code.txt'):
    exclude_dirs = {os.path.abspath(d) for d in ['build', 'extern/deps', 'logs','.conda','.config','.git','.vscode','venv','compute/target','compute/build','extern/vendor']}
    exclude_files = {'code.py', 'code.txt','.gitignore','.DS_Store','.md'}
    
    with open(output_file, 'w', encoding='utf-8') as outfile:
        for root, dirs, files in os.walk('.'):
            # Check if current root or any parent is in exclude_dirs
            abs_root = os.path.abspath(root)
            should_skip_dir = False
            for ex_dir in exclude_dirs:
                if abs_root == ex_dir or abs_root.startswith(ex_dir + os.sep):
                    should_skip_dir = True
                    break
            
            if should_skip_dir:
                dirs[:] = [] # Don't descend into this directory
                continue

            # Also filter dirs in-place to avoid descending into excluded names
            dirs[:] = [d for d in dirs if os.path.join(root, d) not in exclude_dirs]

            for file in files:
                if file in exclude_files or file.endswith(tuple(exclude_files)):
                    continue
                
                file_path = os.path.join(root, file)
                relative_path = os.path.relpath(file_path, '.')
                
                try:
                    with open(file_path, 'r', encoding='utf-8', errors='ignore') as infile:
                        content = infile.read()
                        outfile.write(f"{relative_path} ...\n")
                        outfile.write(content)
                        outfile.write("\n\n" + "="*40 + "\n\n") # Separator for clarity
                except Exception as e:
                    print(f"Skipping {relative_path}: {e}")

if __name__ == "__main__":
    aggregate_files()
    print("Done! Contents saved in code.txt")
