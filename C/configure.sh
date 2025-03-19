#!/bin/bash

echo "[+] Connfig "

# Check for required tools
for tool in gcc make; do
    if ! command -v $tool &> /dev/null; then
        echo "$tool is required but not installed. Please install it."
        exit 1
    fi
done

echo "[+] All required tools are installed."
echo "[+] Configuration complete."
