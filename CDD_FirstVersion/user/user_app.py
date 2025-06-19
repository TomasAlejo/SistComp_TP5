import matplotlib.pyplot as plt
import time
import struct
import fcntl
import os

DEVICE = "/dev/cdd_signals"
IOCTL_SELECT_SIGNAL = 0x40046101  # _IOW('a', 1, int)

def select_signal(sig_type):
    with open(DEVICE, 'wb') as f:
        fcntl.ioctl(f, IOCTL_SELECT_SIGNAL, sig_type)

def read_signal():
    with open(DEVICE, 'rb') as f:
        raw = f.read(4)
        return struct.unpack('i', raw)[0]

def main():
    sig = int(input("Seleccionar señal (0 = sinusoidal, 1 = triangular): "))
    select_signal(sig)

    x, y = [], []
    plt.ion()
    fig, ax = plt.subplots()

    for t in range(30):
        val = read_signal()
        print(f"[{t}s] Valor leído: {val}")
        x.append(t)
        y.append(val)

        ax.clear()
        ax.plot(x, y, label=f"Señal {sig}")
        ax.set_xlabel("Tiempo (s)")
        ax.set_ylabel("Valor")
        ax.set_title("Simulación de Señal")
        ax.grid(True)
        ax.legend()
        plt.pause(1)

if __name__ == "__main__":
    main()
