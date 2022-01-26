##
# @file main.py
# 
# @brief VUT FIT ISS projekt 2021/2022
# @date 2021-12-26
# @author F3lda
# @update 2022-01-07
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
from scipy.signal import spectrogram
from scipy.signal import find_peaks
from scipy.signal import butter
from scipy.signal import sosfilt
from scipy.signal import filtfilt
from scipy.signal import buttord
from scipy.signal import lfilter
from scipy.signal import tf2zpk
from scipy.signal import freqz
import wave
import copy



def UKOL(cislo):
    print()
    print("Ukol 4."+str(cislo))
    print("--------------------")
    pass



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


def get_column(array, index):
    return [row[index] for row in array]


###############################################################

def delka(signal_data, signal_framerate):
    return signal_data.size/float(signal_framerate)

def ustredneni(signal_data):
    return (signal_data - np.mean(signal_data))

def normalizace(signal_data):
    with np.errstate(divide='ignore', invalid='ignore'): # ignore zero division
        signal_data = signal_data / np.abs(signal_data).max()
    return signal_data

def DFT(signal_data):
    # SOURCE: https://www.youtube.com/watch?v=Xw4voABxU5c
    n = signal_data.size
    w = np.exp(2j * np.pi / n)
    
    xx, yy = np.meshgrid(np.arange(1,n+1), np.arange(1,n+1))

    dft = w**((xx-1)*(yy-1))
    dft = dft @ signal_data

    return dft

    # Python function:
    # https://docs.scipy.org/doc/scipy/reference/generated/scipy.fft.fft.html
    #return np.fft.fft(signal_data)



def main(argv):
    UKOL(1)
    ### Ukol 4.1
    ###############################################################
    # Load data
    signal_framerate, signal_data = signalLoadSCIPYIO('../audio/xjirgl01.wav')
    #signal_time_line = np.linspace(0, signal_data.size/float(signal_framerate), num=signal_data.size)
    signal_time_line = np.arange(signal_data.size) / signal_framerate
    

    # Print data
    print("Signal length: "+str(delka(signal_data, signal_framerate))+" s")
    print("Signal length: "+str(signal_data.size)+" frames")
    print("Signal max value: "+str(max(signal_data)))
    print("Signal min value: "+str(min(signal_data)))


    # Print graph
    plt.figure(figsize=(6,3))
    plt.plot(signal_time_line, signal_data)

    plt.gca().set_xlabel('$t[s]$')
    plt.gca().set_title('Zvukový signál')
    plt.gca().grid(alpha=0.5, linestyle='--')

    plt.tight_layout()

    plt.savefig("Ukol1.pdf")


    UKOL(2)
    ### Ukol 4.2
    ###############################################################
    # ustredneni
    signal_data = ustredneni(signal_data)
    
    # normalizace
    signal_data = normalizace(signal_data)


    ## FRAMES
    velikost_ramce = 1024
    polovina_ramce = (int)(velikost_ramce/2)
    cislo_ramce = 22 # 22 is nice

    # create first matrix
    signal_matrix = [np.zeros(velikost_ramce)]

    # append all frames
    for sec_sig_data in range(((int)(signal_data.size/polovina_ramce))-1):
        signal_matrix = np.append(signal_matrix, [signal_data[sec_sig_data*polovina_ramce:(sec_sig_data*polovina_ramce)+velikost_ramce]], axis=0)

    # append last not 1024 samples long frame
    if ( (float)((int)(signal_data.size/polovina_ramce)) < (signal_data.size/polovina_ramce) ):
        sig_sec_temp = copy.deepcopy(signal_data[(((int)(signal_data.size/polovina_ramce))-1)*polovina_ramce:])
        sig_sec_temp.resize((velikost_ramce), refcheck=False)
        signal_matrix = np.append(signal_matrix, [sig_sec_temp], axis=0)
        
    # remove first matrix with zeros
    signal_matrix = np.delete(signal_matrix, (0), axis=0)

    # number of frames
    print("Number of frames: "+str(signal_matrix.shape[0]))

    print("Frame size: "+str(velikost_ramce))
    print("Selected frame: "+str(cislo_ramce+1)+" ["+str(cislo_ramce)+" - index]")


    # Print graph
    plt.figure(figsize=(6,3))
    plt.plot(np.arange(signal_matrix[cislo_ramce].size) / signal_framerate + ((cislo_ramce*polovina_ramce)/signal_framerate), signal_matrix[cislo_ramce])

    plt.gca().set_xlabel('$t[s]$')
    plt.gca().set_title('Časový rámec číslo $'+str(cislo_ramce+1)+'$ zvukového signálu')
    plt.gca().grid(alpha=0.5, linestyle='--')

    plt.tight_layout()

    plt.savefig("Ukol2.pdf")


    UKOL(3)
    ### Ukol 4.3
    ###############################################################
    N = signal_matrix[cislo_ramce].size

    
    signal_dft = DFT(signal_matrix[cislo_ramce])
    G_dft = 10 * np.log10(1/N * np.abs(signal_dft)**2)

    signal_fft = np.fft.fft(signal_matrix[cislo_ramce])
    G_fft = 10 * np.log10(1/N * np.abs(signal_fft)**2)

    signal_dft_freq = np.arange(G_dft.size) / N * signal_framerate


    # Print graph
    plt.figure(figsize=(8,7))
    
    subplt_1 = plt.subplot(311)
    subplt_1.plot(np.arange(signal_matrix[cislo_ramce].size) / signal_framerate + ((cislo_ramce*polovina_ramce)/signal_framerate), signal_matrix[cislo_ramce])
    subplt_1.set_title("Segment číslo $"+str(cislo_ramce)+"$ zvukového signálu")
    subplt_1.set_xlabel('$t[s]$')
    subplt_1.grid(alpha=0.5, linestyle='--')
    
    subplt_2 = plt.subplot(312)
    subplt_2.plot(signal_dft_freq[:signal_dft_freq.size//2+1], G_dft[:G_dft.size//2+1])
    subplt_2.set_title("Spektrální hustota výkonu [dB] - DFT")
    subplt_2.set_xlabel('$f[Hz]$')
    subplt_2.grid(alpha=0.5, linestyle='--')

    subplt_3 = plt.subplot(313)
    subplt_3.plot(signal_dft_freq[:signal_dft_freq.size//2+1], G_fft[:G_fft.size//2+1])
    subplt_3.set_title("Spektrální hustota výkonu [dB] - FFT")
    subplt_3.set_xlabel('$f[Hz]$')
    subplt_3.grid(alpha=0.5, linestyle='--')
    
    plt.tight_layout()
    plt.savefig("Ukol3.pdf")

    # comparison DFT vs FFT
    print(signal_dft)
    print(signal_fft)
    print("allclose: "+str(np.allclose(signal_dft, signal_fft)))


    UKOL(4)
    ### Ukol 4.4
    ###############################################################
    f, t, sgr = spectrogram(signal_data, signal_framerate, nperseg=1024, noverlap=512)
    # prevod na PSD
    # (ve spektrogramu se obcas objevuji nuly, ktere se nelibi logaritmu, proto +1e-20)
    sgr_log = 10 * np.log10(sgr+1e-20)


    # Print graph
    plt.figure(figsize=(9,5))
    plt.pcolormesh(t,f,sgr_log, shading="gouraud", cmap="jet") # shading="auto"
    plt.gca().set_xlabel('Čas [s]')
    plt.gca().set_ylabel('Frekvence [Hz]')
    cbar = plt.colorbar()
    cbar.set_label('Spektrální hustota výkonu [dB]', rotation=270, labelpad=15)

    plt.title("Spektogram zvukového signálu")

    plt.tight_layout()
    plt.savefig("Ukol4.pdf")


    UKOL(5)
    ### Ukol 4.5
    ###############################################################
    print("INTERFERING FREQUENCIES")
    wrong_freqs = []
    
    ### My function for peaks
    for x in range(4):
        freq_from = x*1000
        freq_to = (x+1)*1000

        # get indexes and  freq values only in interval
        fx = [[i, v] for i, v in enumerate(f) if v >= freq_from and v <= freq_to]
        
        # split list to values and indexes
        indexes = [item[0] for item in fx]
        values = [item[1] for item in fx]

        # get values of power [dB] in interval from column 1
        only_fx_values = [[i, v] for i, v in enumerate(get_column(sgr_log, 1)) if i in indexes]

        # get index of max value in interval
        max_index = np.argmax([item[1] for item in only_fx_values])

        # interfering frequency
        wrong_freq = fx[max_index][1]

        print("Interval: "+str([freq_from, freq_to]))
        print("Frequency: "+str(wrong_freq))

        wrong_freqs.append(wrong_freq)

    print(wrong_freqs)
    wrong_freqs.clear()


    ### Then I found function find_peaks()
    peaks, _ = find_peaks(get_column(sgr_log, 1), height=-50)
    for ind in peaks:
        wrong_freqs.append(f[ind])

    print(wrong_freqs)


    ### Analysis
    print()
    print("ANALYSIS")

    for index, freq in enumerate(wrong_freqs):
        print("f"+str(index+1)+": "+str(freq)+", Multiple: "+str(freq/wrong_freqs[0])+", Deviation: "+str((freq/wrong_freqs[0]-(index+1))*wrong_freqs[0]))


    UKOL(6)
    ### Ukol 4.6
    ###############################################################
    signal_out = np.zeros(signal_data.size)

    time_line = np.arange(signal_data.size)/signal_framerate
    for index, freq in enumerate(wrong_freqs):
        signal_out += np.cos(2 * np.pi * freq * time_line)
    
    signal_out = ustredneni(signal_out)
    signal_out = normalizace(signal_out)
    wavfile.write('../audio/4cos.wav', signal_framerate, (signal_out * np.iinfo(np.int16).max).astype(np.int16))
    
    
    # Spectrogram
    f, t, sgr = spectrogram(signal_out, signal_framerate, nperseg=1024, noverlap=512)
    # prevod na PSD
    # (ve spektrogramu se obcas objevuji nuly, ktere se nelibi logaritmu, proto +1e-20)
    sgr_log = 10 * np.log10(sgr+1e-20)


    # Print graph
    plt.figure(figsize=(9,5))
    plt.pcolormesh(t,f,sgr_log, shading="gouraud", cmap="jet") # shading="auto"
    plt.gca().set_xlabel('Čas [s]')
    plt.gca().set_ylabel('Frekvence [Hz]')
    cbar = plt.colorbar()
    cbar.set_label('Spektrální hustota výkonu [dB]', rotation=270, labelpad=15)

    plt.title("Spektogram generovaného signálu")

    plt.tight_layout()
    plt.savefig("Ukol6.pdf")


    UKOL(7)
    ### Ukol 4.7
    ###############################################################
    clear_filters = []

    for index, freq in enumerate(wrong_freqs):
        ### FILTER - 4 band-stop filters
        stop_band_width = 30
        transition_band_width = 50
        nyq_freq = signal_framerate/2

        ripple = 3
        stop_band_attenuation = 40

        wp = [(freq - ((stop_band_width/2)+transition_band_width)) / nyq_freq, (freq + ((stop_band_width/2)+transition_band_width)) / nyq_freq]
        ws = [(freq - (stop_band_width/2)) / nyq_freq, (freq + (stop_band_width/2)) / nyq_freq]

        order, Wn = buttord(wp, ws, ripple, stop_band_attenuation, True)
        b, a = butter(order, Wn, btype='bandstop')

        clear_filters.append([b, a])

        print("Filter coefficients for f"+str(index+1)+":")
        print("A")
        print(a)
        print("B")
        print(b)


        ### IMPULS RESPONSE
        N_imp = 255
        imp = [1, *np.zeros(N_imp-1)] # jednotkovy impuls
        h = lfilter(b, a, imp)

        plt.figure(figsize=(5,3))
        plt.plot(np.arange(N_imp), h)
        plt.gca().set_xlabel('$n$')
        plt.gca().set_title('Impulsní odezva - f'+str(index+1)+" ("+str(freq)+" Hz)")

        plt.grid(alpha=0.5, linestyle='--')

        plt.tight_layout()
        plt.savefig("Ukol7_"+str(index+1)+".pdf")


    UKOL(8)
    ### Ukol 4.8
    ###############################################################
    for index, ba in enumerate(clear_filters):
        # nuly, poly
        z, p, k = tf2zpk(ba[0], ba[1])


        # Print graph
        plt.figure(figsize=(4,3.5))

        # jednotkova kruznice
        ang = np.linspace(0, 2*np.pi,100)
        plt.plot(np.cos(ang), np.sin(ang))

        # nuly, poly
        plt.scatter(np.real(z), np.imag(z), marker='o', facecolors='none', edgecolors='r', label='nuly')
        plt.scatter(np.real(p), np.imag(p), marker='x', color='g', label='póly')

        plt.gca().set_xlabel('Realná složka $\mathbb{R}\{$z$\}$')
        plt.gca().set_ylabel('Imaginarní složka $\mathbb{I}\{$z$\}$')

        plt.grid(alpha=0.5, linestyle='--')
        plt.legend(loc='upper right')

        plt.title('Nulové body a póly - f'+str(index+1)+" ("+str(wrong_freqs[index])+" Hz)")

        plt.tight_layout()
        plt.savefig("Ukol8_"+str(index+1)+".pdf")


    UKOL(9)
    ### Ukol 4.9
    ###############################################################
    for index, ba in enumerate(clear_filters):
        # frekvencni charakteristika
        w, H = freqz(ba[0], ba[1])


        # Print graph
        _, ax = plt.subplots(1, 2, figsize=(9,3))

        ax[0].plot(w / 2 / np.pi * signal_framerate, np.abs(H))
        ax[0].set_xlabel('Frekvence [Hz]')
        ax[0].set_title('Modul frekvenční charakteristiky $|H(e^{j\omega})|$')

        ax[1].plot(w / 2 / np.pi * signal_framerate, np.angle(H))
        ax[1].set_xlabel('Frekvence [Hz]')
        ax[1].set_title('Argument frekvenční charakteristiky $\mathrm{arg}\ H(e^{j\omega})$')

        for ax1 in ax:
            ax1.grid(alpha=0.5, linestyle='--')

        plt.suptitle('Frekvenční charakteristika - f'+str(index+1)+" ("+str(wrong_freqs[index])+" Hz)")

        plt.tight_layout()
        plt.savefig("Ukol9_"+str(index+1)+".pdf")


    UKOL(10)
    ### Ukol 4.10
    ###############################################################
    ### FILTRATION
    signal_filtered_ba = signal_data

    for b, a in clear_filters:
        signal_filtered_ba = filtfilt(b, a, signal_filtered_ba)

    # Spectrogram
    f, t, sgr = spectrogram(signal_filtered_ba, signal_framerate, nperseg=1024, noverlap=512)
    # prevod na PSD
    # (ve spektrogramu se obcas objevuji nuly, ktere se nelibi logaritmu, proto +1e-20)
    sgr_log = 10 * np.log10(sgr+1e-20)


    # Print graph
    plt.figure(figsize=(9,5))
    plt.pcolormesh(t,f,sgr_log, shading="gouraud", cmap="jet") # shading="auto"
    plt.gca().set_xlabel('Čas [s]')
    plt.gca().set_ylabel('Frekvence [Hz]')
    cbar = plt.colorbar()
    cbar.set_label('Spektrální hustota výkonu [dB]', rotation=270, labelpad=15)

    plt.title("Spektogram zvukového signálu po filtraci")

    plt.tight_layout()
    plt.savefig("Ukol10.pdf")

    wavfile.write("../audio/clean_bandstop.wav", signal_framerate, (signal_filtered_ba * np.iinfo(np.int16).max).astype(np.int16))


    ###############################################################
    print("Done.")
    pass

###############################################################
if __name__ == "__main__":
    try:
        main(sys.argv)
    except KeyboardInterrupt:
        #loop = False
        print('\n\nKeyboard exception received. Exiting.')
    except Exception as e:
        print("Fatal ERROR: ", e)
