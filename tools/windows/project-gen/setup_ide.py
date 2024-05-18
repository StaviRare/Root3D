import subprocess

BUILD_SCRIPT = "resources/build.lua"
PREMAKE_EXECUTABLE = "resources/premake/premake5.exe"

# Main Program
print("** Starting Visual Studio project generation...")
command = [PREMAKE_EXECUTABLE, "--file=" + BUILD_SCRIPT, "vs2019"]
result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

if result.returncode == 0:
    print("** Visual Studio project has been created successfully.")
else:
    print(f"** An error occurred while running Premake:\n{result.stderr.decode()}")
