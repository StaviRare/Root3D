import os
import sys
import struct
import config_retriever

def package_resources(resource_folder, output_file):
    if not os.path.exists(resource_folder):
        print(f"Error: Resource folder not found at {resource_folder}")
        sys.exit(1)

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

def main():
    output_dir = config_retriever.get_config_arg('build_settings.json', 'output_dir')
    resource_folder = config_retriever.get_config_arg('dev_environment.json', 'RESOURCE_FOLDER')
    output_file = os.path.join(output_dir, 'resources.bin')
    package_resources(resource_folder, output_file)

# Main Program
main()
