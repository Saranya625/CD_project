#!/usr/bin/env sh

ASM_FILE="${1:-output.s}"
CREATOR_URL="${2:-https://creatorsim.github.io/creator/}"

if [ ! -f "$ASM_FILE" ]; then
  echo "Error: assembly file not found: $ASM_FILE" >&2
  exit 1
fi

if [ ! -s "$ASM_FILE" ]; then
  echo "Error: assembly file is empty: $ASM_FILE" >&2
  exit 1
fi

copy_ok=0
if command -v wl-copy >/dev/null 2>&1; then
  wl-copy < "$ASM_FILE"
  copy_ok=1
elif command -v xclip >/dev/null 2>&1; then
  xclip -selection clipboard < "$ASM_FILE"
  copy_ok=1
elif command -v xsel >/dev/null 2>&1; then
  xsel --clipboard --input < "$ASM_FILE"
  copy_ok=1
elif command -v pbcopy >/dev/null 2>&1; then
  pbcopy < "$ASM_FILE"
  copy_ok=1
elif command -v clip.exe >/dev/null 2>&1; then
  clip.exe < "$ASM_FILE"
  copy_ok=1
fi

open_ok=0
if command -v xdg-open >/dev/null 2>&1; then
  xdg-open "$CREATOR_URL" >/dev/null 2>&1 &
  open_ok=1
elif command -v wslview >/dev/null 2>&1; then
  wslview "$CREATOR_URL" >/dev/null 2>&1 &
  open_ok=1
elif command -v open >/dev/null 2>&1; then
  open "$CREATOR_URL" >/dev/null 2>&1 &
  open_ok=1
elif command -v cmd.exe >/dev/null 2>&1; then
  cmd.exe /C start "" "$CREATOR_URL" >/dev/null 2>&1
  open_ok=1
fi

if [ "$open_ok" -eq 1 ]; then
  echo "Opened Creator: $CREATOR_URL"
else
  echo "Could not auto-open browser. Open this URL manually: $CREATOR_URL"
fi

if [ "$copy_ok" -eq 1 ]; then
  echo "Copied '$ASM_FILE' to clipboard. Click Creator editor and press Ctrl+V."
else
  echo "No clipboard tool found. Paste manually from file: $ASM_FILE"
fi
