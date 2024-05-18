import os
import shutil

ROOT_PATH = "../../../"
RESOURCES = os.path.join("resources")
OUTPUT_PATH = os.path.join(ROOT_PATH, "workspaces", "android")

GRADLEW = os.path.join(RESOURCES, "gradle", "gradlew")
GRADLEW_BAT = os.path.join(RESOURCES, "gradle", "gradlew.bat")
GRADLE_WRAPPER = os.path.join(RESOURCES, "gradle", "gradle-wrapper.jar")
WRAPPER_PROPS = os.path.join(RESOURCES, "gradle", "gradle-wrapper.properties")
BUILD_GRADLE = os.path.join(RESOURCES, "project", "build.gradle.kts")
SETTINGS_GRADLE = os.path.join(RESOURCES, "project", "settings.gradle.kts")
GRADLE_PROPS = os.path.join(RESOURCES, "project", "gradle.properties")
APP_GRADLE = os.path.join(RESOURCES, "app", "build.gradle.kts")
PROGUARD = os.path.join(RESOURCES, "app", "proguard-rules.pro")
CMAKE_LISTS = os.path.join(RESOURCES, "app", "CMakeLists.txt")
MANIFEST = os.path.join(RESOURCES, "apk", "AndroidManifest.xml")
RES_DIR = os.path.join(RESOURCES, "apk", "res")


# Main Program
print("** Starting Android Studio project generation...")

os.makedirs(os.path.join(OUTPUT_PATH, "gradle", "wrapper"), exist_ok=True)
os.makedirs(os.path.join(OUTPUT_PATH, "app", "src", "main"), exist_ok=True)
os.makedirs(os.path.join(OUTPUT_PATH, "app", "src", "main", "cpp"), exist_ok=True)

shutil.copy(GRADLE_WRAPPER, os.path.join(OUTPUT_PATH, "gradle", "wrapper"))
shutil.copy(WRAPPER_PROPS, os.path.join(OUTPUT_PATH, "gradle", "wrapper"))
shutil.copy(GRADLEW, OUTPUT_PATH)
shutil.copy(GRADLEW_BAT, OUTPUT_PATH)
shutil.copy(BUILD_GRADLE, OUTPUT_PATH)
shutil.copy(SETTINGS_GRADLE, OUTPUT_PATH)
shutil.copy(GRADLE_PROPS, OUTPUT_PATH)
shutil.copy(APP_GRADLE, os.path.join(OUTPUT_PATH, "app"))
shutil.copy(PROGUARD, os.path.join(OUTPUT_PATH, "app"))
shutil.copy(CMAKE_LISTS, os.path.join(OUTPUT_PATH, "app", "src", "main", "cpp"))
shutil.copy(MANIFEST, os.path.join(OUTPUT_PATH, "app", "src", "main"))
shutil.copytree(RES_DIR, os.path.join(OUTPUT_PATH, "app", "src", "main", "res"), dirs_exist_ok=True)

print("** Android Studio project has been created successfully.")
