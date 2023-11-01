import os
import json
import sys

def get_config_arg(config_file, arg_name):
    script_dir = os.path.dirname(os.path.realpath(__file__))
    config_path = os.path.join(script_dir, '..', 'configs', config_file)
    config_path = os.path.abspath(config_path)  # Normalize the path
    
    if not os.path.exists(config_path):
        print("Configuration file not found:", config_path)
        sys.exit(1)
    
    with open(config_path, 'r') as f:
        config = json.load(f)
        
    if arg_name not in config:
        print(f"Argument {arg_name} not found in configuration file.")
        sys.exit(1)
    
    return config[arg_name]

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python config_retriever.py <config_file> <arg_name>")
        sys.exit(1)
    
    _, config_file, arg_name = sys.argv
    result = get_config_arg(config_file, arg_name)
    print(result)
