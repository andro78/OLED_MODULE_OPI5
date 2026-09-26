# oled_fan

Orange Pi 5용 0.96인치 I2C OLED(SSD1306) 상태 표시 + PWM 팬 제어
([OLED_MODULE_OPI5](https://github.com/andro78/OLED_MODULE_OPI5) 기반)

## 하드웨어
| 장치 | 연결 |
|------|------|
| OLED 0.96" | `/dev/i2c-2` (i2c2-m0, SDA pin 3 / SCL pin 5), 주소 `0x3C` |
| Fan (HAT) | 52Pi EP-0152 등 HAT의 팬 MCU: `/dev/i2c-2` 주소 `0x0D`, 레지스터 `0x08` (0x00 끔, 0x01 100%, 0x02~0x09 = 20~90%) |
| Fan (보드) | 보드 팬 커넥터, 커널 `pwm-fan` 드라이버 (`/sys/class/hwmon/hwmonN/pwm1`, name=`pwmfan`) |

두 팬 중 찾은 것을 모두 같은 값으로 제어합니다. HAT 팬은 PWM 값을 가장 가까운 10% 단계로 변환해 씁니다.

테스트 환경: Orange Pi 5 Max (RK3588) / Ubuntu 22.04 (Orange Pi 1.0.2 Jammy) / 커널 6.1.99-rockchip-rk3588 /
52Pi EP-0152 HAT (0.91" OLED + 팬 MCU `0x0D`)

> 현재 OLED 드라이버는 0.96" 128x64(`OLED_0in96`) 기준입니다. EP-0152의 0.91" 128x32 OLED에서는 화면 일부만 보일 수 있습니다.

## 화면
```
09/24 19:57:36
192.168.0.10
CPU  1% MEM 16%
TEMP 42.5C
FAN 58% [#####   ]
```

## 팬 커브 (디바이스 트리 값 사용, 가장 뜨거운 thermal zone 기준)
| 온도 | < 50°C | 50 | 55 | 60 | 65 | ≥ 70 |
|------|------|----|----|----|----|------|
| PWM  | 0 | 50 | 100 | 150 | 200 | 255 |

- 내려갈 때는 3°C 히스테리시스
- 정지 상태에서 켤 때는 0.5초간 255로 킥스타트

## 빌드 / 실행
```bash
make
sudo ./oled_fan          # 테스트 (Ctrl+C 종료)
```

## 서비스 등록 (부팅 시 자동 실행)
```bash
sudo make install        # /usr/local/bin + systemd oled_fan.service
journalctl -u oled_fan -f
sudo make uninstall
```

설정값: OLED I2C 버스/주소는 `lib/Config/DEV_Config.h`, 팬 MCU 주소와 팬 커브는 `src/fan.c`
