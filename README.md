# Root3D

Root3D is a 3D engine project focused on platform independence. Many parts of the engine are still in development.

> **Note:** The engine is currently developed on x64 Windows, so all instructions and setups have been tested on that platform. The folder structure might change as the project evolves.

Currently, the engine can be built for Android and Windows. Future plans include support for more platforms.

## Getting Started
1. Ensure Python 3 is installed on your system.
2. Install Android Studio for Android development.
3. Install Visual Studio 2019 for Windows development.
4. Run the `setup_ide` script in the `tools/<platform>/project-gen` directory.

## Building
**Important:** Before building, run the `pack_assets` script in the `tools` directory to package resources.

There are two ways to build the generated projects:

1. **Using the IDE Toolset:**
   - Open the generated project in the appropriate IDE.
   - The IDE will automatically download the necessary build tools.
   - After the tools are downloaded, you can build the project directly within the IDE.

2. **Using Pre-determined Build Tools:**
   - Navigate to `tools/<platform>/build-tools` to find various build scripts.
   - Run the `get_tools` script to download the required build tools.
   - Use the `build` and `build_and_run` scripts to build the project.
   - Specify parameters such as configuration and platform when using the build scripts.
   - Instructions will be provided upon usage.

## License
This project is licensed under the MIT License.  
See the [LICENSE](LICENSE.txt) file for details.

## Authors
- [@StaviRare](https://github.com/StaviRare)
