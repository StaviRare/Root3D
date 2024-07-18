import os
import sys
import struct

# Project-specific imports
sys.path.append('../utilities/')
from logger import print_regular, print_success, print_error

# Global path definitions
PROJECT_ROOT_PATH = os.path.join("..", "..")
RESOURCE_DIR_PATH = os.path.join(PROJECT_ROOT_PATH, 'assets', 'raw')
PACKED_ASSETS_DIR_PATH = os.path.join(PROJECT_ROOT_PATH, 'assets', 'packed')
PACKED_ASSETS_FILE_PATH = os.path.join(PACKED_ASSETS_DIR_PATH, 'resources.bin')

# Main Program
if not os.path.exists(RESOURCE_DIR_PATH):
    print_error(f"Resource directory not found at {RESOURCE_DIR_PATH}")
else:
    if not os.path.exists(PACKED_ASSETS_DIR_PATH):
        os.makedirs(PACKED_ASSETS_DIR_PATH)

    with open(PACKED_ASSETS_FILE_PATH, 'wb') as output_file:
        for root, _, files in os.walk(RESOURCE_DIR_PATH):
            for file_name in files:
                file_path = os.path.join(root, file_name)
                relative_file_path = os.path.relpath(file_path, RESOURCE_DIR_PATH)
                with open(file_path, 'rb') as resource_file:
                    resource_data = resource_file.read()
                    # Write file metadata and data to the packed assets file
                    output_file.write(struct.pack('<I', len(relative_file_path)))
                    output_file.write(relative_file_path.encode())
                    output_file.write(struct.pack('<I', len(resource_data)))
                    output_file.write(resource_data)

    print_success("Resources packaged successfully. Output location:")
    print_regular(os.path.abspath(PACKED_ASSETS_FILE_PATH))
