from PIL import Image
import os

sprites_dir = r"C:\Users\User\Desktop\MINIX-LCOM\shared\grupo_2leic01_2\project\sprites"

directions = ["NORTH", "NORTHEAST", "EAST", "SOUTHEAST", "SOUTH", "SOUTHWEST", "WEST", "NORTHWEST"]
xpm_names = ["car_up", "car_up_right", "car_right", "car_down_right", "car_down", "car_down_left", "car_left", "car_up_left"]

all_xpms = []

# Build character set for XPM (exclude ", \, ?, ')
chars = []
for c in range(33, 127):
    ch = chr(c)
    if ch != '"' and ch != '\\' and ch != '?' and ch != "'":
        chars.append(ch)

all_first = [" "] + chars
all_second = chars

MAX_COLORS = 200

for i, d in enumerate(directions):
    fname = f"{d}_000.png"
    fpath = os.path.join(sprites_dir, fname)
    img = Image.open(fpath).convert("RGBA")

    # Quantize RGB to reduce color count
    rgb = img.convert("RGB")
    rgb_q = rgb.quantize(colors=MAX_COLORS, method=Image.Quantize.MEDIANCUT).convert("RGB")

    alpha = img.split()[3]
    pixels_q = list(rgb_q.getdata())
    alpha_data = list(alpha.getdata())

    # Transparent pixels -> black, black car pixels -> (1,1,1)
    pixels = []
    for px, a in zip(pixels_q, alpha_data):
        if a < 128:
            pixels.append((0, 0, 0))
        elif px == (0, 0, 0):
            pixels.append((1, 1, 1))
        else:
            pixels.append(px)

    unique_colors = set(pixels)
    unique_colors.discard((0, 0, 0))

    # Map colors to 2-char codes
    color_to_char = {}
    color_to_char[(0, 0, 0)] = "  "

    idx = 0
    for color in sorted(unique_colors):
        first_idx = idx // len(all_second)
        second_idx = idx % len(all_second)
        if first_idx >= len(all_first):
            break
        code = all_first[first_idx] + all_second[second_idx]
        if code == "  ":
            idx += 1
            first_idx = idx // len(all_second)
            second_idx = idx % len(all_second)
            code = all_first[first_idx] + all_second[second_idx]
        color_to_char[color] = code
        idx += 1

    w, h = img.size
    num_colors = len(color_to_char)
    cpp = 2

    xpm_lines = []
    xpm_lines.append(f'static xpm_row_t const {xpm_names[i]}_xpm[] = {{')
    xpm_lines.append(f'  "{w} {h} {num_colors} {cpp}",')

    for color, ch in sorted(color_to_char.items(), key=lambda x: x[1]):
        r, g, b = color
        xpm_lines.append(f'  "{ch} c #{r:02X}{g:02X}{b:02X}",')

    for y in range(h):
        row = ""
        for x in range(w):
            px = pixels[y * w + x]
            row += color_to_char.get(px, "  ")
        comma = "," if y < h - 1 else ""
        xpm_lines.append(f'  "{row}"{comma}')

    xpm_lines.append('};')
    all_xpms.append('\n'.join(xpm_lines))
    print(f"  {xpm_names[i]}: {w}x{h}, {num_colors} colors, {cpp} cpp")

header_path = os.path.join(r"C:\Users\User\Desktop\MINIX-LCOM\shared\grupo_2leic01_2\project\include", "car_pixmaps.h")
with open(header_path, 'w') as f:
    f.write('#ifndef _CAR_PIXMAPS_H_\n')
    f.write('#define _CAR_PIXMAPS_H_\n\n')
    f.write('#include <lcom/lcf.h>\n\n')
    for xpm in all_xpms:
        f.write(xpm + '\n\n')

    f.write('#define CAR_XPM_COUNT 8\n')
    f.write('static xpm_row_t * const car_xpms[CAR_XPM_COUNT] = {\n')
    for i, name in enumerate(xpm_names):
        comma = "," if i < 7 else ""
        f.write(f'    (xpm_row_t *) {name}_xpm{comma}\n')
    f.write('};\n\n')
    f.write('#endif /* _CAR_PIXMAPS_H_ */\n')

print(f"\nDone! Saved to {header_path}")
