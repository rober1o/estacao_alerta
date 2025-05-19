#ifndef ESTACAO_ALERTA // Previne múltiplas inclusões do cabeçalho
#define ESTACAO_ALERTA

#include <stdio.h>
#include <stdint.h> // Para tipos uint32_t e uint8_t

// Bibliotecas do Raspberry Pi Pico
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"    // Controle do PIO (Programável I/O)
#include "hardware/clocks.h" // Manipulação de clock
#include "hardware/adc.h"    // Manipulação do Joystick

// Bibliotecas para displays e fontes
#include "lib/ssd1306.h"
#include "lib/font.h"

// Arquivos específicos do projeto
#include "pio_wave.pio.h"   // Código PIO
#include "lib/matriz_5X5.h" // Arrays para exibir na matriz de LED
#include "lib/desenhos.h"

// Bibliotecas para FreeRTOS
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"

// Definições de pinos e parâmetros
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define BUZZER_A 10 // Pino conectado ao buzzer
#define BUZZER_B 21 // Pino conectado ao buzzer

// Matriz de LEDs
#define MATRIZ_PIN 7      // Pino da matriz de LEDs
#define NUM_PIXELS 25     // Número de pixels na matriz
#define BRILHO_PADRAO 5 // Brilho padrão

// Definições dos LEDs
#define LED_VERDE 11
#define LED_VERMELHO 13
#define LED_AZUL 12

// definições Joystick
#define ADC_JOYSTICK_Y 27
#define ADC_JOYSTICK_X 28

// Instâncias e variáveis globais
PIO pio;       // Instância do PIO
int sm;        // Máquina de estado do PIO
ssd1306_t ssd; // display ssd
QueueHandle_t volume_chuva_display;
QueueHandle_t nivel_agua_display;
QueueHandle_t volume_chuva_buzzer;
QueueHandle_t nivel_agua_buzzer;
QueueHandle_t volume_chuva_led;
QueueHandle_t nivel_agua_led;
QueueHandle_t volume_chuva_matriz;
QueueHandle_t nivel_agua_matriz;

//DECLARAÇÃO FUNÇÕES DE INICALIZAÇÃO
void configurar_matriz_leds();
void inicializar_pinos_adc();
void inicializar_display_i2c();
void inicializar_leds();

// DECLARAÇÃO DAS TAREFAS
void vLerJoystick();
void vAtualizarDisplay();
void vTocarBuzzer();
void vTaskLed();
void vTaskMatriz();

//DECLARAÇÃO FUNÇÕES AUXILIARES
void gpio_irq_handler(uint gpio, uint32_t events);
void desenha_fig(uint32_t *_matriz, uint8_t _intensidade, PIO pio, uint sm);
void inicializar_pwms_buzzers();
void draw_ssd1306(uint32_t *_matriz);
void tocar_pwm_buzzer(uint gpio_pin, uint duracao_ms);
void piscar_led_azul();
void piscar_led_vermelho();

#endif // ESTACAO_ALERTA