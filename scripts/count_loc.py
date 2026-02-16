import os

def count_lines(filepath):
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            return sum(1 for _ in f)
    except Exception as e:
        print(f"Error reading {filepath}: {e}")
        return 0

def main():
    # Use the parent directory of 'scripts' as root
    root_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    print(f"Scanning project root: {root_dir}")

    ignore_dirs = {'.git', 'extern', 'build', '.cache'}
    include_extensions = {'.cc', '.c', '.h', '.hh', '.py', '.in', '.pyi'}
    
    total_lines = 0
    file_count = 0

    for dirpath, dirnames, filenames in os.walk(root_dir):
        # Filter out ignored directories in-place
        dirnames[:] = [d for d in dirnames if d not in ignore_dirs]
        
        for filename in filenames:
            ext = os.path.splitext(filename)[1]
            if ext in include_extensions:
                filepath = os.path.join(dirpath, filename)
                lines = count_lines(filepath)
                total_lines += lines
                file_count += 1
                # Optional: print individual file counts if needed
                # print(f"{filepath}: {lines}")

    print("-" * 30)
    print(f"Total files counted: {file_count}")
    print(f"Total lines of code: {total_lines}")
    print("-" * 30)

if __name__ == "__main__":
    main()
