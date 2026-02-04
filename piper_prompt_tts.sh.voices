#!/bin/bash

# Piper TTS wrapper for AllStar

# Generates ONLY .wav in /mp3 (8 kHz mono)


TEXT="$1"

OUTPUT_NAME="$2"

VOICE="$3"   # NEW: third argument = voice model path


if [ -z "$TEXT" ] || [ -z "$OUTPUT_NAME" ]; then

    echo "Usage: $0 \"Text to speak\" output_filename [voice_model]"

    exit 1

fi


# Default voice if none provided

if [ -z "$VOICE" ]; then

    VOICE="/opt/piper/voices/en_US-lessac-medium.onnx"

fi


# Paths

PIPER_BIN="/opt/piper/bin/piper/piper"  # Correct binary path

OUT_DIR="/mp3"


# Ensure output directory exists

if [ ! -d "$OUT_DIR" ]; then

    echo "ERROR: $OUT_DIR does not exist"

    exit 1

fi


# Set library path for Piper

export LD_LIBRARY_PATH="/opt/piper/bin:$LD_LIBRARY_PATH"


TMP_WAV="/tmp/${OUTPUT_NAME}_tmp.wav"

FINAL_WAV="${OUT_DIR}/${OUTPUT_NAME}.wav"


# Generate speech with selected voice

echo "$TEXT" | "$PIPER_BIN" --model "$VOICE" --output_file "$TMP_WAV"


# Convert to 8 kHz mono WAV for AllStar

sox "$TMP_WAV" -r 8000 -c 1 "$FINAL_WAV"


# Cleanup

rm -f "$TMP_WAV"


echo "Generated WAV: $FINAL_WAV (using voice: $VOICE)"
