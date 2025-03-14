#!/bin/bash

# Check if input file is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 filename"
    exit 1
fi

INPUT_FILE="$1"
OUTPUT_FILE="${INPUT_FILE%.*}_deduped.${INPUT_FILE##*.}"
LOG_FILE="${INPUT_FILE%.*}_duplicates.log"

# Check if input file exists
if [ ! -f "$INPUT_FILE" ]; then
    echo "Error: File $INPUT_FILE does not exist."
    exit 1
fi

# Create log file
echo "Duplicate lines removed from $INPUT_FILE:" > "$LOG_FILE"
echo "----------------------------------------" >> "$LOG_FILE"

# Using awk for best performance - O(n) time complexity with hash table
# This approach processes the file in a single pass
# 1. Keeps track of lines seen in an associative array
# 2. Prints each unique line to the output file
# 3. Logs duplicate lines with their line numbers
awk '
{
    if (!seen[$0]++) {
        # First time seeing this line, print to output
        print $0 > "'$OUTPUT_FILE'"
    } else {
        # Duplicate line, log it
        print "Line " NR ": " $0 > "'$LOG_FILE'"
    }
}
' "$INPUT_FILE"

# Count results
ORIGINAL_COUNT=$(wc -l < "$INPUT_FILE")
UNIQUE_COUNT=$(wc -l < "$OUTPUT_FILE")
DUPLICATE_COUNT=$((ORIGINAL_COUNT - UNIQUE_COUNT))

echo
echo "Results:"
echo "  Original lines: $ORIGINAL_COUNT"
echo "  Unique lines: $UNIQUE_COUNT"
echo "  Duplicates removed: $DUPLICATE_COUNT"
echo
echo "Removed duplicates logged in: $LOG_FILE"
echo "Deduplicated file saved as: $OUTPUT_FILE"
