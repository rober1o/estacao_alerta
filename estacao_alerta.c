#include "estacao_alerta.h"

/*******************************************************

                      TAREFAS

****************************************************** */
void vLerJoystick()
{
    inicializar_pinos_adc(); // tarefa para receber o valores do joystick
    while (true)
    {
        // Eixo Y (ADC 0)
        adc_select_input(0);
        uint16_t y_pos = adc_read();
        int percentual_y = (y_pos * 100) / 4095; // pós calcular o percentual de y, passa para as 4 filas do volume da chuva
        xQueueSend(volume_chuva_display, &percentual_y, portMAX_DELAY);
        xQueueSend(volume_chuva_buzzer, &percentual_y, portMAX_DELAY);
        xQueueSend(volume_chuva_led, &percentual_y, portMAX_DELAY);
        xQueueSend(volume_chuva_matriz, &percentual_y, portMAX_DELAY);
        adc_select_input(1);
        uint16_t x_pos = adc_read();
        int percentual_x = (x_pos * 100) / 4095; // pós calcular o percentual de x, passa para as 4 filas do nivel da água
        xQueueSend(nivel_agua_display, &percentual_x, portMAX_DELAY);
        xQueueSend(nivel_agua_buzzer, &percentual_x, portMAX_DELAY);
        xQueueSend(nivel_agua_led, &percentual_x, portMAX_DELAY);
        xQueueSend(nivel_agua_matriz, &percentual_x, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

void vAtualizarDisplay(void *params) // Tarefa para atualizar o display
{
    inicializar_display_i2c(); // inicializa o i2c

    int valor_chuva = 0;
    int valor_agua = 0;
    char buffer[20];

    while (true)
    {
        // Bloqueia até receber dados novos para cada fila, sem complicação de contadores
        if (xQueueReceive(volume_chuva_display, &valor_chuva, portMAX_DELAY) != pdPASS)
            continue;
        if (xQueueReceive(nivel_agua_display, &valor_agua, portMAX_DELAY) != pdPASS)
            continue;

        // Atualiza display
        ssd1306_fill(&ssd, false);
        if (valor_chuva >= 80 && valor_agua >= 70) // Caso o nivel da água seja maior que 70% e o da chuva seja maior que 80%, imprime uma caveira como sinal de alerta crítico
        {
            ssd1306_fill(&ssd, false);
            draw_ssd1306(caveira);
        }
        else if (valor_chuva >= 80) // caso somente a chuva tenha valor acima de 80%, imprime uma nuvem simbolizando alerta de chuva
        {
            ssd1306_fill(&ssd, false);
            draw_ssd1306(chuva);
        }
        else if (valor_agua >= 70) // caso somente a água tenha valor acima de 70%, imprime uma casa imundada simbolizando alerta de enchente
        {
            ssd1306_fill(&ssd, false);
            draw_ssd1306(agua);
        }

        if (valor_chuva >= 80 || valor_agua >= 70) // Caso ou a água ou  achuva passem dos valores normais, imprime a mensagem arleta no display
        {
            snprintf(buffer, sizeof(buffer), "ALERTA!!");
            ssd1306_draw_string(&ssd, buffer, 40, 48);
        }
        else // caso contrario o display mostrará o modo normal
        {
            snprintf(buffer, sizeof(buffer), "NORMAL");
            ssd1306_draw_string(&ssd, buffer, 40, 48);
        }

        ssd1306_rect(&ssd, 3, 3, 122, 60, true, false);

        snprintf(buffer, sizeof(buffer), "CHV: %d%%", valor_chuva); // imprime o volume da chuva no display
        ssd1306_draw_string(&ssd, buffer, 12, 9);

        snprintf(buffer, sizeof(buffer), "AG: %d%%", valor_agua); // imprime o nivel da água no display
        ssd1306_draw_string(&ssd, buffer, 12, 25);
        ssd1306_rect(&ssd, 40, 4, 121, 1, true, false);
        ssd1306_rect(&ssd, 4, 80, 1, 37, true, false);
        ssd1306_send_data(&ssd);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void vTocarBuzzer()
{
    inicializar_pwms_buzzers(); // inicializa dos dois buzzers, cada um com frequencia direfente
    int valor_chuva = 0;
    int valor_agua = 0;

    while (true)
    {
        // Receber os valores da fila do buzzer (bloqueia até ter dado)
        if (xQueueReceive(volume_chuva_buzzer, &valor_chuva, portMAX_DELAY) != pdPASS ||
            xQueueReceive(nivel_agua_buzzer, &valor_agua, portMAX_DELAY) != pdPASS)
        {
            // Se falhar, tenta novamente no próximo loop
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        // Checar condições para tocar buzzer
        if (valor_agua >= 70)
        {
            tocar_pwm_buzzer(BUZZER_A, 200); // se o nivel da água for maior que 70%, tocará o buzzer A por 200ms
        }

        if (valor_chuva >= 80)
        {
            tocar_pwm_buzzer(BUZZER_B, 50); // se o chuva da água for maior que 80%, tocará o buzzer B por 100 ms
        }
    }
}

void vTaskLed()
{
    inicializar_leds();
    int valor_chuva = 0;
    int valor_agua = 0;

    while (true)
    {
        // Receber os valores da fila do buzzer (bloqueia até ter dado)
        if (xQueueReceive(volume_chuva_led, &valor_chuva, portMAX_DELAY) != pdPASS ||
            xQueueReceive(nivel_agua_led, &valor_agua, portMAX_DELAY) != pdPASS)
        {
            // Se falhar, tenta novamente no próximo loop
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        // se o nivel da água for maior que 70%, piscará o led vermelho com periodo de 100ms
        if (valor_agua >= 70)
        {
            piscar_led_vermelho();
        }
        // se o volume da chuva for maior que 80%, piscará o led vermelho com periodo de 100ms
        if (valor_chuva >= 80)
        {
            piscar_led_azul();
        }
    }
}

void vTaskMatriz()
{
    configurar_matriz_leds(); // inicializa e configura o PIO da matriz de leds
    int valor_chuva = 0;
    int valor_agua = 0;

    while (true)
    {
        // Receber os valores da fila do buzzer (bloqueia até ter dado)
        if (xQueueReceive(volume_chuva_matriz, &valor_chuva, portMAX_DELAY) != pdPASS ||
            xQueueReceive(nivel_agua_matriz, &valor_agua, portMAX_DELAY) != pdPASS)
        {
            // Se falhar, tenta novamente no próximo loop
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }
        // se o nivel da água for maior que 70%, e a chuva for maior que 80%, piscará um x na matriz com periodo de 100ms
        if (valor_agua >= 70 && valor_chuva >= 80)
        {
            desenha_fig(alerta_critico, BRILHO_PADRAO, pio, sm);
            vTaskDelay(pdMS_TO_TICKS(200));
            desenha_fig(matriz_apagada, BRILHO_PADRAO, pio, sm);
        }
        else if (valor_agua >= 70)// se o nivel da água for maior que 70%, piscará uma exclamação na matriz com periodo de 100ms
        {
            desenha_fig(alerta_agua, BRILHO_PADRAO, pio, sm);
            vTaskDelay(pdMS_TO_TICKS(200));
            desenha_fig(matriz_apagada, BRILHO_PADRAO, pio, sm);
        }
        else if (valor_chuva >= 80)// se o volume da chuva for maior que 80%, piscará um quadrado na matriz com periodo de 100ms
        {
            desenha_fig(alerta_chuva, BRILHO_PADRAO, pio, sm);
            vTaskDelay(pdMS_TO_TICKS(200));
            desenha_fig(matriz_apagada, BRILHO_PADRAO, pio, sm);
        }
    }
}

/*******************************************************

                      FUNÇÕES AUXILIARES

****************************************************** */

// Trecho para modo BOOTSEL com botão B
#include "pico/bootrom.h"
#define botaoB 6
void gpio_irq_handler(uint gpio, uint32_t events)
{
    reset_usb_boot(0, 0);
}

void desenha_fig(uint32_t *_matriz, uint8_t _intensidade, PIO pio, uint sm) // FUNÇÃO PARA DESENHAR O SEMAFORO NA MATRIZ
{
    uint32_t pixel = 0;
    uint8_t r, g, b;

    for (int i = 24; i > 19; i--) // Linha 1
    {
        pixel = _matriz[i];
        b = ((pixel >> 16) & 0xFF) * (_intensidade / 100.00); // Isola os 8 bits mais significativos (azul)
        g = ((pixel >> 8) & 0xFF) * (_intensidade / 100.00);  // Isola os 8 bits intermediários (verde)
        r = (pixel & 0xFF) * (_intensidade / 100.00);         // Isola os 8 bits menos significativos (vermelho)
        pixel = 0;
        pixel = (g << 16) | (r << 8) | b;
        pio_sm_put_blocking(pio, sm, pixel << 8u);
    }

    for (int i = 15; i < 20; i++) // Linha 2
    {
        pixel = _matriz[i];
        b = ((pixel >> 16) & 0xFF) * (_intensidade / 100.00); // Isola os 8 bits mais significativos (azul)
        g = ((pixel >> 8) & 0xFF) * (_intensidade / 100.00);  // Isola os 8 bits intermediários (verde)
        r = (pixel & 0xFF) * (_intensidade / 100.00);         // Isola os 8 bits menos significativos (vermelho)
        pixel = 0;
        pixel = (b << 16) | (r << 8) | g;
        pixel = (g << 16) | (r << 8) | b;
        pio_sm_put_blocking(pio, sm, pixel << 8u);
    }

    for (int i = 14; i > 9; i--) // Linha 3
    {
        pixel = _matriz[i];
        b = ((pixel >> 16) & 0xFF) * (_intensidade / 100.00); // Isola os 8 bits mais significativos (azul)
        g = ((pixel >> 8) & 0xFF) * (_intensidade / 100.00);  // Isola os 8 bits intermediários (verde)
        r = (pixel & 0xFF) * (_intensidade / 100.00);         // Isola os 8 bits menos significativos (vermelho)
        pixel = 0;
        pixel = (g << 16) | (r << 8) | b;
        pio_sm_put_blocking(pio, sm, pixel << 8u);
    }

    for (int i = 5; i < 10; i++) // Linha 4
    {
        pixel = _matriz[i];
        b = ((pixel >> 16) & 0xFF) * (_intensidade / 100.00); // Isola os 8 bits mais significativos (azul)
        g = ((pixel >> 8) & 0xFF) * (_intensidade / 100.00);  // Isola os 8 bits intermediários (verde)
        r = (pixel & 0xFF) * (_intensidade / 100.00);         // Isola os 8 bits menos significativos (vermelho)
        pixel = 0;
        pixel = (g << 16) | (r << 8) | b;
        pio_sm_put_blocking(pio, sm, pixel << 8u);
    }

    for (int i = 4; i > -1; i--) // Linha 5
    {
        pixel = _matriz[i];
        b = ((pixel >> 16) & 0xFF) * (_intensidade / 100.00); // Isola os 8 bits mais significativos (azul)
        g = ((pixel >> 8) & 0xFF) * (_intensidade / 100.00);  // Isola os 8 bits intermediários (verde)
        r = (pixel & 0xFF) * (_intensidade / 100.00);         // Isola os 8 bits menos significativos (vermelho)
        pixel = 0;
        pixel = (g << 16) | (r << 8) | b;
        pio_sm_put_blocking(pio, sm, pixel << 8u);
    }
}

void configurar_matriz_leds() // FUNÇÃO PARA CONFIGURAR O PIO PARA USAR NA MATRIZ DE LEDS
{
    bool clock_setado = set_sys_clock_khz(133000, false);
    pio = pio0;
    sm = pio_claim_unused_sm(pio, true);
    uint offset = pio_add_program(pio, &Matriz_5x5_program);
    Matriz_5x5_program_init(pio, sm, offset, MATRIZ_PIN);
}

void inicializar_pinos_adc()
{
    adc_gpio_init(ADC_JOYSTICK_Y);
    adc_gpio_init(ADC_JOYSTICK_X);
    adc_init();
}

void inicializar_display_i2c()
{ // FUNÇÃO PARA INICIALIZAR O I2C DO DISPLAY
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

void inicializar_pwms_buzzers()
{
    // Buzzer A - grave audível (~2 kHz)
    gpio_set_function(BUZZER_A, GPIO_FUNC_PWM);
    uint slice_a = pwm_gpio_to_slice_num(BUZZER_A);
    pwm_set_wrap(slice_a, 62499);
    pwm_set_chan_level(slice_a, pwm_gpio_to_channel(BUZZER_A), 31250);
    pwm_set_enabled(slice_a, false);

    // Buzzer B - agudo (~5 kHz)
    gpio_set_function(BUZZER_B, GPIO_FUNC_PWM);
    uint slice_b = pwm_gpio_to_slice_num(BUZZER_B);
    pwm_set_wrap(slice_b, 24999);
    pwm_set_chan_level(slice_b, pwm_gpio_to_channel(BUZZER_B), 12500);
    pwm_set_enabled(slice_b, false);
}

void draw_ssd1306(uint32_t *_matriz)
{ // FUNÇÃO PARA DESENHAR NO DISPLAY COM O CÓDIGO EXPORTADO DO PISKEL
    for (int i = 0; i < 8192; i++)
    {
        int x = i % 128; // coluna
        int y = i / 128; // linha

        if (_matriz[i] > 0x00000000)
        {
            ssd1306_pixel(&ssd, x, y, true);
        }
    }
}

void inicializar_leds()
{
    // Inicializa o pino do LED vermelho
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_put(LED_VERMELHO, 0); // Desliga o LED inicialmente

    // Inicializa o pino do LED azul
    gpio_init(LED_AZUL);
    gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_put(LED_AZUL, 0); // Desliga o LED inicialmente

    // Inicializa o pino do LED azul
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_put(LED_VERDE, 0); // Desliga o LED inicialmente
}

void tocar_pwm_buzzer(uint gpio_pin, uint duracao_ms)
{
    uint slice_num = pwm_gpio_to_slice_num(gpio_pin);
    pwm_set_enabled(slice_num, true);
    vTaskDelay(pdMS_TO_TICKS(duracao_ms));
    pwm_set_enabled(slice_num, false);
}

void piscar_led_vermelho()
{
    gpio_put(LED_VERMELHO, 1); // Liga LED vermelho
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_put(LED_VERMELHO, 0); // Desliga LED vermelho
}

void piscar_led_azul()
{
    gpio_put(LED_AZUL, 1); // Liga LED azul
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_put(LED_AZUL, 0); // Desliga LED azul
}

int main()
{

    // Para ser utilizado o modo BOOTSEL com botão B
    gpio_init(botaoB);
    gpio_set_dir(botaoB, GPIO_IN);
    gpio_pull_up(botaoB);
    gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    // Fim do trecho para modo BOOTSEL com botão B
    stdio_init_all();

    // Cria as 8 finas, 2 para cada tarefa que usará elas(a tarefa vLerJoystick apenas alimenta elas, não usa de fato)
    volume_chuva_display = xQueueCreate(10, sizeof(int));
    nivel_agua_display = xQueueCreate(10, sizeof(int));
    volume_chuva_buzzer = xQueueCreate(10, sizeof(int));
    nivel_agua_buzzer = xQueueCreate(10, sizeof(int));
    volume_chuva_led = xQueueCreate(10, sizeof(int));
    nivel_agua_led = xQueueCreate(10, sizeof(int));
    volume_chuva_matriz = xQueueCreate(10, sizeof(int));
    nivel_agua_matriz = xQueueCreate(10, sizeof(int));

    //Faz a chamada das 5 tarefas do sistema
    xTaskCreate(vLerJoystick, "Ler joystick", 256, NULL, 1, NULL);
    xTaskCreate(vAtualizarDisplay, "Atualizar display", 256, NULL, 1, NULL);
    xTaskCreate(vTocarBuzzer, "Tocar buzzer", 256, NULL, 1, NULL);
    xTaskCreate(vTaskLed, "Alerta Matriz", 256, NULL, 1, NULL);
    xTaskCreate(vTaskMatriz, "Alerta Matriz", 256, NULL, 1, NULL);
    vTaskStartScheduler();

    panic_unsupported();
}
