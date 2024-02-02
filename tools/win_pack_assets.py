import os
import struct

# Constants
CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
RESOURCE_FOLDER = os.path.join(CURRENT_DIR, '..', 'assets', 'raw')
OUTPUT_DIR = os.path.join(CURRENT_DIR, '..', 'assets', 'packed')
OUTPUT_FILE = os.path.join(OUTPUT_DIR, 'resources.bin')

def package_resources(resource_folder, output_file):
    if not os.path.exists(resource_folder):
        print(f"Error: Resource folder not found at {resource_folder}")
        return

    if not os.path.exists(os.path.dirname(output_file)):
        os.makedirs(os.path.dirname(output_file))

    with open(output_file, 'wb') as f:
        for root, _, files in os.walk(resource_folder):
            for file in files:
                file_path = os.path.join(root, file)
                relative_path = os.path.relpath(file_path, resource_folder)
                with open(file_path, 'rb') as resource_file:
                    resource_data = resource_file.read()
                    # Writing the length of the file path, the file path, the length of the resource data, and the resource data itself
                    f.write(struct.pack('<I', len(relative_path)))
                    f.write(relative_path.encode())
                    f.write(struct.pack('<I', len(resource_data)))
                    f.write(resource_data)
    
    print("Resources packaged successfully.")
    print(f"Output file: {output_file}")

# Main Program
package_resources(RESOURCE_FOLDER, OUTPUT_FILE)
