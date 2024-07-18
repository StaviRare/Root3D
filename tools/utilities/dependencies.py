import os
import sys
import subprocess
import importlib.util

# Project-specific imports
from logger import print_regular, print_warning, print_error, print_success

dependencies = [
    "gdown",
    "colorama",
]

def install_package(package_name):
    print_regular(f"** Installing {package_name}...")
    command = [sys.executable, "-m", "pip", "install", package_name, "-q", "--disable-pip-version-check"]
    result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, env=dict(os.environ, PIP_DISABLE_PIP_VERSION_CHECK="1"))
    if result.returncode != 0:
        print_error(f"** Failed to install {package_name}.")
        return False
    return True

def check_dependencies():
    all_installed = True
    for package in dependencies:
        package_spec = importlib.util.find_spec(package)
        if package_spec is None:
            # print_warning(f"** Dependency '{package}' is not installed.")
            all_installed = False
    if not all_installed:
        print_error("** Some dependencies are missing. Please run the environment setup.")

    return all_installed

def install_dependencies():
    all_installed = True
    for package in dependencies:
        package_spec = importlib.util.find_spec(package)
        if package_spec is None:
            if not install_package(package):
                all_installed = False
        else:
            print_regular(f"** Dependency '{package}' is already installed. Skipping installation.")
    return all_installed
