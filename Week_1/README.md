# Bài tập 03 — GPIO đảo dữ liệu

Chương trình dành cho STM32F103C8T6:

- đọc 8 bit tại `PA0..PA7`;
- đảo từng bit (`0 -> 1`, `1 -> 0`);
- xuất kết quả tới dãy LED theo thứ tự `PA8, PA9, PA10, PA11, PA12, PB13, PB12, PA15`.

Mapping từng bit:

| Ngõ vào | Sau đảo | Ngõ ra LED |
|---|---:|---|
| PA0 | bit 0 | PA8 |
| PA1 | bit 1 | PA9 |
| PA2 | bit 2 | PA10 |
| PA3 | bit 3 | PA11 |
| PA4 | bit 4 | PA12 |
| PA5 | bit 5 | PB13 |
| PA6 | bit 6 | PB12 |
| PA7 | bit 7 | PA15 |

## Cấu trúc module

```text
inc/gpio.h     API GPIO và đọc PA0..PA7
inc/led.h      API khởi tạo/ghi dãy LED
src/gpio.c     clock, SWD, input pull-down và primitive GPIO
src/led.c      cấu hình chân LED và ánh xạ bit sang hai port
src/main.c     chỉ điều phối đọc -> đảo bit -> ghi LED
```

`PB13` thay cho `PA13` và `PB12` thay cho `PA14` theo phần cứng của bài. `PA13/PA14` được giữ cho SWD; JTAG-DP được tắt trong `GPIO_Init()` để giải phóng `PA15` nhưng vẫn nạp/debug được bằng ST-Link SWD.

PA0..PA7 được cấu hình input pull-down bằng điện trở nội. Vì vậy khi không tác động, input ở mức LOW, sau phép đảo các output ở mức HIGH và LED dương chung tắt. Đưa một input lên 3.3 V sẽ làm LED tương ứng sáng; nối GND giữ LED tắt.

## Biên dịch

Từ thư mục `bt_3` chạy:

```text
make
```

Kết quả nằm trong `build/firmware.elf` và `build/firmware.bin`. Có thể chỉ rõ lệnh OpenOCD khi nạp, ví dụ `make OPENOCD_SCRIPTS=<đường-dẫn-openocd>/scripts flash`.

Makefile đã trỏ tới bộ OpenOCD xPack ở `D:/Downloads/...` trên máy hiện tại và tự đổi đường dẫn ELF MSYS (`/d/...`) sang dạng Windows khi flash. Nếu OpenOCD nằm ở nơi khác, ghi đè như sau:

```text
make flash OPENOCD=D:/path/to/openocd.exe OPENOCD_SCRIPTS=D:/path/to/openocd/scripts
```
