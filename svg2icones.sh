#!/usr/bin/env sh

# Check if we have exactly one argument
if [ $# -ne 1 ]; then
    printf "Usage: %s input.svg\n" "$0"
    printf "Example: %s BeatIt.svg\n" "$0"
    exit 1
fi

# Check if the file has .svg extension
case "$1" in
    *.svg) ;;
    *)  printf "Error: Input file must be a .svg file\n"
        exit 1
        ;;
esac

# Check if the input file exists
if [ ! -f "$1" ]; then
    printf "Error: File %s not found\n" "$1"
    exit 1
fi

# Extract base name without .svg extension
basename="${1%.svg}"

# Function to check if a required command is available
check_dependency() {
    if ! command -v "$1" >/dev/null 2>&1; then
        printf "Error: %s is not installed.\n" "$1"
        printf "Please install %s package\n" "$2"
        exit 1
    fi
}

# Check for required commands
check_dependency convert "imagemagick"
check_dependency icotool "icoutils"
check_dependency png2icns "icnsutils"

# Create a temporary directory in a portable way
# Use /tmp if available, otherwise use current directory
if [ -d "/tmp" ]; then
    temp_dir="/tmp/svg2icones.$$"
else
    temp_dir="./svg2icones.$$"
fi
mkdir -p "$temp_dir"

# Clean up the temporary directory on exit
# and handle signals for proper cleanup
trap 'rm -rf "$temp_dir"' 0
trap 'exit 2' 1 2 3 15

printf "Converting %s to various PNG sizes...\n" "$1"

# Generate PNGs of different sizes required for both ICO and ICNS
for size in 16 32 48 64 128 256 512 1024; do
    if ! convert -background none "$1" -resize "${size}x${size}" "$temp_dir/icon-${size}.png"; then
        printf "Error during PNG conversion\n"
        exit 1
    fi
done

printf "Creating .ico file...\n"

# Create Windows ICO file
# ICO format typically uses: 16x16, 32x32, 48x48, 64x64, 128x128, 256x256
if ! icotool -c -o "${basename}.ico" \
    "$temp_dir/icon-16.png" \
    "$temp_dir/icon-32.png" \
    "$temp_dir/icon-48.png" \
    "$temp_dir/icon-64.png" \
    "$temp_dir/icon-128.png" \
    "$temp_dir/icon-256.png"; then
    printf "Error during ICO creation\n"
    exit 1
fi

printf "Creating .icns file...\n"

# Create macOS ICNS file
# ICNS format typically uses: 16x16, 32x32, 48x48, 128x128, 256x256, 512x512, 1024x1024
if ! png2icns "${basename}.icns" \
    "$temp_dir/icon-16.png" \
    "$temp_dir/icon-32.png" \
    "$temp_dir/icon-48.png" \
    "$temp_dir/icon-128.png" \
    "$temp_dir/icon-256.png" \
    "$temp_dir/icon-512.png" \
    "$temp_dir/icon-1024.png"; then
    printf "Error during ICNS creation\n"
    exit 1
fi

printf "Done! Created %s.ico and %s.icns\n" "$basename" "$basename"
