from PIL import Image
import os
import sys

fileCount = next(os.walk("working\\img"))[2]

nameIndex = 1
for list_item in fileCount:
    imgPath = "working\\img\\img" + str(nameIndex) + ".png"

    widthRead = open("working\\width", "r")
    imgWidth = widthRead.read()
    imgWidth_int = int(imgWidth)
    widthRead.close()

    heightRead = open("working\\height", "r")
    imgHeight = heightRead.read()
    imgHeight_int = int(imgHeight)
    heightRead.close()

    area = imgWidth_int * imgHeight_int

    x = 0
    y = 0
    rgb_str = ""

    img = Image.open(imgPath).convert('RGB')
    for i in range(area):
        rgb = img.getpixel((x, y))
        rgb_str += str(rgb[0]) + "," + str(rgb[1]) + "," + str(rgb[2]) + "|"

        x += 1
        if x == imgWidth_int:
            y += 1
            x = 0

    print("Converting img" + str(nameIndex) + ".png to RGB")
    output_file = open("working\\rgb\\img_rgb" + str(nameIndex), "w")
    output_file.write(rgb_str[:-1])
    output_file.close()

    nameIndex += 1

sys.exit(0)