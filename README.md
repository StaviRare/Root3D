# Root3D

Root3D is an experimental 3D engine focused on platform independence.

It serves as a platform for research and experimentation.

Bugs, incomplete features, and varying levels of polish are expected.

## Features
- **Platforms:** Android, Windows, Linux
- **Rendering:** Vulkan, OpenGL, OpenGLES, DirectX
- **Physics:** Jolt

## Getting Started
1. Ensure Python 3 is installed on your system.
2. Install Android Studio for Android development.
3. Install Visual Studio 2019 for Windows development.

## Building
1. Run the `env_setup` script in `tools/common/`.
2. Run the `assets_pack` script in `tools/common/` to package resources.
3. Run the `setup_project` script in `tools/platforms/<platform>/`.

There are two ways to build the generated projects:

1. **Using the IDE Toolset:**
   - Open the generated project in the appropriate IDE.
   - The IDE will automatically download the necessary build tools.
   - After the tools are downloaded, you can build the project directly within the IDE.

2. **Using Pre-determined Build Tools:**
   - Navigate to `tools/platforms/<platform>/` to find various build scripts.
   - Run the `setup_toolchain` script to download the required build tools.
   - Use the `build` and `build_and_run` scripts to build the project.
   - Specify parameters such as configuration and platform when using the build scripts.
   - Instructions will be provided upon usage.

## Architecture
<img src="https://raw.githubusercontent.com/StaviRare/root3d-media/main/engine_layers.jpg" alt="Engine Layers" width="600"/>

## License
This project is licensed under the MIT License.  
See the [LICENSE](LICENSE.txt) file for details.

## Authors
- [@StaviRare](https://github.com/StaviRare)
