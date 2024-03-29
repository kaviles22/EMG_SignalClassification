# EMG Signal Classification
## ➡️ When using this resource, please cite the original publication:
- [Avilés-Mendoza, K., Gaibor-León, N. G., Asanza, V., Lorente-Leyva, L. L., & Peluffo-Ordóñez, D. H. (2023). A 3D Printed, Bionic Hand Powered by EMG Signals and Controlled by an Online Neural Network. Biomimetics, 8(2), 255.](https://doi.org/10.3390/biomimetics8020255)
## ✅ Abstract:
About 8% of the Ecuadorian population suffers some type of amputation of upper or lower limbs. Due to the high cost of a prosthesis and the fact that the salary of an average worker in the country reached 248 USD in August 2021, they experience a great labor disadvantage and only 17% of them are employed. Thanks to advances in 3D printing and the accessibility of bioelectric sensors, it is now possible to create economically accessible proposals. This work proposes the design of a hand prosthesis that uses electromyography (EMG) signals and neural networks for real-time control. The integrated system has a mechanical and electronic design, and the latter integrates artificial intelligence for control. To train the algorithm, an experimental methodology was developed to record muscle activity in upper extremities associated with specific tasks, using three EMG surface sensors. These data were used to train a five-layer neural network. the trained model was compressed and exported using TensorflowLite. The prosthesis consisted of a gripper and a pivot base, which were designed in Fusion 360 considering the movement restrictions and the maximum loads. It was actuated in real time thanks to the design of an electronic circuit that used an ESP32 development board, which was responsible for recording, processing and classifying the EMG signals associated with a motor intention, and to actuate the hand prosthesis. As a result of this work, a database with 60 electromyographic activity records from three tasks was released. The classification algorithm was able to detect the three muscle tasks with an accuracy of 78.67% and a response time of 80 ms. Finally, the 3D printed prosthesis was able to support a weight of 500 g with a safety factor equal to 15.


<p align="center">
<img src="https://github.com/kaviles22/EMG_SignalClassification/blob/main/static/case.jpg" width="40%" height="30%"/> <img src="https://github.com/kaviles22/EMG_SignalClassification/blob/main/static/render.png" width="30%" height="20%"/>
</p>

## Introduction
This project aims to preprocess EMG signals and classify them into three classes using AI techniques to control a 3d printed prosthesis.
<p align="center">
<img src="https://github.com/kaviles22/EMG_SignalClassification/blob/main/static/summary.png"  width="80%" height="70%"/>
</p>

### Tools:
- [ESP32 C3](https://www.espressif.com/en/products/socs/esp32-c3)
- [MyoWare sensors](https://cdn.sparkfun.com/datasheets/Sensors/Biometric/MyowareUserManualAT-04-001.pdf)
- Servo motors
- [SD card module](https://create.arduino.cc/projecthub/electropeak/sd-card-module-with-arduino-how-to-read-write-data-37f390)
- Computer

<img src= "https://github.com/kaviles22/EMG_SignalClassification/blob/main/static/circuit.png" width="50%" height="40%"/>

## Data collection
[src/data_collection](https://github.com/kaviles22/EMG_SignalClassification/blob/main/src/data_collection)
<p align="center">
<img src= "https://github.com/kaviles22/EMG_SignalClassification/blob/main/static/data_collect.png" width="80%" height="70%"/>
</p>

For this stage, a computer, running a python script and an Esp32c3, communicated using bluetooth low energy (BLE). The computer was in charge of indicating the tests subject when to start performing a certain muscular action. The esp32c3 was in charge of recording the emg data using 3 MyoWare sensors, labeling it and saving it on an SD card.

Note: the ideal would have been to send the data via bluetooth to the computer, however we were unable to find a way to do this quickly without affecting the data recording process which was done at a frequency of 1kHz. 

<p align="center">
<img src= "https://github.com/kaviles22/EMG_SignalClassification/blob/main/static//ble_communication.png" width="80%" height="70%"/>
</p>

### Screen visuals
The python script shows visual signs on the screen, representing each one of the actions. Between each recording interval, a resting sign appears, which means the test subject can stop performing the previous action.
![Screen visuals](static/screen_visuals.png)

## Preprocessing
Preprocessing data is an important step, specially when working with biological signals.

<img src= "https://github.com/kaviles22/EMG_SignalClassification/blob/main/static/myoware_signals.jpeg" width="50%" height="40%"/>

### Filtering
To filter out the noise we used the RMS Envelope technique, which was calculated in a 50 ms wide window.
<img src= "https://github.com/kaviles22/EMG_SignalClassification/blob/main/static/filtered_signal.png" width="50%" height="40%"/>

### Normalization
The peak dynamic method was used, which consists on representing each signal as a ratio of the peak value of that time window. Values were kept between [0, 1].
 <br />
 <br />
![equation1](https://latex.codecogs.com/svg.image?X_{norm}=&space;\frac{X}{X_{peak}})

### Feature extraction
[src/feature_extraction/feature_extraction.ipynb](https://github.com/kaviles22/EMG_SignalClassification/blob/main/src/feature_extraction/feature_extraction.ipynb)<br />
Two approaches were analyzed:
1. Statistical features
2. RMS in time windows: extracting the rms value in subwindows.

## Training and converting the TF model to a TF Lite model
[src/train_model/train_test_model.py](https://github.com/kaviles22/EMG_SignalClassification/blob/main/src/train_model/train_test_model.py)<br />
A tensorflow model was trained and then converted into a TF Lite model. 

## Deploying model and testing it on real-time.
The whole pipeline was deployed on a ESP-C3 development board. The code was written in C++ and the model output was used to activate 2 servo motors to move the 3d printed prosthesis. 
![Real time testing](static/real_time.png)

