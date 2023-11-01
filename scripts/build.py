import subprocess
import sys
import os

def run_script(script_name):
    script_path = os.path.join(os.path.dirname(os.path.realpath(__file__)), script_name)
    process = subprocess.run([sys.executable, script_path], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    
    if process.returncode == 0:
        print(f"{script_name} succeeded.")
        return True
    else:
        print(f"{script_name} failed.")
        print("Output:\n", process.stdout)
        print("Errors:\n", process.stderr)
        return False

# Main Program
print("Build process starting...")
run_script('build_project.py')
run_script('package_resources.py')