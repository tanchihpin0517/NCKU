import os
import argparse
import cv2
import time

def main():
    argp = argparse.ArgumentParser()
    argp.add_argument('-a', dest='anima_dir', type=str,
                      help='animation directory')
    argp.add_argument('-i', dest='image_tags', nargs='+', default=[],
                      help='image tags')
    args = argp.parse_args()

    raw_images = []
    for i in range(len(args.image_tags)-1):
        src = args.image_tags[i]
        dst = args.image_tags[i+1]
        for T in range(101):
            image_name = f'{src}_{dst}_{T}.jpg'
            path = os.path.join(args.anima_dir, image_name)
            image = cv2.imread(path)
            raw_images.append(image)

    i = 0
    while True:
        image = raw_images[i]
        for image in raw_images:
            cv2.imshow('My Image', image)
            cv2.waitKey(10)
        i = (i+1)%len(raw_images)

    print('animation')

if __name__ == '__main__':
    main()
