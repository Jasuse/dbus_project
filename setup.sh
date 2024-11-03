conan profile detect
conan install . --output-folder=build --build=missing
meson setup --native-file build/conan_meson_native.ini . build
meson build

meson compile -C build