import os
import sys

# Project-specific imports
sys.path.append('../utilities/')
from dependencies import install_dependencies
from logger import print_success, print_error

# Main Program
if install_dependencies():
    print_success("All dependencies installed successfully.")
else:
    print_error("Failed to install some dependencies.")
