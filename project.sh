# project.sh
# Utility script to build project in various modes.

if [[ $# -ne 1 ]]; then
    echo "Invalid argument count, try option 'i' for usage."
    exit 1
fi

choice="$1"

if [[ "$choice" = "d" ]]; then
    cmake --fresh -S . -B build -DDBG_BUILD:BOOL=1 && cmake --build build -- -j 2 && echo "Debug build OK!" || echo "Build failed, see logs!"
elif [[ "$choice" = "r" ]]; then
    cmake --fresh -S . -B build -DDBG_BUILD:BOOL=0 && cmake --build build -- -j 2 && echo "Release build OK!" || echo "Build failed, see logs!"
else
    echo "Usage: ./build.sh [d | r | i]\nOptions:\nd = debug build\nr = release build\ni = usage info"
fi
