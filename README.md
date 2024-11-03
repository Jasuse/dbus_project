
# Prerequisites:
- [Git](https://git-scm.com/),
- [Conan](https://conan.io/),
- [Meson](https://mesonbuild.com/),
- [CMake](https://cmake.org/) (for building dependencies),
- [Ninja](https://ninja-build.org/),
- [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/),
- [clang-format](https://clang.llvm.org/docs/ClangFormat.html)

# Getting started
## 1. Setting up tools
[Tested on Debian 12.7.0] To setup tools, write down these commands to terminal (command prompt)
- `sudo apt-get install pipx ninja-build cmake pkg-config clang-format`
- Agree to install packages (Y)
- `pipx install meson`
- `pipx install conan`
- If pipx will ask for setting up PATH to pipx, do it (command below)
- `pipx ensurepath`
- Reboot terminal


## 2. Clone project
- `git clone https://github.com/Jasuse/dbus_project.git`
- `cd dbus_project/`

## 3. Setup build folder
Run `./setup.sh` in root project folder

This process will take some time to download and compile all of dependencies. After this, project will be compiled and ready to use. 

Executables are located at :
- Client : build/Client
- Time server : build/TimeServer
- Permissions server : build/PermissionServer

## 4. [Optional] Build after editing
Run `./build.sh` at project`s root

Ninja will throw an error (but not refuse to build) if source code is not formatted according to ".clang-format" rules

To automatically format source code, run `./format.sh` (don`t forget to backup files) 


# Troubleshooting:
A. Project fails to compile:
1. Ensure every prerequisite is installed
2. Delete build folder and run `./setup.sh` again

B. clang-format(-check) is unknown target for ninja
1. Ensure `clang-format` is installed
2. Delete build folder and run `./setup.sh` again 

# Obvious TO DOs:
1. Move clients, servers to subprojects
2. Move DBus code to classes to ensure these objects are incapsulated (and embeddable)
3. Add async execution of methods (see 2.)
