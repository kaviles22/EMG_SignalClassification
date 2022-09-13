from bleak import BleakClient
import asyncio
import nest_asyncio

SERVER_ADDRESS = "7c:df:a1:5b:9a:3e"                #Ble server MAC address
CHAR_UUID ="beb5483e-36e1-4688-b7f5-ea07361b26a8"   #Characteristic UNIQUE ID (in order to access ble server readings)
ADC_BUFFER_U32 = 50  