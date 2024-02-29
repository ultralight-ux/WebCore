import os
import shutil
from glob import glob
import platform
import subprocess

def run_command(command):
    """
    Executes a given command using the system shell, handling errors and specific return codes.
    Captures standard output and standard error to suppress unnecessary console output.

    Args:
        command (str): The command to execute.
    """
    try:
        # Capture output and error to suppress them in console.
        result = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, check=False)
        
        if 'robocopy' in command:
            # Robocopy considers return codes 0-3 as success.
            if result.returncode > 3:
                print(f"Error: {result.stderr}")
                raise subprocess.CalledProcessError(result.returncode, command, output=result.stdout, stderr=result.stderr)
            elif result.returncode <= 3 and result.stderr:
                # Optionally print stderr if there's a warning or message, but considered a success by robocopy
                print(f"Robocopy message: {result.stderr}")
        else:
            if result.returncode != 0:
                print(f"Error: {result.stderr}")
                raise subprocess.CalledProcessError(result.returncode, command, output=result.stdout, stderr=result.stderr)
    except subprocess.CalledProcessError as e:
        print(f"Error executing command: {e.cmd}")
        print(f"Return code: {e.returncode}")
        # Optionally print error output
        if e.stderr:
            print(f"Error output: {e.stderr}")

def copy_file(source, destination):
    """
    Copies a single file from source to destination, creating the destination directory if it does not exist.
    If destination is a directory, the file is copied into this directory with its original filename.
    If destination is a file path, the file is copied and renamed to this path. File metadata is also copied.

    Args:
        source (str): The path to the source file.
        destination (str): The path to the destination directory or file.
    """
    if not os.path.isfile(source):
        print(f"Warning: Source file '{source}' does not exist. Copy operation skipped.")
        return
    
    if os.path.isdir(destination):
        destination = os.path.join(destination, os.path.basename(source))

    os.makedirs(os.path.dirname(destination), exist_ok=True)

    shutil.copy2(source, destination)

def copy_files(source, destination, include_patterns=None, exclude_patterns=None):
    """
    Copies files from source to destination based on include and exclude patterns, utilizing native OS utilities
    (robocopy for Windows, rsync for Unix-like systems) for improved performance. This function assumes the operation
    is always directory-based.

    Args:
        source (str): The path to the source directory.
        destination (str): The path to the destination directory.
        include_patterns (list, optional): A list of patterns to include in the copy. Defaults to ['*'].
        exclude_patterns (list, optional): A list of patterns to exclude from the copy. Defaults to [].
    """
    if not os.path.exists(source):
        print(f"Warning: Source '{source}' does not exist. Copy operation skipped.")
        return

    os.makedirs(destination, exist_ok=True)
    include_patterns = include_patterns or ['*']
    exclude_patterns = exclude_patterns or []

    if platform.system() == 'Windows':
        # Use robocopy for Windows: optimized for large numbers of files and supports advanced filtering
        robocopy_options = '/S /NFL /NDL /NJH /NJS /nc /ns /np'
        include_args = ' '.join(include_patterns)
        exclude_dirs = []
        for pattern in exclude_patterns:
            matched_dirs = glob(os.path.join(source, pattern), recursive=True)
            exclude_dirs.extend(matched_dirs)
        exclude_args = ' '.join([f'/XD "{dir}"' for dir in exclude_dirs])
        robocopy_cmd = f'robocopy "{source}" "{destination}" {include_args} {exclude_args} {robocopy_options}'
        run_command(robocopy_cmd)
    else:
        # Use rsync for Unix-like systems: efficient for large datasets and supports incremental updates
        rsync_filter = ' '.join([f'--include="{pattern}"' for pattern in include_patterns] +
                                [f'--exclude="{pattern}"' for pattern in exclude_patterns])
        rsync_options = '-avm'
        source_path = f'"{source}/"'
        destination_path = f'"{destination}"'
        rsync_cmd = f'rsync {rsync_options} {rsync_filter} {source_path} {destination_path}'
        run_command(rsync_cmd)
