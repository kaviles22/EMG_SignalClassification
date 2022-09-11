from numpy import random
import tensorflow as tf

from tensorflow import keras
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import math
import time

from tensorflow.keras.models import Sequential
from keras.layers import Input, Dense, Dropout

import sklearn
import os

""""
This is an experimental script. The code written here was used to try different models and
tune different hyperparameters.

Authors: Karla Aviles & George Gaibor
"""

# Define training hyperparameters
TRAIN_PERC = 0.6
TEST_VALID_PERC = (1-TRAIN_PERC)/2 
LEARNING_RATE = 0.01
EPOCHS = 300
BATCH_SIZE = 32

# Define paths for saving trained and converted model
MODELS_DIR = '../models/'
PLOT_DIR = '../plots/'
if not os.path.exists(MODELS_DIR):
    os.mkdir(MODELS_DIR)
MODEL_TF = MODELS_DIR + 'model'
MODEL_TFLITE = MODELS_DIR + 'model.tflite'

# Read features from csv files
df_features = pd.read_csv(os.path.join("../Features/Features_dobleExt.csv"))

# Get number of classes. Make sure the classes start from 0.
num_classes = len(df_features["Label"].unique())

# Prepare features data for training
df_features.dropna(inplace=True)
df_features['Label'] = df_features['Label'].map(int)
df_features = df_features.sample(frac=1)
df_features = df_features.to_numpy()

# Separate labels from features
y_values = keras.utils.to_categorical(df_features[:,0])
x_values = df_features[:,1:].astype('float32')

# Split data into training, validation and testing dataq
TRAIN_SPLIT =  int(TRAIN_PERC * df_features.shape[0])
TEST_SPLIT = int(TEST_VALID_PERC * df_features.shape[0] + TRAIN_SPLIT)
x_train, x_test, x_validate = np.split(x_values, [TRAIN_SPLIT, TEST_SPLIT])
y_train, y_test, y_validate = np.split(y_values, [TRAIN_SPLIT, TEST_SPLIT])
assert(x_train.shape[0] + x_test.shape[0] + x_validate.shape[0] == df_features.shape[0])

# Define model
model = Sequential()
model.add(Dense(30, input_shape = (x_train.shape[1],), activation='relu'))
#model.add(Dropout(0.2))
#model.add(Dense(20, activation='relu'))
#model.add(Dropout(0.2))
model.add(Dense(20, activation='relu'))
#model.add(Dropout(0.2))
model.add(Dense(10,activation='relu'))
#model.add(Dropout(0.2))
model.add(Dense(num_classes,activation='softmax'))

# Train model
optimizer = tf.keras.optimizers.Adam(LEARNING_RATE)
model.compile(optimizer=optimizer, loss='categorical_crossentropy', metrics=['accuracy', tf.keras.metrics.Recall(), tf.keras.metrics.Precision()])
start = time.time()
history = model.fit(x_train, y_train, epochs=EPOCHS, batch_size=BATCH_SIZE,
                            validation_data=(x_validate, y_validate))
stop = time.time()
print(f"Training time: {stop - start}s")
test_results = model.evaluate(x_test, y_test, verbose=1)

if not os.path.exists(PLOT_DIR):
    os.mkdir(PLOT_DIR)

# Plot accuracy history
plt.figure(0)
plt.plot(history.history['accuracy'])
plt.plot(history.history['val_accuracy'])
plt.title('model accuracy')
plt.ylabel('accuracy')
plt.xlabel('epoch')
plt.legend(['train', 'test'], loc='upper left')
plt.savefig(os.path.join(PLOT_DIR,'accuracy.jpg'))
plt.show()

# Plot loss history
plt.figure(1)
plt.plot(history.history['loss'])
plt.plot(history.history['val_loss'])
plt.title('model loss')
plt.ylabel('loss')
plt.xlabel('epoch')
plt.legend(['train', 'test'], loc='upper left')
plt.savefig(os.path.join(PLOT_DIR,'loss.jpg'))
plt.show()

# Save Tensorflow model
model.save(MODEL_TF)

# Convert the model to the TensorFlow Lite format without quantization
converter = tf.lite.TFLiteConverter.from_saved_model(MODEL_TF)
model_no_quant_tflite = converter.convert()

# Save the model to disk
open(MODEL_TFLITE, "wb").write(model_no_quant_tflite)

# Convert to a C source file, i.e, a TensorFlow Lite for Mic+rocontrollers model
# (Run this command in Linux or in WSL)
#xxd -i {MODEL_NO_QUANT_TFLITE} > {MODEL_TFLITE_MICRO}

