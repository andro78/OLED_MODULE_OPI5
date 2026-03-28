# OLED_MODULE_OPI5

Orange Pi 5 / 5B / 5Plus 용 OLED 모듈 C 소스코드

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
