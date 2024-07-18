import os
import sys
import argparse
import subprocess

# Project-specific imports
sys.path.append('../../utilities/')
from logger import print_regular, print_error, print_success

# Global path definitions
ROOT_PATH = "../../../"
BUILD_TOOLS = os.path.join("resources", "build_tools")
JDK_PATH = os.path.join(BUILD_TOOLS, "jdk")
SDK_PATH = os.path.join(BUILD_TOOLS, "sdk")
NDK_PATH = os.path.join(BUILD_TOOLS, "ndk")
PROJECT_PATH = os.path.join(ROOT_PATH, "workspaces", "android")
GRADLE_PATH = os.path.join(ROOT_PATH, "workspaces", "android", "gradlew")
APK_PATH_TEMPLATE = os.path.join(ROOT_PATH, "build", "android", "outputs", "apk", "{platform}", "{configuration}", "root3d.apk")

def parse_arguments():
    parser = argparse.ArgumentParser(description="Build and run Android APK")
    parser.add_argument("configuration", choices=["debug", "release"], help="Build configuration")
    parser.add_argument("platform", choices=["arm7", "arm8", "x86", "universal"], help="Target platform/architecture")
    return parser.parse_args()

def build(configuration, platform):
    returnValue = False
    if not validate_toolchain():
        print_error("** Build tools validation failed. Aborting Build...")
    else:
        env = os.environ.copy()
        env["JAVA_HOME"] = os.path.abspath(JDK_PATH)
        env["ANDROID_HOME"] = os.path.abspath(SDK_PATH)
        env["ANDROID_NDK_HOME"] = os.path.abspath(NDK_PATH)
        gradle_task = f"assemble{platform.capitalize()}{configuration.capitalize()}"
        build_command = f'"{GRADLE_PATH}" -p "{PROJECT_PATH}" {gradle_task}'
        
        print_regular("** Building APK...")
        process = subprocess.run(build_command, shell=True, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, env=env)
        
        if process.returncode == 0:
            returnValue = True
            output_path = os.path.abspath(APK_PATH_TEMPLATE.format(platform=platform, configuration=configuration))
            print_success("** Build successful. Output location:")
            print_regular(output_path)
        else:
            print_error("** Build failed:")
            print_error(process.stderr)
    
    return returnValue

def validate_toolchain():
    returnValue = True
    
    if not os.path.exists(JDK_PATH):
        returnValue = False
        print_error("** Failed to locate 'JDK' path.")

    if not os.path.exists(SDK_PATH):
        returnValue = False
        print_error("** Failed to locate 'SDK' path.")

    if not os.path.exists(NDK_PATH):
        returnValue = False
        print_error("** Failed to locate 'NDK' path.")

    if not os.path.exists(GRADLE_PATH):
        returnValue = False
        print_error("** Failed to locate 'gradlew' path.")
        
    return returnValue

def run(configuration, platform):
    if restart_adb():
        if install_apk(configuration, platform):
            launch_application()
                      
def restart_adb():
    print_regular("** Restarting ADB...")
    restart_adb = subprocess.run("adb kill-server && adb start-server", shell=True, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if restart_adb.returncode != 0:
        print_error("** Failed to restart ADB:")
        print_error(restart_adb.stderr.strip())
    
    return restart_adb.returncode == 0

def install_apk(configuration, platform):
    print_regular("** Installing APK...")
    apk_path = APK_PATH_TEMPLATE.format(platform=platform, configuration=configuration)
    install_apk = subprocess.run(f"adb install -r {apk_path}", shell=True, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if install_apk.returncode != 0:
        print_error("** Failed to install APK:")
        print_error(install_apk.stderr.strip())
    
    return install_apk.returncode == 0

def launch_application():
    print_regular("** Launching application...")
    adb_launch_command = "adb shell monkey -p com.example.root3d -c android.intent.category.LAUNCHER 1"
    launch_app = subprocess.run(adb_launch_command, shell=True, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)    
    if launch_app.returncode != 0:
        print_error("** Failed to launch application:")
        print_error(launch_app.stderr.strip())
    
    return launch_app.returncode == 0