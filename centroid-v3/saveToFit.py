import numpy as np
import matplotlib.pyplot as plt
from astropy.io import fits


def main():
    f = open("frame.csv", "r")
    if f.mode == "r":
        pic_size = f.readline().split(";")
        data = f.readlines()
        f.close()

        w = pic_size[0]
        h = pic_size[1]
        image = np.array([np.array(row.split(",")) for row in data]).astype('uint16')
        # plt.imshow(image)
        # plt.show()

        hdu = fits.PrimaryHDU(image)
        # hdu = fits.HDUList()
        # hdu.append(fits.PrimaryHDU(image))
        hdu.writeto("2.fit", overwrite="true")


if __name__ == "__main__":
    main()
