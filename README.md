# EMG Signal Classification
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

For this stage, a computer, running a python script and an Esp32c3, communicated using bluetooth low energy (BLE). The computer was in charge of showing on the screen when the test subject had to start performing a certain muscular action. The esp32c3 was in charge of recording the emg data using 3 MyoWare sensors, labeling it and saving it on an SD card.

<p align="center">
<img src= "https://github.com/kaviles22/EMG_SignalClassification/blob/main/static//ble_communication.png" width="80%" height="70%"/>
</p>

### Screen visuals
The experiment shows visual signs to let the subject jnow when to perform certain actions.
![Screen visuals](static/screen_visuals.png)

Note: the ideal would be to send the data via bluetooth to the laptop, however we were unable to find a way to do this quickly without affecting the data recording which was done at 1kHz. 

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
The whole pipeline was deployed on a ESP-C3 development board. The code was written in C++ and the model output was used to activate the servo motors to move a prosthesis. 
![Real time testing](static/real_time.png)

