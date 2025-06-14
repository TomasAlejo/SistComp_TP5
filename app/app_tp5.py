import tkinter as tk
from tkinter import messagebox
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import time
import threading

DEVICE_PATH = "/dev/signal_driver"
VALORES_MAX = 100

class SignalApp:
    def __init__(self, root):
        self.root = root
        self.root.title("TP5 - Visualización de Señales")
        self.root.geometry("300x200")

        self.signal_type = None
        self.valores = []
        self.tiempos = []
        self.inicio = None
        self.anim = None
        self.running = False
        self.fig = None
        self.ax = None
        self.linea = None

        # Botones
        tk.Button(root, text="Señal Cuadrada", command=lambda: self.configurar_y_mostrar("1")).pack(pady=10)
        tk.Button(root, text="Señal Triangular", command=lambda: self.configurar_y_mostrar("2")).pack(pady=10)
        tk.Button(root, text="Salir", command=self.salir).pack(pady=10)

    def cambiar_senal(self, numero):
        try:
            with open(DEVICE_PATH, "w") as f:
                f.write(str(numero))
        except Exception as e:
            messagebox.showerror("Error", f"No se pudo cambiar la señal:\n{e}")

    def leer_valor(self):
        try:
            with open(DEVICE_PATH, "r") as f:
                return int(f.read().strip())
        except Exception:
            return 0

    def configurar_y_mostrar(self, tipo):
        if self.running:
            messagebox.showinfo("Info", "Ya hay una señal en ejecución.")
            return

        self.signal_type = tipo
        self.cambiar_senal(tipo)
        self.valores.clear()
        self.tiempos.clear()
        self.inicio = time.time()
        self.running = True

        self.fig, self.ax = plt.subplots()
        self.linea, = self.ax.plot([], [], label=f"Señal {'Cuadrada' if tipo == '1' else 'Triangular'}")
        self.ax.set_xlim(0, 30)
        self.ax.set_xlabel("Tiempo (s)")
        self.ax.set_ylabel("Valor")
        self.ax.set_title("Visualización en tiempo real")
        self.ax.grid()
        self.ax.legend()

        if tipo == "1":
            self.ax.set_ylim(-0.5, 1.5)
        else:
            self.ax.set_ylim(-10, 110)

        self.anim = animation.FuncAnimation(self.fig, self.actualizar_grafico, interval=100)
        plt.show()
        self.running = False

    def actualizar_grafico(self, frame):
        if not self.running:
            return self.linea,

        val = self.leer_valor()
        t = time.time() - self.inicio
        self.valores.append(val)
        self.tiempos.append(t)

        if len(self.valores) > VALORES_MAX:
            self.valores.pop(0)
            self.tiempos.pop(0)

        self.linea.set_data(self.tiempos, self.valores)
        self.ax.set_xlim(max(0, t - 30), t + 1)
        self.ax.relim()
        self.ax.autoscale_view()
        return self.linea,

    def salir(self):
        self.running = False
        self.root.destroy()

if __name__ == "__main__":
    root = tk.Tk()
    app = SignalApp(root)
    root.mainloop()
