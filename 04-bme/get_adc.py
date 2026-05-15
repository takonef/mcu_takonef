import time
import serial
import matplotlib.pyplot as plt


def read_value(ser, timeout=2.0):
    start_time = time.time()
    while time.time() - start_time < timeout:
        try:
            if ser.in_waiting > 0:
                line = ser.readline().decode('ascii').strip()
                if line:
                    return float(line)
        except (ValueError, UnicodeDecodeError):
            continue
        time.sleep(0.01)
    return None

def main():

    ser = serial.Serial(port='/dev/ttyACM0', baudrate=115200, timeout=0.0)
    if ser.is_open:
        print(f"Port {ser.name} opened")
    else:
        print(f"Port {ser.name} closed")

    measure_temperature_C = []
    measure_pressure_P = []
    measure_humidity_H = []
    measure_ts = []

    start_ts = time.time()
    try:
        for i in range(100):
            print("pu")
            
            ts = time.time() - start_ts

            measure_ts.append(ts)
            ser.write("temp\n".encode('ascii'))
            time.sleep(0.05)
            temp_C = read_value(ser)
            measure_temperature_C.append(temp_C)
            ser.write("press\n".encode('ascii'))
            time.sleep(0.05)
            press_P = read_value(ser)
            measure_pressure_P.append(press_P)
            ser.write("hum\n".encode('ascii'))
            time.sleep(0.05)
            hum_H = read_value(ser)
            measure_humidity_H.append(hum_H)

            # print(f'{voltage_V:.3f} V - {temp_C:.1f}C - {ts:.2f}s')
            time.sleep(0.1)

    finally:
        ser.write("tm_stop\n".encode('ascii'))
        ser.close()
        print("Port closed")

        plt.subplot(3, 1, 1)
        plt.plot(measure_ts, measure_temperature_C)
        plt.title('График зависимости температуры от времени')
        plt.xlabel('время, с')
        plt.ylabel('Температура, Cels')

        plt.subplot(3, 1, 2)
        plt.plot(measure_ts, measure_pressure_P)
        plt.title('График зависимости давления от времени')
        plt.xlabel('время, с')
        plt.ylabel('Давление, Па')

        plt.subplot(3, 1, 3)
        plt.plot(measure_ts, measure_humidity_H)
        plt.title('График зависимости влажности от времени')
        plt.xlabel('время, с')
        plt.ylabel('Влажность, проценты')

        plt.tight_layout()
        plt.show()


if __name__ == "__main__":
	main()

