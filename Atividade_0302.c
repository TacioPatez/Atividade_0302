#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

#define BUTTON_A 5
#define BUTTON_B 6

#define LED_G 11
#define LED_B 12

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


int main() {
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

  bool cor = true;

  // Configuração da interrupção com callback
  gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
  gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
  while (true) {
    cor = !cor;
    // Atualiza o conteúdo do display com animações
    ssd1306_fill(&ssd, !cor); // Limpa o display
    ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
    ssd1306_draw_string(&ssd, "CEPEDI   TIC37", 8, 10); // Desenha uma string
    ssd1306_draw_string(&ssd, "EMBARCATECH", 20, 30); // Desenha uma string
    ssd1306_draw_string(&ssd, "PROF WILTON", 15, 48); // Desenha uma string      
    ssd1306_send_data(&ssd); // Atualiza o display

    sleep_ms(1000);
  }
}