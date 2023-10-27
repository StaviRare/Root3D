import json
import subprocess
import sys
import os

def load_config(script_dir):
    config_path = os.path.join(script_dir, 'env_config.json')
    if not os.path.exists(config_path):
        print("Configuration file not found.")
        sys.exit(1)
    with open(config_path, 'r') as f:
        return json.load(f)

def build_project(solution_path, configuration, platform, output_path, env_vars):
    msbuild_path = env_vars.get('MSBUILD_PATH')
    if not msbuild_path or not os.path.exists(msbuild_path):
        print("MSBuild.exe not found. Please check your configuration file and ensure Visual Studio is installed.")
        sys.exit(1)

    platform = "x86" if platform == "32" else "x64"
    
    build_command = [
        msbuild_path,
        solution_path,
        "/p:Configuration=" + configuration,
        "/p:Platform=" + platform,
        "/p:OutDir=" + output_path + "//output\\",
        "/p:IntDir=" + output_path + "//intermediate\\",
        "/t:Rebuild"
    ]
    
    process = subprocess.run(build_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    
    if process.returncode == 0:
        print("Build succeeded.")
    else:
        print("Build failed.")
        print("Output:\n", process.stdout)
        print("Errors:\n", process.stderr)

def main():
    if __name__ == "__main__":
        if len(sys.argv) != 4:
            print("Usage: python build_vs_project.py <configuration> <platform> <build_base_path>")
            sys.exit(1)

        script_dir = os.path.dirname(os.path.realpath(__file__))
        _, configuration, platform, build_base_path = sys.argv
        env_vars = load_config(script_dir)
        solution_path = env_vars.get('SOLUTION_PATH')
        
        if not solution_path:
            print("Solution path not found in configuration file.")
            sys.exit(1)
        
        build_project(solution_path, configuration, platform, build_base_path, env_vars)

# Main Program
main()