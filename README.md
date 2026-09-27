# OLED_MODULE_OPI5

Orange Pi 5 / 5B / 5Plus 용 OLED 모듈 C 소스코드

---

## 테스트 환경

| 항목 | 내용 |
|------|------|
| 보드 | Orange Pi 5 Max (RK3588, 8GB) |
| OS | Orange Pi 1.0.2 Jammy (Ubuntu 22.04.5 LTS) |
| 커널 | 6.1.99-rockchip-rk3588 |
| Overlay (`/boot/orangepiEnv.txt`) | `overlays=i2c2-m0 pwm0-m0 pwm1-m0 pwm3-m3` |
| HAT | [52Pi EP-0152](https://wiki.52pi.com/index.php/EP-0152) Cooling Fan Expansion Board Plus 0.91" OLED V1.0 |

### EP-0152 HAT 구성 (Orange Pi 5 Max 기준)

| 기능 | 연결 | 비고 |
|------|------|------|
| OLED 0.96" (128x64, SSD1306) | I2C `/dev/i2c-2`, 주소 `0x3C` (Pin 3 SDA / Pin 5 SCL) | HAT 기본 0.91" 대신 0.96" 모듈 사용 |
| 팬 + RGB LED MCU | I2C `/dev/i2c-2`, 주소 `0x0D` | 팬: 레지스터 `0x08` (0x00 끔, 0x01 100%, 0x02~0x09 = 20~90%), RGB: `0x00`~`0x07` |
| LED1~4 | Pin 35 / 33 / 31 / 29 (wPi 23 / 22 / 20 / 19) | Active-high |

I2C 스캔 결과 (`sudo i2cdetect -y 2`): `0x0d`, `0x3c`

> **주의:** 52Pi 위키에는 팬이 GPIO14(Pin 8)로 제어된다고 되어 있지만, 이 보드에서는 팬이 I2C `0x0D` MCU로만 동작합니다.
> Orange Pi 5 Max의 Pin 8은 UART2 TXD(디버그 콘솔 `ttyS2`)이므로 GPIO로 사용하지 마세요.
> 보드 자체 팬 커넥터(`pwm-fan`)를 제어해도 HAT 팬은 돌지 않습니다.

GPIO 핀 정보는 `gpio readall`로 확인할 수 있습니다.

---

## 하드웨어 연결 (IIC)

| OLED | Orange Pi 5 |
|------|-------------|
| VCC  | 3.3V or 5.0V |
| GND  | GND |
| DIN  | SDA (Pin 3) |
| CLK  | SCL (Pin 5) |

I2C 버스: `/dev/i2c-2` (주소: `0x3C`)

---

## I2C 활성화

`sudo orangepi-config` 실행 후 **i2c2-m0** 활성화

활성화 확인:
```bash
ls /dev/i2c-*
```

OLED 주소 스캔:
```bash
sudo i2cdetect -y -r 2
```
`0x3c` 위치에 장치가 보이면 정상

---

## 빌드

```bash
cd OrangePi5/c
mkdir -p bin
make
```

---

## 실행

```bash
sudo ./main <OLED 크기>
```

### 지원 모델

| 명령어 | OLED 모델 |
|--------|-----------|
| `sudo ./main 0.91` | 0.91인치 OLED |
| `sudo ./main 0.91fan` | 0.91인치 OLED + Fan + RGB LED HAT |
| `sudo ./main 0.96` | 0.96인치 OLED |
| `sudo ./main 0.96rgb` | 0.96인치 RGB OLED |
| `sudo ./main 0.95rgb` | 0.95인치 RGB OLED |
| `sudo ./main 1.3` | 1.3인치 OLED |
| `sudo ./main 1.3c` | 1.3인치 OLED (C 타입) |
| `sudo ./main 1.32` | 1.32인치 OLED |
| `sudo ./main 1.27rgb` | 1.27인치 RGB OLED |
| `sudo ./main 1.5` | 1.5인치 OLED |
| `sudo ./main 1.5b` | 1.5인치 OLED (B 타입) |
| `sudo ./main 1.5rgb` | 1.5인치 RGB OLED |
| `sudo ./main 1.51` | 1.51인치 OLED |

### 실행 예시 (1.3인치)
```bash
sudo ./main 1.3
```

종료: `Ctrl + C`

---

## 표시 내용 (1.3인치 기준)

- 현재 날짜 및 시간
- 메모리 사용률
- CPU 온도
- IP 주소 (wlan0 / eth0)

---

## 0.91fan 모드 (Fan + RGB LED HAT)

Fan과 RGB LED가 달린 HAT 사용 시 (`sudo ./main 0.91fan`):

- I2C 주소 `0x0D` (HAT) 추가 필요
- CPU 온도 55°C 이상 → 팬 자동 ON
- CPU 온도 48°C 이하 → 팬 자동 OFF
- RGB LED 레인보우 효과 자동 설정

---

## I2C 권한 (sudo 없이 실행)

```bash
sudo usermod -aG i2c $USER
# 로그아웃 후 재로그인
```

---

## 클린 빌드

```bash
cd OrangePi5/c
make clean
make
```

![image](https://github.com/sagpaycokr/OLED_MODULE_OPI5/assets/70673576/4c550e7d-f450-4f38-a049-517b6a0c1533)

---

## oled_fan (상태 표시 + 팬 제어 서비스)

OLED에 시간/IP/CPU/메모리/온도를 표시하고 온도에 따라 팬을 제어하는 systemd 서비스입니다.
HAT 팬 MCU(I2C `0x0D`)와 보드 `pwm-fan`을 모두 지원합니다. 팬은 `fanctl`, HAT의 RGB LED는 `rgbctl` 명령으로 직접 제어할 수 있습니다. 자세한 내용은 [`OrangePi5/oled_fan`](OrangePi5/oled_fan/README.md) 참고.
