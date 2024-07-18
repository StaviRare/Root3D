import os
import sys
import argparse
import subprocess

# Project-specific imports
sys.path.append('../../utilities/')
from logger import print_regular, print_error, print_success

# Global path definitions
ROOT_PATH = "../../../"
SOLUTION_PATH = os.path.join(ROOT_PATH, "workspaces", "windows", "root3d.sln")
EXECUTABLE_PATH_TEMPLATE = os.path.join(ROOT_PATH, "build", "windows", "{platform}", "{configuration}", "bin", "Root3D.exe")
BUILD_TOOLS = os.path.join("resources", "build_tools")
MSBUILD_PATH = os.path.join(BUILD_TOOLS, "MSBuild", "Current", "Bin", "MSBuild.exe")

def parse_arguments():
    parser = argparse.ArgumentParser()
    parser.add_argument("configuration", choices=["debug", "release"])
    parser.add_argument("platform", choices=["x86", "x64"])
    return parser.parse_args()

def build(configuration, platform):
    returnValue = False
    
    if platform == "x86":
        platform = "Win32"
    elif platform == "x64":
        platform = "x64"

    if not validate_toolchain():
        print_error("** Build tools validation failed. Aborting Build...")
    else:
        print_regular("** Building...")
        command = f'"{MSBUILD_PATH}" "{SOLUTION_PATH}" /p:Configuration={configuration} /p:Platform="{platform}" /v:detailed'
        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
        stdout, stderr = process.communicate()

        if process.returncode == 0:
            returnValue = True
            output_path = os.path.abspath(EXECUTABLE_PATH_TEMPLATE.format(platform=platform, configuration=configuration))
            print_success("** Build successful. Output location:")
            print_regular(output_path)
        else:
            print_error("** Build failed. Standard Output:")
            print_error(stdout.decode())
    
    return returnValue

def validate_toolchain():
    returnValue = True
    
    if not os.path.exists(MSBUILD_PATH):
        returnValue = False
        print_error("** Failed to locate 'MSBuild' path.")
        
    if not os.path.exists(SOLUTION_PATH):
        returnValue = False
        print_error("** Failed to locate 'root3d.sln' path.")

    return returnValue

def run(configuration, platform):
    executable_path = EXECUTABLE_PATH_TEMPLATE.format(platform=platform, configuration=configuration)

    if executable_path:
        print_regular("** Starting executable...")
        try:
            subprocess.Popen(executable_path, creationflags=subprocess.CREATE_NEW_CONSOLE)
            print_success("** Executable started successfully.")
        except Exception as e:
            print_error("** Error:")
            print_error(str(e))
