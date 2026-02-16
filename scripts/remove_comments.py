import re
import os
import sys 

def remove_c_cpp_comments(text):
    def replacer(match):
        s = match.group(0)
        if s.startswith('/'):
            return " "  
        else:
            return s 

    pattern = re.compile(
        r'//.*?$|/\*.*?\*/|\'(?:\\.|[^\\\'])*\'|"(?:\\.|[^\\"])*"',
        re.DOTALL | re.MULTILINE
    )
    return re.sub(pattern, replacer, text)

def remove_cmake_comments(text):
    lines = text.split('\n')
    new_lines = []
    for line in lines:
       
        def replacer(match):
            s = match.group(0)
            if s.startswith('#'):
                return ""
            return s

        pattern = re.compile(r'"(?:\\.|[^\\"])*"|#.*')
      
        pass 
    
    def replacer(match):
        s = match.group(0)
        if s.startswith('#'):
            return ""
        return s
        
    pattern = re.compile(r'"(?:\\.|[^\\"])*"|#.*')
    return re.sub(pattern, replacer, text)

def process_file(filepath):
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
            
        ext = os.path.splitext(filepath)[1].lower()
        filename = os.path.basename(filepath)
        
        new_content = None
        if ext in ['.c', '.cc', '.cpp', '.h', '.hh', '.hpp', '.cxx']:
            new_content = remove_c_cpp_comments(content)
        elif ext == '.cmake' or filename == 'CMakeLists.txt':
            new_content = remove_cmake_comments(content)
            
        if new_content is not None:
            
            lines = new_content.split('\n')
            cleaned_lines = []
            for line in lines:
                if line.strip():
                    cleaned_lines.append(line)
                elif not line.strip() and line == "": 
                     pass

            final_lines = [line.rstrip() for line in new_content.split('\n')]
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write('\n'.join(final_lines))
                
    except Exception as e:
        print(f"Error processing {filepath}: {e}")

def main():
    root_dir = os.getcwd()
    if len(sys.argv) > 1:
        root_dir = sys.argv[1]
        
    print(f"Scanning {root_dir}...")
    
    excludes = ['build', 'extern', '.git', 'assets']
    
    for dirpath, dirnames, filenames in os.walk(root_dir):
        # dirnames[:] = [d for d in dirnames if d not in excludes]
        dirnames = [d for d in dirnames if d not in excludes]

        for filename in filenames:
            filepath = os.path.join(dirpath, filename)
            process_file(filepath)

if __name__ == "__main__":
    main()
