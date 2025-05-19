# SISTEMA DE MONITORAMENTO DE CHEIAS

O projeto tem por finalidade criar um sistema de alerta para cheias, operando dois modos, normal e alerta, oferecendo sinais visuais e sonoros garantindo a segurança de todos, incluindo deficientes visuais. Além disso o projeto usa conceitos de multitarefas para gerenciar diferentes periféricos ao mesmo tempo.

## Componentes Utilizados


1. **Botão Pushbutton**
2. **Display OLED 1306**
3. **Buzzer**
4. **Matriz de LED 5x5 WS2812** 
5. **Led RGB**
6. Joystick

## Funcionalidade

Ao inciar o o sistema estára operando no modo normal com valores de volume de chuva e água proximo a 50%, tais informações são exibidas em tempo real no display

🚨 Alerta de Enchente:
Quando o nível da água for igual ou superior a 70%, o sistema entrará em modo de alerta com os seguintes comportamentos:

🔊 Buzzer: toca um som grave por 200ms enquanto o nível permanecer acima de 70%.

🔴 LED RGB: pisca em vermelho com frequencia de 100ms.

💡 Matriz de LEDs: pisca uma exclamação "!" com frequencia de 200ms.

🖥️ Display OLED:

 -- Exibe os dados em tempo real dos sensores.

 -- Mostra a mensagem: ALERTA !!
 -- Mostra o desenho de uma nuvem

 🌧️🚨 Alerta de Chuvas Fortes:
Quando o volume de chuva for igual ou superior a 80%, o sistema entra em modo de alerta com os seguintes comportamentos:

🔊 Buzzer: emite um som agudo por 50ms, enquanto o nível permanecer acima de 80%.

🔵 LED RGB: pisca na cor azul com frequência de 100ms.

💡 Matriz de LEDs: pisca um quadrado com frequência de 200ms.

🖥️ Display OLED:

 -- Exibe os dados em tempo real dos níveis de água e chuva.
 -- Mostra a mensagem: ALERTA
 -- Mostra o desenho de uma casa imundada

 🚨 Alerta de Chuvas Fortes e enchentes:
Quando o nível de chuva for igual ou superior a 80% e de água igual ou supeiror a 70%, o sistema entra em modo de alerta com os seguintes comportamentos:

🔊 Buzzer: emite um som agudo por 50ms, ao mesmo tempo que o outro buzzer emite um som grave no periodo de 200ms.

🔵 LED RGB: pisca na cor azul com frequência de 100ms e o vermelho com frequência de 100 ms.

💡 Matriz de LEDs: pisca um X vermelho com frequência de 200ms.

🖥️ Display OLED:

 -- Exibe os dados em tempo real dos níveis de água e chuva.
 -- Mostra a mensagem: ALERTA
 -- Mostra o desenho de uma caveira

### Como Usar

#### Usando a BitDogLab

- Clone este repositório: git clone https://github.com/rober1o/estacao_alerta.git;
- Usando a extensão Raspberry Pi Pico importar o projeto;
- Ajuste o diretório do FreeRTOS, conforme seu computador
- Compilar o projeto;
- Plugar a BitDogLab usando um cabo apropriado e gravar o código.

## Demonstração

<!-- TODO: adicionar link do vídeo -->
Vídeo demonstrando as funcionalidades da solução implementada: [Demonstração](https://youtu.be/rlCmV45Yfp4)
