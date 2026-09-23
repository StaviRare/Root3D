import sys
import shutil
import os

# Project-specific imports
sys.path.append('../../utilities/')
from logger import print_regular, print_error, print_success

# Global path definitions
PROJECT_GEN_PATH = "resources/project_gen"
SOURCE_CMAKE = f"{PROJECT_GEN_PATH}/CMakeLists.txt"
PROJECT_DIR = "../../../workspaces/linux/"
TARGET_CMAKE = f"{PROJECT_DIR}/CMakeLists.txt"

# Main Program
print_regular("** Starting Linux project generation...")

try:
    os.makedirs(PROJECT_DIR, exist_ok=True)
    shutil.copy2(SOURCE_CMAKE, TARGET_CMAKE)

    print_success("** Linux CMake project has been created successfully.")
except Exception as error:
    print_error("** An error occurred while creating the Linux project:")
    print_error(str(error))
