import serial
import time
from datetime import datetime
from datetime import timedelta  
import csv
import argparse

def __main__(args):
        csvwriter = csv.DictWriter(args.csvfile,["start of batch","time","value"])
        csvwriter.writeheader()
        with serial.Serial(args.tty.name, 9600, timeout=None) as ser:
            time.sleep(2)
            while True:
                ser.write(b'f\n')
                values = ser.readline().decode("utf8").rstrip(',\r\n').split(",")
                for i in range(0,len(values)):
                    reading_time = datetime.now() - timedelta(milliseconds=250*(len(values)-i-1))
                    csvwriter.writerow({"start of batch":i==0,"time":reading_time.isoformat(),"value":values[i]})
                time.sleep(2)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--csvfile', type=argparse.FileType('w'))
    parser.add_argument('tty', type=argparse.FileType('r'))
    args = parser.parse_args()
    __main__(args)
    