
import importlib
from cv2 import data
_winrt=importlib.import_module("winrt._winrt")
_winrt.uninit_apartment()
import winrt

import cv2
import os
import random as rnd
import pandas as pd
import struct
from pytictoc import TicToc
import ble_client as ble
import input_valid as ival
import time

# Define path variables
CURRENT_PATH = os.getcwd()
# Define where are stored tasks images
STATIC_PATH = "../../static"
# Path to store already registered subjects
SUBJECTS_LIST = "subjects_id_list.txt"
# Path to baseline image
REST_PATH = "baseline.jpg"

# Define a dictionary that relates each task image with its label 
# example: Task_1.PNG: 1
MOTOR_TASKS = ['task1.jpg', 'task2.jpg', 'task3.jpg']
#MOTOR_TASKS_DIC = {MOTOR_TASKS[i].split('.')[0]: i+1 for i in range(len(MOTOR_TASKS))}
MOTOR_TASKS_DIC = {label.split('.')[0]:num+1 for num, label in enumerate(MOTOR_TASKS)}

#Number of times that all the images will be showing
TOTAL_REPETITIONS = 30

#Making sure that all images are shown the same amount of times.
assert TOTAL_REPETITIONS%len(MOTOR_TASKS) == 0,'NUM_REPETITIONS divided by the number of MOTOR_TASKS should be an even number'

# Generate  a list containing all the instances of each image task 
REPETITIONS_PER_TASK = int(TOTAL_REPETITIONS/len(MOTOR_TASKS))
MOTOR_TASKS_REPETITIONS = MOTOR_TASKS * REPETITIONS_PER_TASK
rnd.shuffle(MOTOR_TASKS_REPETITIONS)

def show_image(image_path):
    image = cv2.imread(os.path.join(STATIC_PATH, image_path))
    cv2.imshow('Image', image)
    cv2.waitKey(10)
    return -1

def get_task_name(task_path):
    return task_path.split('.')[0]


print("-- EMG DATA REGISTRATION --")
print(MOTOR_TASKS_DIC.keys())
answer = input("Welcome. What do you want to do? :\n1. Register a subject for the first time\n2. Register a new experiment for an existent subject\n3. Overwrite an experiment\nEnter an option: ")
id_subject, experiment_num = ival.give_options(os.path.join(CURRENT_PATH, SUBJECTS_LIST), answer)
print("ID_SUBJECT: {}, EXP_NUM: {}".format(id_subject, experiment_num))
#raise Exception(" ----- FIN ------")

#Because there are three motor tasks
count = [0] * len(MOTOR_TASKS)
t = TicToc()

async def run(address):
    rest1_repetition_num = 1
    rest2_repetition_num = 1
    async with ble.BleakClient(address) as client:
        global task_label

        
        cv2.namedWindow('Image', cv2.WINDOW_NORMAL)
        cv2.setWindowProperty('Image', cv2.WND_PROP_FULLSCREEN, 
                                cv2.WINDOW_FULLSCREEN)
        
        for task_path in list(MOTOR_TASKS_REPETITIONS):
            
            task_label = 0
            if show_image(REST_PATH) != -1:
                break
            await client.write_gatt_char(ble.CHAR_UUID, bytearray([4,int(id_subject),int(experiment_num),int(rest1_repetition_num)]), False)
            await client.read_gatt_char(ble.CHAR_UUID)

            await client.write_gatt_char(ble.CHAR_UUID, bytearray([5,int(id_subject),int(experiment_num),int(rest2_repetition_num)]), False)
            await client.read_gatt_char(ble.CHAR_UUID)

            rest1_repetition_num += 1
            rest2_repetition_num += 1

            task_label = MOTOR_TASKS_DIC[get_task_name(task_path)]
            count[task_label - 1] += 1
            repetition_num = count[task_label - 1]
            
            t.tic()

            if show_image(task_path) != -1:
                break
            await client.write_gatt_char(ble.CHAR_UUID, bytearray([int(task_label),int(id_subject),int(experiment_num),int(repetition_num)]), False)
            await client.read_gatt_char(ble.CHAR_UUID)
            t.toc()
            
            
            
            MOTOR_TASKS_REPETITIONS.remove(task_path)
        #await client.stop_notify(CHAR_UUID)
        cv2.destroyAllWindows()

if __name__ == "__main__":
    ble.nest_asyncio.apply()
    loop = ble.asyncio.get_event_loop()
    loop.run_until_complete(run(ble.SERVER_ADDRESS))
