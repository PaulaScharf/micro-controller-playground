from pynput.mouse import Listener

# program to capture single image from webcam in python
  
# importing OpenCV library
import cv2

import datetime;
import time
  
# initialize the camera
# If you have multiple camera connected with 
# current device, assign a value in cam_port 
# variable according to that
cam_port = 0
cam = cv2.VideoCapture(cam_port)
  
num_picts = 10
interval = 0.1


def on_click(x, y, button, pressed):
    for i in range(num_picts):
        result, image = cam.read()
    
        # If image will detected without any error, 
        # show result
        if result:    
            # saving image in local storage
            cv2.imwrite("/home/xxx/" + str(datetime.datetime.now()) + ".png", image)
        # If captured image is corrupted, moving to else part
        else:
            print("No image detected. Please! try again")
        time.sleep(interval)

with Listener(on_click=on_click) as listener:
    listener.join()