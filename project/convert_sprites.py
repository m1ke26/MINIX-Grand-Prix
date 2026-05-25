from PIL import Image
import os

sprites_dir = r"C:\Users\User\Desktop\MINIX-LCOM\shared\grupo_2leic01_2\project\sprites"

# 8 original directions from the sprite pack
orig_directions = ["NORTH", "NORTHEAST", "EAST", "SOUTHEAST", "SOUTH", "SOUTHWEST", "WEST", "NORTHWEST"]

# 16 XPM names: original + intermediate (rotated 22.5 CW from the previous original)
xpm_names = [
    "car_n",    # 0    NORTH (original)
    "car_nne",  # 1    22.5  (rotate NORTH +22.5)
    "car_ne",   # 2    NORTHEAST (original)
    "car_ene",  # 3    67.5  (rotate NORTHEAST +22.5)
    "car_e",    # 4    EAST (original)
    "car_ese",  # 5    112.5 (rotate EAST +22.5)
    "car_se",   # 6    SOUTHEAST (original)
    "car_sse",  # 7    157.5 (rotate SOUTHEAST +22.5)
    "car_s",    # 8    SOUTH (original)
    "car_ssw",  # 9    202.5 (rotate SOUTH +22.5)
    "car_sw",   # 10   SOUTHWEST (original)
    "car_wsw",  # 11   247.5 (rotate SOUTHWEST +22.5)
    "car_w",    # 12   WEST (original)
    "car_wnw",  # 13   292.5 (rotate WEST +22.5)
    "car_nw",   # 14   NORTHWEST (original)
    "car_nnw",  # 15   337.5 (rotate NORTHWEST +22.5)
]

# Build character set for XPM (exclude ", \, ?, ')
chars = []
for c in range(33, 127):
    ch = chr(c)
    if ch != '"' and ch != '\\' and ch != '?' and ch != "'":
        chars.append(ch)

all_first = [" "] + chars
all_second = chars

MAX_COLORS = 200

def load_sprite_image(direction):
    """Load a sprite PNG and return the RGBA image."""
    fname = f"{direction}_000.png"
    fpath = os.path.join(sprites_dir, fname)
    return Image.open(fpath).convert("RGBA")

def rotate_sprite(img, angle_cw):
    """Rotate an RGBA sprite clockwise by angle_cw degrees, keeping canvas size."""
    # PIL rotate is CCW, so negate. Use BICUBIC for smooth edges.
    rotated = img.rotate(-angle_cw, resample=Image.BICUBIC, expand=False)
    return rotated

def process_image(img):
    """Convert RGBA image to quantized pixel list with transparency handling."""
    # Quantize RGB
    rgb = img.convert("RGB")
    rgb_q = rgb.quantize(colors=MAX_COLORS, method=Image.Quantize.MEDIANCUT).convert("RGB")

    alpha = img.split()[3]
    pixels_q = list(rgb_q.getdata())
    alpha_data = list(alpha.getdata())

    pixels = []
    for px, a in zip(pixels_q, alpha_data):
        if a < 128:  # Use threshold for rotated sprites (anti-aliased alpha)
            pixels.append((0, 0, 0))
        elif px == (0, 0, 0):
            pixels.append((1, 1, 1))
        else:
            pixels.append(px)

    return pixels, img.size

def generate_xpm(name, pixels, size):
    """Generate XPM string from pixel data."""
    w, h = size
    unique_colors = set(pixels)
    unique_colors.discard((0, 0, 0))

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

    num_colors = len(color_to_char)
    cpp = 2

    xpm_lines = []
    xpm_lines.append(f'static xpm_row_t const {name}_xpm[] = {{')
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
    return '\n'.join(xpm_lines), num_colors

all_xpms = []

for i in range(16):
    orig_idx = i // 2  # Which original direction (0-7)
    is_intermediate = (i % 2 == 1)  # Odd = intermediate (rotated)

    img = load_sprite_image(orig_directions[orig_idx])

    if is_intermediate:
        img = rotate_sprite(img, 22.5)

    pixels, size = process_image(img)
    xpm_str, num_colors = generate_xpm(xpm_names[i], pixels, size)
    all_xpms.append(xpm_str)

    label = f"{orig_directions[orig_idx]}+22.5" if is_intermediate else orig_directions[orig_idx]
    print(f"  {xpm_names[i]:10s}: {size[0]}x{size[1]}, {num_colors:3d} colors  ({label})")

header_path = os.path.join(r"C:\Users\User\Desktop\MINIX-LCOM\shared\grupo_2leic01_2\project\include", "car_pixmaps.h")
with open(header_path, 'w') as f:
    f.write('#ifndef _CAR_PIXMAPS_H_\n')
    f.write('#define _CAR_PIXMAPS_H_\n\n')
    f.write('#include <lcom/lcf.h>\n\n')
    for xpm in all_xpms:
        f.write(xpm + '\n\n')

    f.write('#define CAR_XPM_COUNT 16\n')
    f.write('static xpm_row_t * const car_xpms[CAR_XPM_COUNT] = {\n')
    for i, name in enumerate(xpm_names):
        comma = "," if i < 15 else ""
        f.write(f'    (xpm_row_t *) {name}_xpm{comma}\n')
    f.write('};\n\n')
    f.write('#endif /* _CAR_PIXMAPS_H_ */\n')

print(f"\nDone! 16 sprites saved to {header_path}")
