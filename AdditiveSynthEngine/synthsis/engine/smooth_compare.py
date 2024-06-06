import math
import random
import numpy as np
import matplotlib.pyplot as plt
import scipy.fft as fft
import scipy.signal as s

fs = 48000
speed = 200
fft_size = 32768
out_len = fft_size // 2 + 1


update_div = 32
update_rate = fs / update_div
update_len = fft_size // update_div

update_sign = []
for i in range(update_len):
    update_sign.append(#math.sin(math.pi / 32 * i)
                       #+ math.sin(math.pi / 16 * i)
                       #+ math.sin(math.pi / 8 * i)
                       #+ math.sin(math.pi / 4 * i)
                       + math.sin(math.pi / 2 * i)
                       #1.0
                       #random.uniform(-1,1)
    )

# J_0(y)
display_len = 1000
plt.figure(figsize=(8, 6))

def Gain2Db(x):
    x = np.abs(x)
    return np.where(x <= 0.0001, -60, 20 * np.log10(x)) 

def Normalize(x):
    vmin = np.max(x)
    gain = 1.0 / vmin
    return x * gain

def FftSign(input, name):
    input = Normalize(np.array(input))
    fft_out = fft.fft(input)[:out_len]

    fft_amp = np.abs(fft_out) / (fft_size / 2)
    fft_freq = np.linspace(0, fs / 2, out_len)
    plt.subplot(1,2,1)
    #plt.plot(fft_freq, Gain2Db(fft_amp), label = name)
    plt.plot(fft_freq, fft_amp, label = name)
    plt.subplot(1,2,2)
    plt.plot(np.arange(update_div * 16), input[:update_div * 16], label = name)

# none
zero_input_sign = np.zeros(fft_size)
for i in range(update_len):
    zero_input_sign[i * update_div] = update_sign[i] * update_div
#FftSign(zero_input_sign, 'none')

# zero keep
keep_sign = []
for i in range(update_len):
    for j in range(update_div):
        keep_sign.append(update_sign[i])
#FftSign(keep_sign, 'keep')

# linner
cos_sign = []
cos_last = 0
for i in range(update_len):
    cos_curr = update_sign[i]
    for j in range(update_div):
        cos_sign.append(cos_last + (cos_curr - cos_last) * j / update_div)
    cos_last = cos_curr
#FftSign(cos_sign, 'linear_interp')

# cos
cos_sign = []
cos_last = 0
cos_table = 0.5 - 0.5 * np.cos(np.linspace(0, np.pi, update_div))
for i in range(update_len):
    cos_curr = update_sign[i]
    for j in range(update_div):
        cos_sign.append(cos_last + (cos_curr - cos_last) * cos_table[j])
    cos_last = cos_curr
FftSign(cos_sign, 'cos_interp')

# sinc
sinc_sign = []
for i in range(update_len):
    for j in range(update_div):
        m = i + j / update_div
        tmp_x = m - np.arange(update_len)
        tmp_sum = np.inner(np.sinc(tmp_x), update_sign)
        sinc_sign.append(tmp_sum)
FftSign(sinc_sign, 'sinc_interp')

# fir convolution
filter_order = update_div * 3
filter_len = filter_order + 1
fir_cut_f = np.pi * update_rate / 2 / fs
tmp_n = np.arange(filter_len) - filter_order / 2
hann_window = 0.5 * (1.0 - np.cos(np.pi * 2.0 * np.arange(filter_len) / filter_order))
fir_coef = np.where(tmp_n == 0, fir_cut_f / np.pi, np.sin(fir_cut_f * tmp_n) / (tmp_n * np.pi))
fir_coef = fir_coef * hann_window * 2
fir_fifo = np.zeros(filter_len)

def FirPushSample(x):
    for i in range(filter_len - 1):
        fir_fifo[filter_len - i - 1] = fir_fifo[filter_len - i - 2]
    fir_fifo[0] = x

def FirConvolution():
    return np.inner(fir_fifo, fir_coef)

fir_sign = []
for i in range(fft_size):
    FirPushSample(zero_input_sign[i])
    fir_sign.append(FirConvolution())

FftSign(fir_sign, 'fir_filter')

# polyphase fir convolution
filter_order = update_div * 2
filter_len = filter_order + 1
fir_cut_f = np.pi * update_rate / 1 / fs
tmp_n = np.arange(filter_len) - filter_order / 2
hann_window = 0.5 * (1.0 - np.cos(np.pi * 2.0 * np.arange(filter_len) / filter_order))
shared_fir_coef = np.where(tmp_n == 0, fir_cut_f / np.pi, np.sin(fir_cut_f * tmp_n) / (tmp_n * np.pi))
shared_fir_coef = shared_fir_coef * hann_window
#fir_coef = shared_fir_coef * filter_order/2
#fir_fifo = np.zeros(2)

#fir_sign = []
#for i in range(update_len):
#    curr = update_sign[i]
#    for j in range(update_div):
#        if j == 0:
#            fir_sign.append(curr * fir_coef[2 * update_div]
#                            + fir_fifo[0] * fir_coef[1 * update_div]
#                            + fir_fifo[1] * fir_coef[0 * update_div])
#        else:
#            fir_sign.append(curr * fir_coef[2 * update_div - j]
#                            + fir_fifo[0] * fir_coef[1 * update_div - j])
#    fir_fifo[1] = fir_fifo[0]
#    fir_fifo[0] = curr

#FftSign(np.array(fir_sign), 'polyphase_fir_filter')

# polyphase 2 fir
fir_coef = list(reversed(list(shared_fir_coef)))
fir_coef0 = []
fir_coef1 = []
fir_coef2 = []
#fir_coef3 = []
for i in range(update_div):
    temp_sum = 0
    for j in range(0, i + 1):
        temp_sum += fir_coef[j]
    fir_coef0.append(temp_sum)

    temp_sum = 0
    for j in range(i + 1, update_div + i + 1):
        temp_sum += fir_coef[j]
    fir_coef1.append(temp_sum)

    temp_sum = 0
    #for j in range(update_div + 1 + i, 2 * update_div + i + 1):
    for j in range(update_div + 1 + i, 2 * update_div + 1):
        temp_sum += fir_coef[j]
    fir_coef2.append(temp_sum)

    #temp_sum = 0
    #for j in range(update_div * 2 + i + 1, 3 * update_div + 1):
    #    temp_sum += fir_coef[j]
    #fir_coef3.append(temp_sum)

fir_fifo = np.zeros(3)
def FirPushSample(x):
    #fir_fifo[3] = fir_fifo[2]
    fir_fifo[2] = fir_fifo[1]
    fir_fifo[1] = fir_fifo[0]
    fir_fifo[0] = x

fir_pos = 0
def FirConvolution():
    return fir_fifo[0] * fir_coef0[fir_pos] \
         + fir_fifo[1] * fir_coef1[fir_pos] \
         + fir_fifo[2] * fir_coef2[fir_pos]
         #+ fir_fifo[3] * fir_coef3[fir_pos]

fir_sign = []
for i in range(update_len):
    curr = update_sign[i]
    fir_pos = 0
    FirPushSample(curr)
    for j in range(update_div):
        fir_sign.append(FirConvolution())
        fir_pos = fir_pos + 1

FftSign(np.array(fir_sign), 'polyphase_fir_filter2')


#plt.ylim(-1,1)
plt.plot()
plt.legend()
plt.grid(True)
plt.show()
