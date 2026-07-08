#!/bin/bash
#
# audio_convert.sh - Convert audio file to ulaw .ul with optional leading pause
#
# Usage: audio_convert.sh input_file [output_file.ul] [pause_seconds]
#
# - If output_file is not specified, it will be named like input_file but with .ul extension
# - pause_seconds: optional number of seconds of silence to add at the start (default 0)
#
# Requires sox (apt install sox libsox-fmt-mp3)

if [ $# -lt 1 ]; then
    echo "Usage: $0 input_file [output_file.ul] [pause_seconds]"
    echo "Example: $0 announcement.mp3 announcement.ul 1.5"
    exit 1
fi

INPUT_FILE="$1"
OUTPUT_FILE="${2:-${INPUT_FILE%.*}.ul}"
PAUSE_SECONDS="${3:-0}"

# Validate pause is a number (including decimals)
if ! [[ "$PAUSE_SECONDS" =~ ^[0-9]*\.?[0-9]+$ ]]; then
    echo "Error: pause_seconds must be a number (e.g. 1, 0.5, 2.3)"
    exit 1
fi

# If pause > 0, create a temporary silence file and concatenate
if (( $(awk 'BEGIN {print ('"$PAUSE_SECONDS"' > 0)}') )); then
    TEMP_SILENCE=$(mktemp --suffix=.wav)
    
    # Create silence
    sox -n -r 8000 -c 1 -e u-law "$TEMP_SILENCE" trim 0 "$PAUSE_SECONDS"
    
    # Concatenate silence + original audio, then convert to ulaw
    sox "$TEMP_SILENCE" "$INPUT_FILE" -t raw -r 8000 -c 1 -e u-law "$OUTPUT_FILE"
    
    rm -f "$TEMP_SILENCE"
else
    # No pause — original behavior
    sox "$INPUT_FILE" -t raw -r 8000 -c 1 -e u-law "$OUTPUT_FILE"
fi

if [ $? -eq 0 ]; then
    echo "Conversion successful!"
    echo "Output file: $OUTPUT_FILE"
    if (( $(awk 'BEGIN {print ('"$PAUSE_SECONDS"' > 0)}') )); then
        echo "Added ${PAUSE_SECONDS} second pause at the beginning."
    fi
else
    echo "Error: Conversion failed."
fi
