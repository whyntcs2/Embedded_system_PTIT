import serial
import time

PORT = "COM13"          # Đổi thành cổng COM của CH340.
BAUD = 115200
NUM_LINES = 21         # 21 dòng tương đương khoảng 10 giây đo.

valid_lines = []
timestamps = []
types = []

ser = serial.Serial(PORT, BAUD, timeout=2)

ser.reset_input_buffer()

print("Dang do tan so lay mau...")
print("Hay dong Hercules truoc khi chay chuong trinh.\n")

while len(valid_lines) < NUM_LINES:

    raw = ser.readline().decode(
        "ascii",
        errors="ignore"
    ).strip()

    if not raw:
        continue

    sample_type = None

    if raw.startswith("HT:"):
        sample_type = "HT"
        raw = raw[3:]

    elif raw.startswith("TC:"):
        sample_type = "TC"
        raw = raw[3:]

    else:
        # Bỏ qua các dòng thông báo lúc STM32 khởi động.
        continue

    try:
        values = [
            int(x)
            for x in raw.split(",")
        ]
    except ValueError:
        print("Dong khong hop le.")
        continue

    # Mỗi HT hoặc TC phải chứa đúng 50 mẫu.
    if len(values) != 50:
        print(
            sample_type,
            "sai so mau:",
            len(values)
        )
        continue

    # ADC STM32F103 là ADC 12-bit.
    if not all(0 <= value <= 4095 for value in values):
        print(
            sample_type,
            "co gia tri ADC khong hop le."
        )
        continue

    current_time = time.perf_counter()

    valid_lines.append(values)
    timestamps.append(current_time)
    types.append(sample_type)

    print(
        f"{len(valid_lines):02d} - "
        f"{sample_type} - "
        f"50 samples"
    )

ser.close()


print("\nKet qua:")

print(
    "So dong hop le:",
    len(valid_lines)
)

print(
    "Tong so mau nhan duoc:",
    len(valid_lines) * 50
)


if len(valid_lines) >= 2:

    duration = (
        timestamps[-1]
        - timestamps[0]
    )

    # Giữa N dòng có N-1 khoảng thời gian.
    # Mỗi khoảng tương ứng với 50 mẫu ADC.
    measured_samples = (
        len(valid_lines) - 1
    ) * 50

    sample_rate = (
        measured_samples
        / duration
    )

    average_line_time = (
        duration
        / (len(valid_lines) - 1)
    )

    print(
        "Thoi gian do:",
        round(duration, 3),
        "s"
    )

    print(
        "Khoang thoi gian trung binh "
        "giua HT/TC:",
        round(
            average_line_time,
            4
        ),
        "s"
    )

    print(
        "Tan so lay mau:",
        round(
            sample_rate,
            2
        ),
        "Hz"
    )


# Kiểm tra thứ tự HT -> TC -> HT -> TC.
sequence_ok = True

for i in range(1, len(types)):

    if types[i] == types[i - 1]:
        sequence_ok = False
        break


print(
    "Thu tu HT/TC:",
    "DUNG" if sequence_ok else "SAI"
)