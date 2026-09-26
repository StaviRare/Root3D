# build_utils.py

import os
import sys
import argparse
import subprocess

# Project-specific imports
sys.path.append('../../utilities/')
from logger import print_regular, print_error, print_success

# Global path definitions
ROOT_PATH = "../../../"
PROJECT_PATH = os.path.join(ROOT_PATH, "workspaces", "linux")
CMAKE_PATH = os.path.join(PROJECT_PATH, "CMakeLists.txt")
BUILD_PATH = os.path.join(ROOT_PATH, "build", "linux", "{configuration}")
EXECUTABLE_PATH_TEMPLATE = os.path.join(
    BUILD_PATH,
    "Root3D"
)

DISTROBOX_NAME = "root3d-linux"
DISTROBOX_IMAGE = "ubuntu:24.04"

PACKAGES = [
    "build-essential",
    "cmake",
    "gdb",
    "libwayland-dev",
    "libegl-dev",
    "libgles-dev"
]


def parse_arguments():
    parser = argparse.ArgumentParser(description="Build and run Linux application")
    parser.add_argument("configuration", choices=["debug", "release"], help="Build configuration")
    return parser.parse_args()


def setup_toolchain():
    returnValue = False

    print_regular("** Setting up Linux toolchain...")

    if validate_distrobox():
        if destroy_distrobox():
            if create_distrobox():
                if install_packages():
                    returnValue = True
                    print_success("** Linux toolchain setup completed successfully.")

    return returnValue


def validate_distrobox():
    returnValue = False

    result = subprocess.run(
        ["distrobox", "list"],
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )

    if result.returncode == 0:
        returnValue = True
    else:
        print_error("** Failed to execute Distrobox.")
        print_error(result.stderr.strip())

    return returnValue


def destroy_distrobox():
    returnValue = True

    result = subprocess.run(
        ["distrobox", "list"],
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )

    if result.returncode == 0 and DISTROBOX_NAME in result.stdout:
        print_regular(f"** Destroying Distrobox '{DISTROBOX_NAME}'...")

        result = subprocess.run(
            ["distrobox", "rm", "--force", DISTROBOX_NAME],
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )

        if result.returncode == 0:
            print_success("** Distrobox destroyed successfully.")
        else:
            returnValue = False
            print_error("** Failed to destroy Distrobox.")
            print_error(result.stderr.strip())

    return returnValue


def create_distrobox():
    returnValue = False

    print_regular(f"** Creating Distrobox '{DISTROBOX_NAME}'...")

    result = subprocess.run(
        [
            "distrobox",
            "create",
            "--name",
            DISTROBOX_NAME,
            "--image",
            DISTROBOX_IMAGE,
            "--yes"
        ],
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )

    if result.returncode == 0:
        returnValue = True
        print_success("** Distrobox created successfully.")
    else:
        print_error("** Failed to create Distrobox.")
        print_error(result.stderr.strip())

    return returnValue


def install_packages():
    returnValue = False

    print_regular("** Updating package lists...")

    result = subprocess.run(
        [
            "distrobox",
            "enter",
            DISTROBOX_NAME,
            "--",
            "sudo",
            "apt",
            "update"
        ],
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )

    if result.returncode == 0:
        installSuccess = True

        for package in PACKAGES:
            print_regular(f"** Installing package '{package}'...")

            result = subprocess.run(
                [
                    "distrobox",
                    "enter",
                    DISTROBOX_NAME,
                    "--",
                    "sudo",
                    "apt",
                    "install",
                    "-y",
                    package
                ],
                text=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE
            )

            if result.returncode == 0:
                print_success(f"** Package '{package}' installed.")
            else:
                installSuccess = False
                print_error(f"** Failed to install package '{package}'.")
                print_error(result.stderr.strip())

        if installSuccess:
            returnValue = True
    else:
        print_error("** Failed to update package lists.")
        print_error(result.stderr.strip())

    return returnValue


def build(configuration):
    returnValue = False

    if not validate_toolchain():
        print_error("** Build tools validation failed. Aborting Build...")
    else:
        print_regular("** Building...")

        build_type = configuration.capitalize()
        build_path = os.path.abspath(
            BUILD_PATH.format(configuration=configuration)
        )
        project_path = os.path.abspath(PROJECT_PATH)

        configure_command = [
            "distrobox",
            "enter",
            DISTROBOX_NAME,
            "--",
            "cmake",
            "-S",
            project_path,
            "-B",
            build_path,
            f"-DCMAKE_BUILD_TYPE={build_type}"
        ]

        result = subprocess.run(
            configure_command,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )

        if result.returncode == 0:
            build_command = [
                "distrobox",
                "enter",
                DISTROBOX_NAME,
                "--",
                "cmake",
                "--build",
                build_path,
                "--config",
                build_type
            ]

            result = subprocess.run(
                build_command,
                text=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE
            )

            if result.returncode == 0:
                returnValue = True

                output_path = os.path.abspath(
                    EXECUTABLE_PATH_TEMPLATE.format(
                        configuration=configuration
                    )
                )

                print_success("** Build successful. Output location:")
                print_regular(output_path)
            else:
                print_error("** Build failed:")
                print_error(result.stderr)
        else:
            print_error("** CMake configuration failed:")
            print_error(result.stderr)

    return returnValue


def validate_toolchain():
    returnValue = True

    if not os.path.exists(CMAKE_PATH):
        returnValue = False
        print_error("** Failed to locate 'CMakeLists.txt'.")

    result = subprocess.run(
        ["distrobox", "list"],
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )

    if result.returncode != 0:
        returnValue = False
        print_error("** Failed to query Distrobox.")
    elif DISTROBOX_NAME not in result.stdout:
        returnValue = False
        print_error(f"** Failed to locate Distrobox '{DISTROBOX_NAME}'.")

    return returnValue


def run(configuration):
    executable_path = os.path.abspath(
        EXECUTABLE_PATH_TEMPLATE.format(
            configuration=configuration
        )
    )

    if os.path.exists(executable_path):
        print_regular("** Starting executable...")

        try:
            subprocess.Popen([executable_path])
            print_success("** Executable started successfully.")
        except Exception as error:
            print_error("** Error:")
            print_error(str(error))
    else:
        print_error("** Failed to locate executable:")
        print_error(executable_path)