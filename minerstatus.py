#!/usr/bin/env python3
import os
import time
import json
import datetime
from subprocess import check_output

from font import pad

os.chdir("/home/makin/leddisplay_pc_esp")

def exc(cmd):
  return check_output(cmd,shell=True).decode('utf8')

def display(text):
  print(text)
  exc(f'echo "{text}" | python3 sender_dotmatrix.py')

def pad4(n):
  return pad(n,4,padc=" ")

#store share in each minute timestamp. to find hourly hashrate
share_timestamp = [0 for i in range(0,60)] #fill 60 index
#share_timestamp = [0, 1318, 1319, 1321, 1321, 1323, 1325, 1328, 1329, 1330, 1332, 1333, 1333, 1335, 1336, 1340, 1342, 1340, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1290, 1291, 1297, 1300, 1300, 1304, 1305, 1309, 1311, 1313, 1311, 1311]
#initial
with open("share_timestamp.json") as f:
  data = json.loads(f.read())
  share_timestamp = data['share_timestamp']

CH = 1.2 #constant hashrate based on current difficulty 4294967295.0. number of share per hour times CH is the MHz of hashrate
while True:
  #hashrate
  try:
    data = exc("""echo '{"id":0,"jsonrpc":"2.0","method":"miner_getstatdetail"}' | netcat -q 1 miner.local 3333""")
    data = json.loads(data)
  except:
    print("miner get status err, skip")
    time.sleep(10)
    continue

  #hashrate = int(int(data['result']['mining']['hashrate'],16)/100000) #4digit hashrate
  #display(str(hashrate))
  #time.sleep(2)
  #hourly hashrate
  menit = datetime.datetime.now().minute
  share_timestamp[menit] = data['result']['mining']['shares'][0]
  #if data is not ready, counter active under 1 hour, set initial data
  #set all zeros to current sharecount value
  for i in range(0,60):
    if share_timestamp[i]==0:
      share_timestamp[i] = share_timestamp[menit]
  #if (share_timestamp[menit+1]==0):
  #  share_timestamp[menit+1] = share_timestamp[menit] #initial data
  #  if menit==59:
  #    share_timestamp[0] = share_timestamp[menit+1]
  try:
    compared = share_timestamp[menit+1]
  except IndexError:
    compared = share_timestamp[0] #when menit+1 is over 59, flow it
  hashrate = (share_timestamp[menit] - compared)*CH
  display(pad4(int(hashrate)))
  time.sleep(1)
  #show current total shares
  display(pad4(share_timestamp[menit]))
  time.sleep(1)
  #print(share_timestamp)
  #save to file 
  with open("share_timestamp.json","w") as f:
    f.write(json.dumps({'share_timestamp':share_timestamp}))
  #gputemp
  gpu = exc("""nvidia-smi --query-gpu="temperature.gpu,power.draw" --format="csv,noheader" """)
  gpus = gpu.split("\n")
  for v in gpus:
    try:
      temp,watt = v.split(",")
      display(temp+"C ")
      time.sleep(0.7)
      watt = watt.strip()[:3]+"W"
      display(watt)
      time.sleep(0.7)
    except:print(v)
  #print(gpus)
