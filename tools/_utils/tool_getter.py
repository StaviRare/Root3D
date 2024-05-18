import os
import zipfile
import importlib.util
import subprocess
import sys
import shutil

def ensure_gdown_installed():
    package_name = "gdown"
    package_spec = importlib.util.find_spec(package_name)
    
    if package_spec is None:
        print(f"** Package '{package_name}' not found. Installing...")
        command = [sys.executable, "-m", "pip", "install", package_name]
        subprocess.check_call(command)
        print(f"** Package '{package_name}' installed successfully.")

def download_file_from_google_drive(file_id, output_path):
    print("** Downloading file from Google Drive...")
    import gdown
    url = f"https://drive.google.com/uc?id={file_id}"
    gdown.download(url, output_path, quiet=False)
    print("** File downloaded successfully.")

def extract_and_remove_zip(zip_path):
    print("** Extracting contents...")
    extract_to = os.path.dirname(zip_path)
    with zipfile.ZipFile(zip_path, 'r') as zip_ref:
        zip_ref.extractall(extract_to)
    os.remove(zip_path)
    print("** Extraction complete.")

def remove_output_folder(output_path):
    if os.path.exists(output_path):
        print("** Removing existing resources folder...")
        shutil.rmtree(output_path)
        print("** Resources folder removed.")

def download_and_extract(file_id, output_path):
    temp_zip_path = os.path.join(output_path, "temp.zip")
    remove_output_folder(output_path)
    ensure_gdown_installed()
    os.makedirs(output_path, exist_ok=True)
    download_file_from_google_drive(file_id, temp_zip_path)
    extract_and_remove_zip(temp_zip_path)
