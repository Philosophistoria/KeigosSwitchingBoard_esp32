# KeigosSwitchingBoard_esp32
Keigo's photo MOSFET switching board's hardware and software

PC side program: https://github.com/Philosophistoria/KeigosSwitchingBoard_python/tree/main

| pin (GPIO) | name on board | name in the program |
| ---------- | ------------- | ------------------- |
| 32         | DI            | SER                 |
| 33         | RC            | CLK (clock of shift resisters) |
| 25         | CL            | CLR                 |
| 26         | CK            | RCK (Clock of latches) |

components for switching cirtuit
- photo relay: [TLP176GA](https://www.mouser.jp/ProductDetail/Toshiba/TLP176GAF?qs=L%2FgSpqKJZEvJJ3hcHMwEJw%3D%3D)
- shift resister: [74HC595](https://www.ti.com/product/SN74HC595)

