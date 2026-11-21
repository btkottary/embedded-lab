# Hardware notes

## STM32H7S78-DK

| Item | Setting / fact |
|---|---|
| Connect | USB-C on the **ST-LINK** port: power, SWD and virtual COM port |
| JP1 | **STLK (1-2)**: power from the ST-LINK USB |
| JP5 | **fitted**: MCU supply path (current-measurement jumper). Missing → CubeProgrammer shows `Voltage : 0.00V` |
| BOOT0 | factory default (low): boot from internal flash |
| JP6 | **PC1** position, needed later for Ethernet (back of the board) |
| LEDs | LD1 green PO1 and LD2 orange PO5 (on = HIGH); LD3 red PM2 and LD4 blue PM3 (on = LOW) |
| Gotcha | Ports O and M are in separate I/O supply domains: call `HAL_PWREx_EnableXSPIM1()` (port O) and enable the USB33 detector (port M) before using them |
| VCP UART | UART4 TX PD1 / RX PD0 |

## STM32N6570-DK

| Item | Setting / fact |
|---|---|
| Connect | USB-C **CN6** (ST-LINK) |
| JP2 | **1-2 (5V_STLK)** |
| SW1 = BOOT1 | **H** to flash or debug (dev boot), **L** to run from flash. Change only while unpowered |
| SW2 = BOOT0 | **L** |
| LEDs | LED1 green PO1 (on = HIGH, port O needs `HAL_PWREx_EnableVddIO2()`); LED2 red PG10 (on = LOW) |
| Boot | No internal flash. The boot ROM loads a signed image from NOR flash 0x70000000 into RAM 0x34180400 |
| Signing | `STM32_SigningTool_CLI -bin app.bin -nk -t fsbl -hv 2.3 -align -s -o app-trusted.bin` (done by the build) |
| VCP UART | USART1 TX PE5 / RX PE6 |
| Ethernet | Gigabit, RTL8211F PHY (RGMII) |

## Lab network (later phases)

- PC: Hyper-V switch `EXT_SW`, `vEthernet (EXT_SW)` = 192.168.1.10/24.
- Tftpd64 service: TFTP, plus DHCP handing out 192.168.1.100–149.
- Plan: N6 at .50 and H7S at .60 when static; otherwise they get addresses from DHCP.
