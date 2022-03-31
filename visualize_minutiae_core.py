import os
import numpy as np
import cv2
import math

img = cv2.imread('./5.tif')

with open('./data.txt') as f:
    data = f.readlines()

core_x = int(data[0].split(' ')[0])
core_y = int(data[0].split(' ')[1])

delta_x = int(data[1].split(' ')[0])
delta_y = int(data[1].split(' ')[1])

for i in range(2,len(data)):
    minutiae_x = int(data[i].split(' ')[0])
    minutiae_y = int(data[i].split(' ')[1])
    minutiae_angle = float(data[i].split(' ')[2])
    minutiae_type = int(data[i].split(' ')[3])
    if minutiae_type == 1:
        img = cv2.line(img,(minutiae_x,minutiae_y),(minutiae_x+int(15*np.cos(minutiae_angle*math.pi/180)),minutiae_y+int(15*np.sin(minutiae_angle*math.pi/180))),(0,0,255),1)
        img = cv2.circle(img,(minutiae_x,minutiae_y),4,(0,0,255),1)
    if minutiae_type == 2:
        img = cv2.line(img,(minutiae_x,minutiae_y),(minutiae_x+int(15*np.cos(minutiae_angle*math.pi/180)),minutiae_y+int(15*np.sin(minutiae_angle*math.pi/180))),(255,0,0),1)
        img = cv2.circle(img,(minutiae_x,minutiae_y),4,(255,0,0),1)
img = cv2.rectangle(img,(core_x-7,core_y-7),(core_x+7,core_y+7),(0,165,255),2)
img = cv2.polylines(img, [np.array([[delta_x,delta_y-8],[delta_x-8,delta_y+4],[delta_x+8,delta_y+4]])], True, (0, 255, 0), 2)
f.close()
cv2.imwrite('./test.png',img)
cv2.waitKey()