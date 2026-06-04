from PIL import Image
import os

track_path = r"C:\Users\User\Desktop\MINIX-LCOM\shared\grupo_2leic01_2\project\maps\track1.png"
output_path = r"C:\Users\User\Desktop\MINIX-LCOM\shared\grupo_2leic01_2\project\include\track1_xpm.h"

MAX_COLORS = 85  # 1 char per pixel, ~90 usable chars

# Build character set (exclude ", \, ?, ', space)
chars = []
for c in range(33, 127):
    ch = chr(c)
    if ch != '"' and ch != '\\' and ch != '?' and ch != "'":
        chars.append(ch)

print(f"Available chars: {len(chars)}")

img = Image.open(track_path).convert("RGB")
print(f"Original size: {img.size[0]}x{img.size[1]}")

# Quantize to reduce colors
img_q = img.quantize(colors=MAX_COLORS, method=Image.Quantize.MEDIANCUT).convert("RGB")

pixels = list(img_q.getdata())
unique_colors = sorted(set(pixels))
print(f"Unique colors after quantize: {len(unique_colors)}")

# Map each color to a single character
color_to_char = {}
for idx, color in enumerate(unique_colors):
    if idx >= len(chars):
        print(f"Warning: too many colors ({len(unique_colors)}), truncating to {len(chars)}")
        break
    color_to_char[color] = chars[idx]

w, h = img_q.size
num_colors = len(color_to_char)

print(f"Generating XPM: {w}x{h}, {num_colors} colors, 1 cpp")
print(f"Max row length: {w + 4} chars (limit: 4095)")

with open(output_path, 'w') as f:
    f.write('#ifndef _TRACK1_XPM_H_\n')
    f.write('#define _TRACK1_XPM_H_\n\n')
    f.write('#include <lcom/lcf.h>\n\n')
    f.write('static xpm_row_t const track1_xpm[] = {\n')
    f.write(f'  "{w} {h} {num_colors} 1",\n')

    for color, ch in sorted(color_to_char.items(), key=lambda x: x[1]):
        r, g, b = color
        f.write(f'  "{ch} c #{r:02X}{g:02X}{b:02X}",\n')

    for y in range(h):
        row = ""
        for x in range(w):
            px = pixels[y * w + x]
            row += color_to_char.get(px, "!")
        comma = "," if y < h - 1 else ""
        f.write(f'  "{row}"{comma}\n')

    f.write('};\n\n')
    f.write('#endif\n')

print(f"\nDone! Saved to {output_path}")
