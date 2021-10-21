from PIL import Image

width, height = 2048, 2048

def gen_img(pixel, file_name):
    img = Image.new('RGB', (width, height))
    color = list([pixel] * width * height)
    img.putdata(color)
    img.save(file_name)


gen_img((128, 128, 128), "grey.png")
gen_img((255, 255, 255), "white.png")
gen_img((255, 0, 0), "red.png")
gen_img((0, 255, 0), "green.png")
gen_img((0, 0, 255), "blue.png")
