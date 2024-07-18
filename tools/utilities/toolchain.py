import os
import sys
import shutil
import zipfile

# Project-specific imports
from dependencies import check_dependencies
from logger import print_regular, print_error, print_success

def download_file_from_google_drive(file_id, destination_path):
    import gdown
    print_regular("** Downloading latest toolchain:")
    url = f"https://drive.google.com/uc?id={file_id}"
    gdown.download(url, destination_path, quiet=False)
    if os.path.exists(destination_path):
        print_success("** Download successful.")
        return True
    else:
        print_error("** Download failed.")
        return False

def extract_and_remove_zip(zip_file_path):
    print_regular("** Extracting toolchain...")
    extraction_directory = os.path.dirname(zip_file_path)
    with zipfile.ZipFile(zip_file_path, 'r') as zip_ref:
        zip_ref.extractall(extraction_directory)
    os.remove(zip_file_path)
    print_success("** Extraction complete.")

def remove_existing_output_folder(output_directory_path):
    if os.path.exists(output_directory_path):
        print_regular("** Existing toolchain detected. Removing...")
        shutil.rmtree(output_directory_path)

def download_and_extract_toolchain(file_id, output_directory):
    temp_zip_file_path = os.path.join(output_directory, "temp.zip")
    remove_existing_output_folder(output_directory)
    os.makedirs(output_directory, exist_ok=True)
    if download_file_from_google_drive(file_id, temp_zip_file_path):
        extract_and_remove_zip(temp_zip_file_path)
    else:
        print_error("** Download and extraction process aborted.")

# Main Program
if not check_dependencies():
    print_error("** Dependencies check failed. Exiting.")
    sys.exit(1)
