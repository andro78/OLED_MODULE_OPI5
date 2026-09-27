# oled_fan

Orange Pi 5용 0.96인치 I2C OLED(SSD1306) 상태 표시 + 팬/RGB LED 제어
([OLED_MODULE_OPI5](https://github.com/andro78/OLED_MODULE_OPI5) 기반)

## 하드웨어
| 장치 | 연결 |
|------|------|
| OLED 0.96" | `/dev/i2c-2` (i2c2-m0, SDA pin 3 / SCL pin 5), 주소 `0x3C` |
| Fan / RGB LED (HAT) | 52Pi EP-0152 등 HAT의 MCU: `/dev/i2c-2` 주소 `0x0D` (팬 레지스터 `0x08`: 0x00 끔, 0x01 100%, 0x02~0x09 = 20~90%) |
| Fan (보드) | 보드 팬 커넥터, 커널 `pwm-fan` 드라이버 (`/sys/class/hwmon/hwmonN/pwm1`, name=`pwmfan`) |

두 팬 중 찾은 것을 모두 같은 값으로 제어합니다. HAT 팬은 PWM 값을 가장 가까운 10% 단계로 변환해 씁니다.
보드 팬 커넥터는 Rockchip 커널 드라이버가 `rockchip,temp-trips` 값으로 자체 제어하므로 아래 커브가 유지되지 않을 수 있습니다.

테스트 환경: Orange Pi 5 Max (RK3588) / Ubuntu 22.04 (Orange Pi 1.0.2 Jammy) / 커널 6.1.99-rockchip-rk3588 /
52Pi EP-0152 HAT (팬 + RGB LED MCU `0x0D`) + 0.96" 128x64 OLED

## 화면
```
09/24 19:57:36
W 192.168.18.34      (3초마다 E <이더넷 IP> 와 번갈아 표시)
CPU  1% MEM 16%
TEMP 42.5C
FAN 58% [#####   ]
```

- 2번째 줄은 Wi-Fi(`wl*`) / 이더넷(`eth*`, `en*`) IP를 3초마다 번갈아 표시, 연결이 없으면 `No link`
- FAN 막대는 맨 아래 4줄(y 60~63)에만 그림. 테스트 패널은 예전 프로그램이 IP를 띄워 두던 y 48~59가 번인되어, 그 영역을 채우면 옛 글자가 비쳐 보임

## 팬 커브 (가장 뜨거운 thermal zone 기준)
| 온도 | < 35°C | 35~40°C | ≥ 40°C |
|------|--------|---------|--------|
| PWM  | 128 (50%) | 현재 속도 유지 | 255 (100%) |

- 35~40°C 구간은 히스테리시스: 올라갈 때는 40°C에서 100%, 내려갈 때는 35°C 미만에서 50%
- 팬 속도는 10초마다 다시 전송 (HAT MCU가 리셋되면 설정을 잃어버림)

## 수동 제어 (fanctl)
```bash
fanctl              # 상태: temp 36.1C  fan 50% (pwm 128)  mode auto
fanctl full         # 100% 고정
fanctl 30           # 30% 고정 (HAT 팬은 10% 단위, 최소 20%)
fanctl off          # 정지
fanctl auto         # 온도 커브로 복귀
```
- 설정은 `/run/oled_fan/override`에 저장되므로 재부팅하면 `auto`로 돌아감
- 현재 상태는 `/run/oled_fan/state` (`<온도> <pwm> <auto|manual>`)

## RGB LED 제어 (rgbctl)
HAT MCU(I2C `0x0D`)의 RGB LED를 제어합니다. 마지막 설정은 `/etc/oled_fan/rgb`에 저장되고, 서비스 시작 시 자동으로 다시 적용됩니다.
```bash
rgbctl effect rainbow fast     # 효과: water, breathing, marquee, rainbow, colorful / 속도: slow, normal, fast
rgbctl color blue              # water/breathing 색: red, green, blue, yellow, purple, cyan, white
rgbctl rgb 255 0 0             # 고정 색 (R G B, 0-255), 뒤에 LED 번호를 주면 해당 LED만
rgbctl off                     # 끄기
rgbctl show                    # 저장된 설정 보기
```

| 레지스터 | 기능 |
|----------|------|
| `0x00`~`0x03` | LED 선택(0xFF = 전체), R, G, B |
| `0x04` | 효과 0~4 |
| `0x05` | 효과 속도 1~3 |
| `0x06` | 효과 색 0~6 |
| `0x07` | `0x00` = 끄기 |
| `0x08` | 팬 속도 |

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
