##
# @file main.py
# 
# @brief VUT FIT ISS projekt 2020/2021
# @date 2020-12-26
# @author F3lda
# @update 2021-01-04
##
# Good to read about pyplot: https://stackoverflow.com/questions/18625085/how-to-plot-a-wav-file
import sys
import math
import os
import matplotlib
if os.environ.get('DISPLAY','') == '':
    #print('No display found! Using non-interactive Agg backend.')
    #matplotlib.use('Agg')
    pass
matplotlib.use('Agg') # setup non-interactive backend
import matplotlib.pyplot as plt
import numpy as np
from scipy.io import wavfile
import wave
import copy



def signalLoadWAVE(filename):
    wav_file = 0
    try:
        wav_file = wave.open(filename, "rb")
    except:
        print("Couldn't read file: " + filename)
        sys.exit(0)

    if wav_file.getnchannels() != 1:
        print("Only mono files!")
        sys.exit(0)

    # Extract Raw Audio from Wav File
    signal_framerate = wav_file.getframerate()
    signal_data = wav_file.readframes(-1)
    signal_data = np.frombuffer(signal_data, dtype='int16')
    return signal_framerate, signal_data


def signalLoadSCIPYIO(filename):
    signal_framerate = 0
    signal_data = 0
    try:
        signal_framerate, signal_data = wavfile.read(filename)
    except:
        print("Couldn't read file: " + filename)
        sys.exit(0)
    return signal_framerate, signal_data


def signalCut(signal_data, signal_start, signal_end): # 0; fs; signal_data.size;
    signal_size = signal_end-signal_start

    graph = np.zeros(signal_size)
    signal_data = signal_data[signal_start:signal_end]
    graph[0:signal_size] = signal_data
    return graph


def signalPrintGraph(filename, signals_data, signal_labels, signal_framerate, signal_start, signal_end):
    plt.figure(figsize=(8,3))
    
    i = 0
    for signal_data in signals_data:
        time_line = np.linspace(0, signal_data.size/float(signal_framerate), num=signal_data.size)
        plt.plot(time_line, signal_data, label=signal_labels[i])#plt.plot(time_line, signal_data2, color='red', linewidth=3)
        i = i + 1
    
    plt.title("Signal o delce "+str(((signal_end/float(signal_framerate))-(signal_start/float(signal_framerate)))*1000)+" ms")
    #plt.gca().set_ylabel('$signal$')
    if(signal_start == 0 and signal_end == 0):
        plt.gca().set_xlabel('$t[s]$')
    else:
        plt.gca().set_xlabel('$t[s] - originalFile - start: '+str(signal_start/float(signal_framerate))+' [s], end: '+str(signal_end/float(signal_framerate))+' [s]$')
    plt.legend(loc="upper left")
    plt.tight_layout()
    
    plt.savefig(filename)
    pass



def ustredeni(signal_data):
    return (signal_data - np.mean(signal_data))

def normalizace(signal_data):
    with np.errstate(divide='ignore', invalid='ignore'): # ignore zero division
        signal_data = signal_data / np.abs(signal_data).max()
    return signal_data



def Ukol3():
    # Load data
    signal_framerate, signal_data = signalLoadSCIPYIO('../audio/maskoff_tone.wav')
    signal_framerate, signal_data2 = signalLoadSCIPYIO('../audio/maskon_tone.wav')
    
    
    signal_start = signal_framerate
    signal_end = signal_framerate+320
    
    
    # Cut signal
    signal_data = signalCut(signal_data, signal_start, signal_end)
    signal_data2 = signalCut(signal_data2, signal_start+3, signal_end+3)# srovnani signalu
    
    
    
    # ustredneni
    signal_data = ustredeni(signal_data)
    signal_data2 = ustredeni(signal_data2)

    # normalizace
    signal_data = normalizace(signal_data)
    signal_data2 = normalizace(signal_data2)
    
    
    
    # Print graph
    signals_data = [signal_data, signal_data2]
    signal_labels = ["maskoff_tone", "maskon_tone"]
    signalPrintGraph("Ukol3.pdf", signals_data, signal_labels, signal_framerate, signal_start, signal_end)
    pass
    

def Ukol4():
    # Load data
    signal_framerate, signal_data = signalLoadSCIPYIO('../audio/maskoff_tone.wav')
    signal_framerate, signal_data2 = signalLoadSCIPYIO('../audio/maskon_tone.wav')
    
    
    # Cut signal
    signal_start = signal_framerate
    signal_end = signal_framerate+320
    signal_data = signalCut(signal_data, signal_start, signal_end)
    signal_data2 = signalCut(signal_data2, signal_start, signal_end)
    
    
    
    signal_data_raw = copy.deepcopy(signal_data)
    signal_data2_raw = copy.deepcopy(signal_data2)
    
    
    
    # ustredneni
    signal_data = ustredeni(signal_data)
    signal_data2 = ustredeni(signal_data2)
    
    # normalizace
    signal_data = normalizace(signal_data)
    signal_data2 = normalizace(signal_data2)
    
    
    
    signal_data_old = copy.deepcopy(signal_data)
    signal_data2_old = copy.deepcopy(signal_data2)
    
    
    
    # Center Clipping
    signal_absolute_max = np.abs(signal_data).max()*0.7
    signal_absolute_min = signal_absolute_max*-1
    
    for index, signal in enumerate(signal_data):
        if(np.abs(signal) == signal): # kladny
            if(signal > signal_absolute_max):
                signal_data[index] = 1
            else:
                signal_data[index] = 0
        else: # zaporny
            if(signal < signal_absolute_min):
                signal_data[index] = -1
            else:
                signal_data[index] = 0
    
    
    
    # Print graph
    plt.figure(figsize=(8,5))
    
    subplt_1 = plt.subplot(211)
    time_line = np.linspace(0, signal_data_old.size/float(signal_framerate), num=signal_data_old.size)
    subplt_1.plot(time_line, signal_data_old, label="maskoff_tone")
    subplt_1.set_title("Ramec")
    #subplt_1.set_ylabel('$signal$')
    subplt_1.set_xlabel('$t[s]$')
    subplt_1.legend(loc="upper left")
    
    subplt_2 = plt.subplot(212)
    time_line = np.linspace(0, signal_data.size/float(signal_framerate), num=signal_data.size)
    subplt_2.plot(time_line, signal_data, label="maskoff_tone")
    subplt_2.set_title("Centralni klipovani s 70 %")
    #subplt_2.set_ylabel('$signal$')
    subplt_2.set_xlabel('$t[s]$')
    subplt_2.legend(loc="upper left")
    
    plt.tight_layout()
    plt.savefig("Ukol4.pdf")
    pass


def main(argv):
    Ukol3()
    Ukol4()
    print("Done.");
    pass

###############################################################
if __name__ == "__main__":
    try:
        main(sys.argv)
    except KeyboardInterrupt:
        #loop = False;
        print('\n\nKeyboard exception received. Exiting.');
    except Exception as e:
        print("Fatal ERROR: ", e);
