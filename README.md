# Projeto: Controle de LEDs e Display OLED com Raspberry Pi Pico

## Descrição
Este projeto utiliza o Raspberry Pi Pico para controlar um display OLED SSD1306 e uma matriz de LEDs WS2812. Além disso, a comunicação UART é usada para interação com o sistema.

## Vídeo Explicativo
https://youtu.be/u25vja3wREw

## Requisitos
- Raspberry Pi Pico
- Display OLED SSD1306 (I2C)
- LEDs WS2812 (NeoPixels)
- Botões de entrada
- Cabo USB para comunicação com o computador
- Bibliotecas:
  - `pico/stdlib.h`
  - `hardware/i2c.h`
  - `hardware/pio.h`
  - `ws2812.pio.h`
  - `inc/ssd1306.h`
  - `inc/font.h`

## Uso
- O dispositivo irá escutar pela UART e exibir caracteres recebidos no display OLED.
- Se um número (0-9) for enviado, ele será representado nos LEDs WS2812.
- Os botões podem ser usados para alternar LEDs de status.

## Autor
Tácio Patez
