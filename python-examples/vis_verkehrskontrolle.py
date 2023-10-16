import sys
import pygame
from random import randint
import threading
import os

def get_time(line):
    return int(line.split(', ')[0].split('.')[0])

lidar_top_file = open('data/lidar_top.txt', 'r')
lidar_top_lines = lidar_top_file.readlines()
lidar_top_lines.pop(0)
lidar_top_lines = [s + ', 0' for s in lidar_top_lines]
lidar_mid_file = open('data/lidar_mid.txt', 'r')
lidar_mid_lines = lidar_mid_file.readlines()
lidar_mid_lines.pop(0)
lidar_mid_lines = [s + ', 1' for s in lidar_mid_lines]
lidar_bot_file = open('data/lidar_bottom.txt', 'r')
lidar_bot_lines = lidar_bot_file.readlines()
lidar_bot_lines.pop(0)
lidar_bot_lines = [s + ', 2' for s in lidar_bot_lines]

img_files = os.listdir('data/camera')
img_files = [s + ', , , , 3' for s in img_files]

lines = lidar_top_lines + lidar_mid_lines + lidar_bot_lines + img_files
lines.sort(key=get_time)

pygame.init()

screen = pygame.display.set_mode((1400, 900))
FPSCLOCK = pygame.time.Clock()
RED = pygame.Color("red")
startpoint_0 = pygame.math.Vector2(0, 150)
startpoint_1 = pygame.math.Vector2(0, 450)
startpoint_2 = pygame.math.Vector2(0, 750)
endpoint = pygame.math.Vector2(200, 0)
angle = 0
done = False
prev_timestamp = 0
prev_angle_0=0
prev_angle_1=0
prev_angle_2=0

for line in lines:
    line_split = line.split(', ')
    type = line_split[4]
    current_timestamp = get_time(line)
    if int(type) == 3:
        imp = pygame.image.load('data/camera/' + line_split[0]).convert()
        screen.blit(imp, (400, 0))

        pygame.display.flip()
        if(prev_timestamp!=0):
            time_diff = current_timestamp - prev_timestamp + 1
            FPSCLOCK.tick(1000/time_diff)
            prev_timestamp = current_timestamp
        else:
            prev_timestamp = current_timestamp
            FPSCLOCK.tick(30)
    elif line_split[0] != 'Time' and ((float(line_split[1]) > 300 and float(line_split[1]) <360) or float(line_split[1]) < 40):
        current_timestamp = int(line_split[0])
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                done = True

        if int(type) == 0:
            current_startpoint = startpoint_0
            current_prev_angle = prev_angle_0
            rect_top = 0
        elif int(type) == 1:  
            current_startpoint = startpoint_1
            current_prev_angle = prev_angle_1
            rect_top = 300
        else:
            current_startpoint = startpoint_2
            current_prev_angle = prev_angle_2
            rect_top = 600
        # % 360 to keep the angle between 0 and 360.
        angle = float(line_split[1]) % 360
        # The current endpoint is the startpoint vector + the
        # rotated original endpoint vector.
        endpoint = pygame.math.Vector2(min(float(line_split[2])/10,250.0), 0)
        current_endpoint = current_startpoint + endpoint.rotate(angle)

        if(angle-current_prev_angle>10):
            pygame.draw.rect(screen,(0,0,0),(0,rect_top,300,300))
        if int(type) == 0:
            prev_angle_0 = angle
        elif int(type) == 1:  
            prev_angle_1 = angle
        else:
            prev_angle_2 = angle
        pygame.draw.line(screen, (0,0,0), current_startpoint, current_endpoint, 2)
        pygame.draw.line(screen, RED, current_startpoint, current_endpoint, 2)

        pygame.display.flip()
        if(prev_timestamp!=0):
            time_diff = current_timestamp - prev_timestamp + 1
            FPSCLOCK.tick(1000/time_diff)
            prev_timestamp = current_timestamp
        else:
            prev_timestamp = current_timestamp
            FPSCLOCK.tick(30)

pygame.quit()
sys.exit()