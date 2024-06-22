import math
import numpy as np
import concurrent.futures

def Sinc(w):
    if w == 0.0:
        return 1.0
    return math.sin(w * math.pi) / (w * math.pi)

def SyncConvolution(arr, sync_ratio):
    num_partials = len(arr)
    out = []
    for partial_idx in range(1, num_partials + 1): 
        gain = 0.0
        for i in range(-num_partials, num_partials + 1):
            if(i == 0): # DC
                continue

            spec_gain = 0.0
            if(i > 0):
                spec_gain = arr[i - 1]
            if(i < 0):
                spec_gain = -arr[-(i + 1)] # mirror table

            w = partial_idx - i * sync_ratio
            sinc_v = Sinc(w)

            tmp = spec_gain * sinc_v
            gain += tmp
        
        if(gain < 1e-5): # small than -100dB
            gain = 0.0

        out.append(gain)
    return out

def SyncTable(arr, max_sync_ratio, table_interval):
    out = []
    num_interval = int(round(1.0 / table_interval))
    table_interval = 1.0 / num_interval

    for ratio_begin in range(1, max_sync_ratio):
        print(f"{ratio_begin} / {max_sync_ratio}")
        for i in range(0, num_interval):
            ratio = ratio_begin + table_interval * i
            out.append(SyncConvolution(arr, ratio))
    
    out.append(SyncConvolution(arr, max_sync_ratio))

    return out


def OutputFile(table_name, sync_table, file_handle):
        print(table_name+": done!")
        file_handle.write(f'static constexpr auto {table_name} = std::array{{\n')

        for single_spectrum in sync_table:
            file_handle.write('std::array{')
            file_handle.write(','.join(str(num) + 'f' for num in single_spectrum))
            #file_handle.write(','.join(str(num) for num in single_spectrum))
            file_handle.write('},\n')

        file_handle.write('};\n')


def SingleSquare(idx):
    if(idx % 2 == 0):
        return 1.0 / (idx + 1.0)
    else:
        return 0.0
    
def SingleTri(idx):
    idx = idx + 1

    if(idx % 2 == 0):
        return 0.0
    
    x2 = (idx - 1) // 2
    sign = 1.0
    if(x2 % 2 == 1):
        sign = -1.0
    pi2 = np.pi * np.pi
    return 8.0 * sign / idx / idx / pi2

# parameter
num_bins = 256
max_octave = 6
table_interval = 0.1

max_ratio = int(round(math.exp2(max_octave)))
sine_table = [1.0] + (num_bins - 1) * [0.0]
saw_table = [1.0 / (i + 1.0) for i in range(0, num_bins)]
square_table = [SingleSquare(i) for i in range(0, num_bins)]
tri_table = [SingleTri(i) for i in range(0, num_bins)]

with open('sync_table.h', 'w+') as f:
    f.write('#include <array>\n\n')
    OutputFile('kSineTable', SyncTable(sine_table, max_ratio, table_interval), f)
    OutputFile('kTriTable', SyncTable(tri_table, max_ratio, table_interval), f)
    OutputFile('kSquareTable', SyncTable(square_table, max_ratio, table_interval), f)
    OutputFile('kSawTable', SyncTable(saw_table, max_ratio, table_interval), f)
