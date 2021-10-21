import argparse
import os
import utils
from image import Image
import algo
import cv2
from tqdm import tqdm

META_FILE = 'meta.txt'

def main():
    argp = argparse.ArgumentParser()
    argp.add_argument('-d', dest='data_dir', type=str,
                      help='data directory')
    argp.add_argument('-a', dest='anima_dir', type=str,
                      help='data directory')
    argp.add_argument('-i', dest='image_tags', nargs='+', default=[],
                      help='image tags')
    args = argp.parse_args()

    images = load_images(args)
    for i in range(len(args.image_tags)-1):
        src_tag = args.image_tags[i]
        src_image = images[src_tag]
        dst_tag = args.image_tags[i+1]
        dst_image = images[dst_tag]
        print(src_tag, dst_tag)
        a = 1e-4
        b = 1.0
        p = 1.0
        pbar = tqdm(total=101)
        for t in range(101):
            out = morphing(src_image, dst_image, t/100, a, b, p, algo.INTERPOLATION_ENDPOINT)
            save_image(out, args.anima_dir, src_tag, dst_tag, t)
            pbar.update(1)
        pbar.close()


def load_images(args):
    data_map = {}
    with open(os.path.join(args.data_dir, META_FILE)) as f:
        for line in f:
            line = line.strip()
            (tag, img_file, cl_file) = tuple(line.split(' '))
            img_file = os.path.join(args.data_dir, img_file)
            cl_file = os.path.join(args.data_dir, cl_file)
            img = Image(img_file, cl_file)
            #images.append((tag, img))
            data_map[tag] = img
    return data_map

def morphing(src, dst, t, a, b, p, interpolation):
    try:
        out, src, dst = algo.morphing(src, dst, t, a, b, p, interpolation)
    except AssertionError as e:
        print(e)
        exit(1)
    return out

def save_image(img, out_dir, tag_1, tag_2, t):
    file_name = f'{tag_1}_{tag_2}_{t}.jpg'
    path = os.path.join(out_dir, file_name)
    cv2.imwrite(path, img)

if __name__ == '__main__':
    main()
