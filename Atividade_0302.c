#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

#define BUTTON_A 5
#define BUTTON_B 6

#define LED_G 11
#define LED_B 12

#define UART_ID uart0 // Seleciona a UART0
#define BAUD_RATE 115200 // Define a taxa de transmissão
#define UART_TX_PIN 0 // Pino GPIO usado para TX
#define UART_RX_PIN 1 // Pino GPIO usado para RX

#define NUM_PIXELS 25
#define IS_RGBW false
#define WS2812_PIN 7

// Armazena o tempo do último evento (em microssegundos)
static volatile uint32_t last_time = 0; 

//  Funções de Interrupção
static void gpio_irq_handler(uint gpio, uint32_t events)
{
    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    if(current_time - last_time > 200000) { // 200 ms de debouncing
        last_time = current_time;
        if(gpio == BUTTON_A){
          gpio_put(LED_G, !gpio_get(LED_G));
        } else {
          gpio_put(LED_B, !gpio_get(LED_B));
        }
    }
}

// Buffer para armazenar quais LEDs estão ligados matriz 5x5
bool led_buffer_1[NUM_PIXELS] = {
  0, 1, 0, 0, 0, 
  0, 0, 0, 1, 0, 
  0, 1, 0, 0, 0, 
  0, 0, 0, 1, 0, 
  0, 1, 0, 0, 0
};

bool led_buffer_2[NUM_PIXELS] = {
  0, 1, 1, 1, 0, 
  0, 1, 0, 0, 0, 
  0, 1, 1, 1, 0, 
  0, 0, 0, 1, 0, 
  0, 1, 1, 1, 0
};

bool led_buffer_3[NUM_PIXELS] = {
  0, 1, 1, 1, 0, 
  0, 0, 0, 1, 0, 
  0, 1, 1, 1, 0, 
  0, 0, 0, 1, 0, 
  0, 1, 1, 1, 0
};

bool led_buffer_4[NUM_PIXELS] = {
  0, 1, 0, 0, 0, 
  0, 0, 0, 1, 0, 
  0, 1, 1, 1, 0, 
  0, 1, 0, 1, 0, 
  0, 1, 0, 1, 0
};

bool led_buffer_5[NUM_PIXELS] = {
  0, 1, 1, 1, 0, 
  0, 0, 0, 1, 0, 
  0, 1, 1, 1, 0, 
  0, 1, 0, 0, 0, 
  0, 1, 1, 1, 0
};

bool led_buffer_6[NUM_PIXELS] = {
  0, 1, 1, 1, 0, 
  0, 1, 0, 1, 0, 
  0, 1, 1, 1, 0, 
  0, 1, 0, 0, 0, 
  0, 1, 1, 1, 0
};

bool led_buffer_7[NUM_PIXELS] = {
  0, 1, 0, 0, 0, 
  0, 0, 0, 1, 0, 
  0, 1, 0, 0, 0, 
  0, 0, 0, 1, 0, 
  0, 1, 1, 1, 0
};

bool led_buffer_8[NUM_PIXELS] = {
  0, 1, 1, 1, 0, 
  0, 1, 0, 1, 0, 
  0, 1, 1, 1, 0, 
  0, 1, 0, 1, 0, 
  0, 1, 1, 1, 0
};

bool led_buffer_9[NUM_PIXELS] = {
  0, 1, 1, 1, 0, 
  0, 0, 0, 1, 0, 
  0, 1, 1, 1, 0, 
  0, 1, 0, 1, 0, 
  0, 1, 1, 1, 0
};

bool led_buffer_0[NUM_PIXELS] = {
  0, 1, 1, 1, 0, 
  0, 1, 0, 1, 0, 
  0, 1, 0, 1, 0, 
  0, 1, 0, 1, 0, 
  0, 1, 1, 1, 0
};

bool led_buffer_nothing[NUM_PIXELS] = {
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0
};

// Array de ponteiros para os buffers
bool* led_buffers[10] = {
  led_buffer_0, led_buffer_1, led_buffer_2, led_buffer_3, led_buffer_4,
  led_buffer_5, led_buffer_6, led_buffer_7, led_buffer_8, led_buffer_9
};

static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}


void set_one_led(char c) {
  uint8_t number = c - '0';
  // Define a cor
  uint32_t color = urgb_u32(255, 0, 0);

  bool* buffer = c == 'n' ? led_buffer_nothing : led_buffers[number % 10];
  // Define todos os LEDs com a cor especificada
  for (int i = 0; i < NUM_PIXELS; i++)
  {
      if (buffer[i])
      {
          put_pixel(color); // Liga o LED com um no buffer
      }
      else
      {
          put_pixel(0);  // Desliga os LEDs com zero no buffer
      }
  }
}

int main() {
  // Inicializa comunicação USB CDC para monitor serial
  stdio_init_all(); 

  // Inicializa a UART
  uart_init(UART_ID, BAUD_RATE);

  // Configura os pinos GPIO para a UART
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART); // Configura o pino 0 para TX
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART); // Configura o pino 1 para RX

    // Mensagem inicial
    const char *init_message = "UART Demo - RP2\r\n"
    "Digite algo e veja o eco:\r\n";
    uart_puts(UART_ID, init_message);

  //inicializacao Pinagem
  gpio_init(BUTTON_A); // Inicializa Botao A 
  gpio_set_dir(BUTTON_A, GPIO_IN); // Configura Botao como entrada
  gpio_pull_up(BUTTON_A);  // Habilita Pull Up interno
  gpio_init(BUTTON_B); // Inicializa Botao B
  gpio_set_dir(BUTTON_B, GPIO_IN); // Configura Botao como entrada
  gpio_pull_up(BUTTON_B);  // Habilita Pull Up interno

  gpio_init(LED_G);   // Inicializa o LED
  gpio_set_dir(LED_G, GPIO_OUT);  // Configura o led como saida
  gpio_init(LED_B);   // Inicializa o LED
  gpio_set_dir(LED_B, GPIO_OUT);  // Configura o led como saida

  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_pull_up(I2C_SDA); // Pull up the data line
  gpio_pull_up(I2C_SCL); // Pull up the clock line
  ssd1306_t ssd; // Inicializa a estrutura do display
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
  ssd1306_config(&ssd); // Configura o display
  ssd1306_send_data(&ssd); // Envia os dados para o display

  // Limpa o display. O display inicia com todos os pixels apagados.
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

  // Configuração da interrupção com callback
  gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
  gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

  //inicializacao pio
  PIO pio = pio0;
  int sm = 0;
  uint offset = pio_add_program(pio, &ws2812_program);

  ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

  while (true) {
    if (uart_is_readable(UART_ID)) {
      // Lê um caractere da UART
      char c = uart_getc(UART_ID);
      ssd1306_fill(&ssd, false);
      ssd1306_draw_char(&ssd, c, 30, 30);    
      ssd1306_send_data(&ssd); // Atualiza o display
      if (c >= '0' && c <= '9'){
        set_one_led(c);
      } else {
        set_one_led('n');
      }
      // Envia de volta o caractere lido (eco)
      uart_putc(UART_ID, c);
      // Envia uma mensagem adicional para cada caractere recebido
      uart_puts(UART_ID, " <- Eco do RP2\r\n");
    }
  }
}