import serial
import time
from PIL import Image

image = Image.open('get.jpg')
width, height = image.size
ser = serial.Serial(port='/dev/ttyACM0', baudrate=115200, timeout=0.0)

DISPLAY_WIDTH = 320
DISPLAY_HEIGHT = 240

width, height = image.size

if width > DISPLAY_WIDTH or height > DISPLAY_HEIGHT:
    image.thumbnail((DISPLAY_WIDTH, DISPLAY_HEIGHT), Image.Resampling.LANCZOS)
    width, height = image.size

if image.mode != 'RGB':
    image = image.convert('RGB')

time.sleep(2)
ser.reset_input_buffer()
total_pixels = 0

try:

    for y in range(height):
        for x in range(width):
            r, g, b = image.getpixel((x, y))
            
            color_hex = (r << 16) | (g << 8) | b
        
            cmd = f"disp_px {x} {y} {color_hex:06X}\n"                
            ser.write(cmd.encode('ascii'))
            total_pixels += 1
            
            time.sleep(0.0005)
    
finally:
    time.sleep(0.1)
    ser.close()