import cv2
from PIL import Image
im = cv2.imread('sample.png')   #读取图片rgb 格式<class 'numpy.ndarray'>
image = Image.fromarray(cv2.cvtColor(im,cv2.COLOR_BGR2RGB))  #格式转换，bgr转rgb
image.save('sampleppi500.jpg',dpi=(500.0,500.0))    #存成jpg能改ppi，存成png就不能