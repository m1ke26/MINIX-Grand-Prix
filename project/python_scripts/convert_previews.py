from PIL import Image
import os

MAPS_DIR = os.path.join(os.path.dirname(__file__), "..", "maps")
INCLUDE_DIR = os.path.join(os.path.dirname(__file__), "..", "include")
MAX_COLORS = 80

# Build character set (exclude ", \, ?, ', space)
chars = []
for c in range(33, 127):
    ch = chr(c)
    if ch not in ('"', '\\', '?', "'"):
        chars.append(ch)

def generate_preview_xpm(track_num):
    png_path = os.path.join(MAPS_DIR, f"track{track_num}.png")
    out_h_path = os.path.join(INCLUDE_DIR, f"track{track_num}_preview_xpm.h")
    
    if not os.path.exists(png_path):
        print(f"Skipping track {track_num} (PNG not found)")
        return
        
    img = Image.open(png_path).convert("RGB")
    # Resize to 164x123 for menu preview
    img = img.resize((164, 123), Image.Resampling.LANCZOS)
    
    img_q = img.quantize(colors=MAX_COLORS, method=Image.Quantize.MEDIANCUT).convert("RGB")
    pixels = list(img_q.getdata())
    unique_colors = sorted(set(pixels))
    
    color_to_char = {}
    for idx, color in enumerate(unique_colors):
        if idx >= len(chars):
            break
        color_to_char[color] = chars[idx]
        
    w, h = img_q.size
    num_colors = len(color_to_char)
    
    with open(out_h_path, "w") as f:
        f.write(f'#ifndef _TRACK{track_num}_PREVIEW_XPM_H_\n')
        f.write(f'#define _TRACK{track_num}_PREVIEW_XPM_H_\n\n')
        f.write('#include <lcom/lcf.h>\n\n')
        f.write(f'static xpm_row_t const track{track_num}_preview_xpm[] = {{\n')
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
        
    print(f"Saved {out_h_path}")

def main():
    for i in range(1, 4):
        generate_preview_xpm(i)

if __name__ == "__main__":
    main()
