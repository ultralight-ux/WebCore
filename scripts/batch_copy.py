# batch_copy.py
import shutil
import sys

def batch_copy(paths_file):
    num_files_copied = 0
    with open(paths_file, 'r') as file:
        for line in file:
            source, dest = line.strip().split(';')
            shutil.copy2(source, dest)
            num_files_copied += 1
            
    print(f"Copied {num_files_copied} files.")

if __name__ == "__main__":
    # The first argument is now the path to the intermediate file
    paths_file = sys.argv[1]
    batch_copy(paths_file)
