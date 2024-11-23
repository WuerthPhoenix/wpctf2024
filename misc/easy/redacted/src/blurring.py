import numpy as np
import cv2
from scipy.ndimage import convolve

from skimage import img_as_float, util,io
from skimage.color import rgb2gray

I = rgb2gray(img_as_float(io.imread('src.png')))

radius = 15
disk_size = 2 * radius + 1
y, x = np.ogrid[-radius:radius+1, -radius:radius+1]
disk = (x**2 + y**2) <= radius**2
PSF = disk / disk.sum()


Blurred = convolve(I, PSF, mode='wrap')
Blurred = util.random_noise(Blurred, mode='gaussian', mean=0.9, var=0.0001)

# save img 
cv2.imwrite('dst.png', Blurred * 255)



