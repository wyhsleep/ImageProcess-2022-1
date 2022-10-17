# README

## 低通高斯滤波器

运行结果如下

![D0 = 40](README%2043804abda3d7429690b36154b3426dd7/outgirl256_40.bmp)

D0 = 40

![D0 = 50](README%2043804abda3d7429690b36154b3426dd7/outgirl256_50.bmp)

D0 = 50

![D0 = 60](README%2043804abda3d7429690b36154b3426dd7/outgirl256_60.bmp)

D0 = 60

![D0 =70](README%2043804abda3d7429690b36154b3426dd7/outgirl256_70.bmp)

D0 =70

可以发现D0为40时降噪效果较好，但图片整体变模糊

![D0 = 40](README%2043804abda3d7429690b36154b3426dd7/outlena256_40.bmp)

D0 = 40

![D0 = 50](README%2043804abda3d7429690b36154b3426dd7/outlena256_50.bmp)

D0 = 50

![D0 = 60](README%2043804abda3d7429690b36154b3426dd7/outlena256_60.bmp)

D0 = 60

![D0 = 70](README%2043804abda3d7429690b36154b3426dd7/outlena256_70.bmp)

D0 = 70

可以发现D0为60时降噪效果较好，lena和girl的降噪区别可能是噪声强度不同导致的。

## 中值滤波器

选用大小不同的滤波器，迭代三次进行降噪

![3*3](README%2043804abda3d7429690b36154b3426dd7/outgirl256_33t.bmp)

3*3

![5*5](README%2043804abda3d7429690b36154b3426dd7/outgirl256_55.bmp)

5*5

![7*7](README%2043804abda3d7429690b36154b3426dd7/outgirl256_77.bmp)

7*7

可以发现3*3效果最好，之后的十分模糊

![3*3](README%2043804abda3d7429690b36154b3426dd7/outlena256_33.bmp)

3*3

![5*5](README%2043804abda3d7429690b36154b3426dd7/outlena256_55.bmp)

5*5

![7*7](README%2043804abda3d7429690b36154b3426dd7/outlena256_77.bmp)

7*7

同样3*3的效果最好