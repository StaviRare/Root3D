import sys
import subprocess

# Project-specific imports
sys.path.append('../../utilities/')
from logger import print_regular, print_error, print_success

# Global path definitions
PROJECT_GEN_PATH = "resources/project_gen"
BUILD_SCRIPT = f"{PROJECT_GEN_PATH}/build.lua"
PREMAKE_EXECUTABLE = f"{PROJECT_GEN_PATH}/premake/premake5.exe"

# Main Program
print_regular("** Starting Visual Studio project generation...")
command = [PREMAKE_EXECUTABLE, "--file=" + BUILD_SCRIPT, "vs2019"]
result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

if result.returncode == 0:
    print_success("** Visual Studio project has been created successfully.")
else:
    print_error("** An error occurred while running Premake:")
    print_error(result.stderr.decode)
