from PIL import Image
import sys

def convert_image_to_header(input_file, output_file):
    # Open and convert image to black and white
    img = Image.open(input_file).convert('L')
    
    # Resize to match display dimensions
    img = img.resize((128, 64), Image.Resampling.LANCZOS)
    
    # Convert to binary (black and white only)
    pixels = img.load()
    binary_pixels = []
    
    # Convert to our format (1 byte per pixel, 0x00 for black, 0xFF for white)
    for y in range(64):
        for x in range(128):
            # Convert grayscale to binary (threshold at 128)
            binary_pixels.append(0xFF if pixels[x, y] > 128 else 0x00)
    
    # Create header file content
    hex_data = ['0x{:02x}'.format(b) for b in binary_pixels]
    formatted_data = ',\n    '.join([', '.join(hex_data[i:i+16]) 
                                   for i in range(0, len(hex_data), 16)])
    
    header_content = f'''#ifndef IMAGE_H
#define IMAGE_H

#define IMAGE_WIDTH  128
#define IMAGE_HEIGHT 64
#define IMAGE_SIZE   (IMAGE_WIDTH * IMAGE_HEIGHT)

const uint8_t image_data[IMAGE_SIZE] = {{
    {formatted_data}
}};

#endif // IMAGE_H
'''
    
    # Write to file
    with open(output_file, 'w') as f:
        f.write(header_content)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: {} input.png inc/image.h".format(sys.argv[0]))
        sys.exit(1)
    
    convert_image_to_header(sys.argv[1], sys.argv[2])