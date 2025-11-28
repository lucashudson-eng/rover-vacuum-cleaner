# rover

Autonomous robotic rover designed for navigation and task execution, inspired by smart vacuum cleaners.

## Main Components

- [BTT PI 2](https://pt.aliexpress.com/item/1005006869449371.html?spm=a2g0o.order_list.order_list_main.5.6ecdcaa4VrRXkU&gatewayAdapt=glo2bra)
- [ESP32-S3 Nano](https://pt.aliexpress.com/item/1005007291319086.html?spm=a2g0o.order_list.order_list_main.35.6ecdcaa4UuJD2Q&gatewayAdapt=glo2bra)
- [Lidar 2D 360º LD14P](https://pt.aliexpress.com/item/1005008670123046.html?spm=a2g0o.order_list.order_list_main.55.6ecdcaa4UuJD2Q&gatewayAdapt=glo2bra)
- [N20 DC Motor 6V 500RPM w/ Magnetic Encoders](https://pt.aliexpress.com/item/1005004999529855.html?spm=a2g0o.order_list.order_list_main.30.6ecdcaa4UuJD2Q&gatewayAdapt=glo2bra)
- [Pololu Motor Driver TB6612FNG](https://www.pololu.com/product/713)
- [IMU 6-DOF MPU-6050](https://pt.aliexpress.com/item/1005008569435439.html?spm=a2g0o.productlist.main.3.4dd4T9KNT9KNTG&algo_pvid=daaf6f87-5922-4101-85bb-d80ca02fc763&algo_exp_id=daaf6f87-5922-4101-85bb-d80ca02fc763-2&pdp_ext_f=%7B%22order%22%3A%22216%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21BRL%2136.05%2115.50%21%21%2144.13%2118.98%21%402101ef5e17621751905178166e3ecb%2112000045757063335%21sea%21BR%212609103932%21X%211%210%21n_tag%3A-29919%3Bd%3Ab6a655b5%3Bm03_new_user%3A-29895&curPageLogUid=6MyxK9Nt2MNL&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005008569435439%7C_p_origin_prod%3A)
- [Wheels 65mm](https://pt.aliexpress.com/item/1005005293126427.html?spm=a2g0o.order_list.order_list_main.25.6ecdcaa4UuJD2Q&gatewayAdapt=glo2bra)
- [Chassi 4WD](https://pt.aliexpress.com/item/1005007026658421.html?spm=a2g0o.order_list.order_list_main.10.6ecdcaa4UuJD2Q&gatewayAdapt=glo2bra)
- [Camera OV5647](https://pt.aliexpress.com/item/1005006279352733.html?spm=a2g0o.productlist.main.4.56ff7acaNgMLwa&algo_pvid=590cb42b-17bc-4a75-8a59-095083667ec9&algo_exp_id=590cb42b-17bc-4a75-8a59-095083667ec9-3&pdp_ext_f=%7B%22order%22%3A%22160%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21BRL%2150.78%2115.41%21%21%2161.37%2118.63%21%402101f43717603634664327876eb2c9%2112000036586833896%21sea%21BR%212609103932%21X%211%210%21n_tag%3A-29919%3Bd%3Ab6a655b5%3Bm03_new_user%3A-29895%3BpisId%3A5000000189119060&curPageLogUid=QsAUfhIPxS6e&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005006279352733%7C_p_origin_prod%3A)
- [3x ToF VL53L1X/TOF400C](https://pt.aliexpress.com/item/1005009534829792.html?spm=a2g0o.order_list.order_list_main.15.6ecdcaa4UuJD2Q&gatewayAdapt=glo2bra)
- [I2C Multiplexer TCA9548A](https://pt.aliexpress.com/item/1005004996004241.html?spm=a2g0o.order_list.order_list_main.40.6ecdcaa4UuJD2Q&gatewayAdapt=glo2bra)
- [Buck Step Down 6V 2.2A LM317](https://pt.aliexpress.com/item/1005006299002383.html?spm=a2g0o.productlist.main.17.6ad3342ecXNScI&algo_pvid=f8052cfb-76f1-484c-81a9-b1f5b1c1cdaf&algo_exp_id=f8052cfb-76f1-484c-81a9-b1f5b1c1cdaf-16&pdp_ext_f=%7B%22order%22%3A%2256%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21BRL%2110.25%2110.25%21%21%2112.55%2112.55%21%402101e81117621755212924255e33af%2112000038530441085%21sea%21BR%212609103932%21X%211%210%21n_tag%3A-29919%3Bd%3Ab6a655b5%3Bm03_new_user%3A-29895&curPageLogUid=DKXvjPlopQFe&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005006299002383%7C_p_origin_prod%3A)
-  [I2C Power Monitoring INA219](https://pt.aliexpress.com/item/1005007533576335.html?spm=a2g0o.cart.0.0.59ba38daPgUGvn&mp=1&pdp_npi=5%40dis%21BRL%21BRL%209.52%21BRL%209.02%21%21BRL%209.02%21%21%21%402101f54117621768399517444ec7b1%2112000041187110498%21ct%21BR%212609103932%21%211%210&gatewayAdapt=glo2bra)
- Battery 3s 2500mah

## Auxiliary Components

- [2P/4P/5P/6P JST 2.54mm Right Angle Pin Connector](https://pt.aliexpress.com/item/33008489410.html?spm=a2g0o.cart.0.0.27dd38da7r1a39&mp=1&pdp_npi=5%40dis%21BRL%21BRL%209.30%21BRL%208.60%21%21BRL%208.60%21%21%21%402101e81117621761186773727e33b5%2167049563013%21ct%21BR%212609103932%21%211%210&gatewayAdapt=glo2bra)
- [2P/4P/5P/6P JST 2.54mm Housing](https://pt.aliexpress.com/item/1005007449862216.html?spm=a2g0o.productlist.main.11.2b1e73b5pSA2eL&algo_pvid=2de088dd-77d7-40c6-9815-db88d39e57ea&algo_exp_id=2de088dd-77d7-40c6-9815-db88d39e57ea-10&pdp_ext_f=%7B%22order%22%3A%22145%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21BRL%217.89%217.89%21%21%219.66%219.66%21%4021033d1217621766002524230e9ba1%2112000040800274467%21sea%21BR%212609103932%21X%211%210%21n_tag%3A-29919%3Bd%3Ab6a655b5%3Bm03_new_user%3A-29895&curPageLogUid=9jnQ1CbhPZ3n&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005007449862216%7C_p_origin_prod%3A)
- [4P JST 2.54mm Extension Line](https://pt.aliexpress.com/item/1005007492397255.html?spm=a2g0o.cart.0.0.27dd38da7r1a39&mp=1&pdp_npi=5%40dis%21BRL%21BRL%2018.49%21BRL%2017.21%21%21BRL%2017.21%21%21%21%402101e81117621761186773727e33b5%2112000041008539254%21ct%21BR%212609103932%21%211%210&gatewayAdapt=glo2bra)
- [Battery Velcro Strap](https://shopee.com.br/Cinta-Correia-Velcro-Para-Bateria-Lipo-Drone-Aeromodelo-automodelo-25cm-i.389375152.9818950596)
- [PCB Universal Board Single Sid 7x9cm](https://pt.aliexpress.com/item/1005007720411113.html?spm=a2g0o.productlist.main.4.3dfc19ab1n2Tg2&algo_pvid=6e5fad30-26c5-4678-8d77-60f01587fe29&algo_exp_id=6e5fad30-26c5-4678-8d77-60f01587fe29-3&pdp_ext_f=%7B%22order%22%3A%22228%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21BRL%2129.97%219.29%21%21%2136.69%2111.37%21%40210319b717621766512291264e0ad9%2112000041969303296%21sea%21BR%212609103932%21X%211%210%21n_tag%3A-29919%3Bd%3Ab6a655b5%3Bm03_new_user%3A-29895&curPageLogUid=EMkKAa5T090T&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005007720411113%7C_p_origin_prod%3A)
- [PTH Resistors 1/4W 3.3K/6.2K/10K](https://pt.aliexpress.com/item/32968455454.html?spm=a2g0o.cart.0.0.31ec38daVKlkNH&mp=1&pdp_npi=5%40dis%21BRL%21BRL%2010.41%21BRL%2010.41%21%21BRL%2010.41%21%21%21%402101f54117621767026043319ec7b1%2110000011287130420%21ct%21BR%212609103932%21%211%210&gatewayAdapt=glo2bra)
- [M2.5/M3 Hex Nylon Standoff Spacer](https://pt.aliexpress.com/item/32862529967.html?spm=a2g0o.cart.0.0.31ec38daVKlkNH&mp=1&pdp_npi=5%40dis%21BRL%21BRL%2023.61%21BRL%2021.92%21%21BRL%2021.92%21%21%21%402101f54117621767041793366ec7b1%2110000000197494051%21ct%21BR%212609103932%21%211%210&gatewayAdapt=glo2bra)
- [12V Round Rocker Switch LED](https://pt.aliexpress.com/item/1005007561730939.html?spm=a2g0o.cart.0.0.311738daTVxsL3&mp=1&pdp_npi=5%40dis%21BRL%21BRL%2015.13%21BRL%2013.89%21%21BRL%2013.89%21%21%21%402101f54117621771594217560ec7b1%2112000046406770156%21ct%21BR%212609103932%21%211%210&gatewayAdapt=glo2bra)
- [N20 Motor Mount](https://pt.aliexpress.com/item/1005007027114123.html?spm=a2g0o.productlist.main.5.5c3f4ac7RAcjJN&algo_pvid=1c03e1d7-4e87-40f0-95ba-62695d580392&algo_exp_id=1c03e1d7-4e87-40f0-95ba-62695d580392-4&pdp_ext_f=%7B%22order%22%3A%2252%22%2C%22eval%22%3A%221%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21BRL%218.25%217.80%21%21%2110.15%219.60%21%402103212317621772337704473e6771%2112000039138284502%21sea%21BR%212609103932%21X%211%210%21n_tag%3A-29919%3Bd%3Ab6a655b5%3Bm03_new_user%3A-29895&curPageLogUid=yJE3T42w33CW&utparam-url=scene%3Asearch%7Cquery_from%3A%7Cx_object_id%3A1005007027114123%7C_p_origin_prod%3A)

## Pinout

### Battery
- Cell 1 - A0 (GPIO1)
- Cell 1+2 - A1 (GPIO2)

### ISR

- 3xVL6180X - A2 (GPIO3)
- MPU6050 - A3 (GPIO4)
- Motor Encoder Back Left C1 - D2 (GPIO5)
- Motor Encoder Back Left C2 - D3 (GPIO6)
- Motor Encoder Back Right C2 - D4 (GPIO7)
- Motor Encoder Back Right C1 - D5 (GPIO8)

### Motor Driver

- PWMA - D6 (GPIO8)
- AIN2 - D7 (GPIO9)
- AIN1 - D8 (GPIO10)
- STBY - D9 (GPIO18)
- BIN1 - D10 (GPIO21)
- BIN2 - D11 (GPIO38)
- PWMB - D12 (GPIO47)

### I2C (MPU6050, TCA9548A (3xVL6180X), INA219)

- SDA - A4 (GPIO11)
- SCL - A5 (GPIO12)
