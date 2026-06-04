from PIL import Image
import os

collision_path = r"C:\Users\User\Desktop\MINIX-LCOM\shared\grupo_2leic01_2\project\maps\collision1.png"
output_path = r"C:\Users\User\Desktop\MINIX-LCOM\shared\grupo_2leic01_2\project\include\collision1_xpm.h"

img = Image.open(collision_path).convert("RGB")
print(f"Size: {img.size[0]}x{img.size[1]}")

pixels = list(img.getdata())
w, h = img.size

# Snap each pixel to nearest of 3 colors: black, red, yellow
# Black (0,0,0) = road, Red (255,0,0) = blocked, Yellow (255,255,0) = slow
def snap_color(px):
    r, g, b = px[:3]
    # If green is high -> yellow
    if g > 128:
        return (255, 255, 0)
    # If red is high -> red
    if r > 128:
        return (255, 0, 0)
    # Otherwise -> black
    return (0, 0, 0)

snapped = [snap_color(px) for px in pixels]

# Count results
from collections import Counter
counts = Counter(snapped)
for color, count in counts.items():
    pct = count / len(snapped) * 100
    name = {(0,0,0): "ROAD", (255,0,0): "BLOCKED", (255,255,0): "SLOW"}[color]
    print(f"  {name}: {count} pixels ({pct:.1f}%)")

# Only 3 colors, 1 char per pixel
color_to_char = {
    (0, 0, 0):     ".",   # road
    (255, 0, 0):   "#",   # blocked
    (255, 255, 0): "~",   # slow
}

num_colors = 3
cpp = 1

print(f"Generating XPM: {w}x{h}, {num_colors} colors, {cpp} cpp")
print(f"Max row length: {w + 4} chars (limit: 4095)")

with open(output_path, 'w') as f:
    f.write('#ifndef _COLLISION1_XPM_H_\n')
    f.write('#define _COLLISION1_XPM_H_\n\n')
    f.write('#include <lcom/lcf.h>\n\n')
    f.write('static xpm_row_t const collision1_xpm[] = {\n')
    f.write(f'  "{w} {h} {num_colors} {cpp}",\n')

    for color, ch in sorted(color_to_char.items(), key=lambda x: x[1]):
        r, g, b = color
        f.write(f'  "{ch} c #{r:02X}{g:02X}{b:02X}",\n')

    for y in range(h):
        row = ""
        for x in range(w):
            px = snapped[y * w + x]
            row += color_to_char[px]
        comma = "," if y < h - 1 else ""
        f.write(f'  "{row}"{comma}\n')

    f.write('};\n\n')
    f.write('#endif\n')

print(f"\nDone! Saved to {output_path}")
