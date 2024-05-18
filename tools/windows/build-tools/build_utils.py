import os
import subprocess
import argparse

ROOT_PATH = "../../../"
SOLUTION_PATH = os.path.join(ROOT_PATH, "workspaces", "windows", "root3d.sln")
EXECUTABLE_PATH_TEMPLATE = os.path.join(ROOT_PATH, "build", "windows", "{platform}", "{configuration}", "bin", "Root3D.exe")
MSBUILD_PATH = "resources/MSBuild/Current/Bin/MSBuild.exe"

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
        print("** Build tools validation failed. Aborting Build...")
    else:
        print("** Building...")
        command = f'"{MSBUILD_PATH}" "{SOLUTION_PATH}" /p:Configuration={configuration} /p:Platform="{platform}" /v:detailed'
        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
        stdout, stderr = process.communicate()

        if process.returncode == 0:
            returnValue = True
            output_path = os.path.abspath(EXECUTABLE_PATH_TEMPLATE.format(platform=platform, configuration=configuration))
            print("** Build successful. Output location:")
            print(output_path)
        else:
            print("** Build failed.")
            print("** Standard Output:\n", stdout.decode())
            print("** Error Output:\n", stderr.decode())
    
    return returnValue

def validate_toolchain():
    returnValue = True
    
    if not os.path.exists(MSBUILD_PATH):
        returnValue = False
        print("** Failed to locate 'MSBuild' path.")
        
    if not os.path.exists(SOLUTION_PATH):
        returnValue = False
        print("** Failed to locate 'root3d.sln' path.")

    return returnValue

def run(configuration, platform):
    executable_path = EXECUTABLE_PATH_TEMPLATE.format(platform=platform, configuration=configuration)

    if executable_path:
        print("** Starting executable...")
        try:
            subprocess.Popen(executable_path, creationflags=subprocess.CREATE_NEW_CONSOLE)
            print("** Executable started successfully.")
        except Exception as e:
            print(f"** Error: {e}")
