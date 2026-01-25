#!/bin/bash
# Piper TTS wrapper for AllStar
# Generates ONLY .wav in /mp3 (8 kHz mono)

TEXT="$1"
OUTPUT_NAME="$2"

if [ -z "$TEXT" ] || [ -z "$OUTPUT_NAME" ]; then
    echo "Usage: $0 \"Text to speak\" output_filename (no extension)"
    exit 1
fi

# Paths - CORRECTED based on your actual structure
PIPER_BIN="/opt/piper/bin/piper"  # This is the real executable
PIPER_MODEL="/opt/piper/voices/en_US-lessac-medium.onnx"  # Change if using different voice
OUT_DIR="/mp3"

# Ensure output directory exists
if [ ! -d "$OUT_DIR" ]; then
    echo "ERROR: $OUT_DIR does not exist"
    exit 1
fi

# Set library path for Piper (points to /opt/piper/bin where libs are)
export LD_LIBRARY_PATH="/opt/piper/bin:$LD_LIBRARY_PATH"

TMP_WAV="/tmp/${OUTPUT_NAME}_tmp.wav"
FINAL_WAV="${OUT_DIR}/${OUTPUT_NAME}.wav"

# Generate speech with Piper
echo "$TEXT" | "$PIPER_BIN" --model "$PIPER_MODEL" --output_file "$TMP_WAV"

# Convert to 8 kHz mono WAV for AllStar
sox "$TMP_WAV" -r 8000 -c 1 "$FINAL_WAV"

# Cleanup
rm -f "$TMP_WAV"

echo "Generated WAV: $FINAL_WAV"
