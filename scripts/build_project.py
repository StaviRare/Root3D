import subprocess
import sys
import os
import config_retriever

def build_project(solution_path, configuration, platform, output_path, msbuild_path):
    platform = "x86" if platform == "32" else "x64"
    
    build_command = [
        msbuild_path,
        solution_path,
        "/p:Configuration=" + configuration,
        "/p:Platform=" + platform,
        "/p:OutDir=" + output_path,
        "/p:IntDir=" + output_path + "//temp_build_files\\",
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
    env_config_file = 'dev_environment.json'
    msbuild_path = config_retriever.get_config_arg(env_config_file, 'MSBUILD_PATH')
    solution_path = config_retriever.get_config_arg(env_config_file, 'SOLUTION_PATH')
    
    build_config_file = 'build_settings.json'
    configuration = config_retriever.get_config_arg(build_config_file, 'configuration')
    platform = config_retriever.get_config_arg(build_config_file, 'platform')
    output_dir = config_retriever.get_config_arg(build_config_file, 'output_dir')
    
    if not msbuild_path or not os.path.exists(msbuild_path):
        print("MSBuild.exe not found. Please check your configuration file and ensure Visual Studio is installed.")
        sys.exit(1)
    
    if not solution_path:
        print("Solution path not found in configuration file.")
        sys.exit(1)
    
    build_project(solution_path, configuration, platform, output_dir, msbuild_path)

# Main Program
main()
