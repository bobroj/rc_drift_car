import struct
from time import sleep
import pigpio
from RF24 import RF24, RPi.GPIO

# Настройки RF24 (CE=25, CSN=8)
radio = RF24(25, 0)
address = b"00001"

# Инициализация pigpio
pi = pigpio.pi()
if not pi.connected:
    print("Ошибка: pigpio не запущен")
    exit()

# Пины для ESC и серво
ESC_PIN = 18  # BCM
SERVO_PIN = 19  # BCM

# Настройка PWM частоты
pi.set_PWM_frequency(ESC_PIN, 50)
pi.set_PWM_frequency(SERVO_PIN, 50)

def pulse_width_to_duty(pulse_us):
    # Преобразуем длительность импульса (1000-2000) в 0-255 (для pigpio)
    return int((pulse_us / 20000) * 255)

# Настройка радио
radio.begin()
radio.setPALevel(RF24.PA_LOW)
radio.setDataRate(RF24.BR_1MBPS)
radio.openReadingPipe(1, address)
radio.startListening()

print("Готов к приёму...")

try:
    while True:
        if radio.available():
            received = radio.read(4)  # 2 int по 2 байта
            steering, throttle = struct.unpack('<hh', received)

            # Угол поворота (пример: 40–125° => 1000–2000 мкс)
            angle = int(1000 + ((steering / 1023.0) * 1000))
            angle = max(1000, min(angle, 2000))

            # Газ (ограничение и масштабирование)
            throttle = max(513, min(throttle, 1024))
            speed = int(((throttle - 513) / (1024 - 513)) * 1000 + 1000)
            speed = max(1000, min(speed, 2000))

            # Вывод
            print(f"Руль: {angle} µs | Газ: {speed} µs")

            # Управляем
            pi.set_servo_pulsewidth(SERVO_PIN, angle)
            pi.set_servo_pulsewidth(ESC_PIN, speed)

        sleep(0.05)

except KeyboardInterrupt:
    print("Остановка...")
    pi.set_servo_pulsewidth(ESC_PIN, 0)
    pi.set_servo_pulsewidth(SERVO_PIN, 0)
    pi.stop()
