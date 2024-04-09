#include <stdint.h>
#include "stm32l053xx.h"

void consultarSaldo();
void realizarDeposito();
void realizarRetiro();
void USART_SendString(char *str);
void USART2_IRQHandler ();
void delay_ms(uint16_t n);
uint16_t saldo = 1000; // Saldo inicial

// Declaraciones de funciones
void USART_SendChar(char c);
void USART_SendInt(uint16_t num);
void procesarComando(char comando);

int main(void) {

    // CLK
    // periferico
    // porta

    RCC->IOPENR |= 1 << 0;

    // PA5
    GPIOA->MODER &= ~(1 << 11);

    // usart 2 pin configuracion
    // alternar la funcion modo  para PA2 y PA3
    GPIOA->MODER &= ~(1 << 4);  //[10]
    GPIOA->MODER &= ~(1 << 6);
    // seleccionar cual moder AF4 para usart
    GPIOA->AFR[0] |= 1 << 10;  // mapera PA2 a AF4
    GPIOA->AFR[0] |= 1 << 14;  // mapear PA3 a AF4

    // configuracion periferico del USART2
    // Clock enable RCC APB1ENR resgistro
    RCC->APB1ENR |= 1 << 17;
    // la tasa de los bits sera expulsado   9600 con
    USART2->BRR = 218;
    // habilitamos trasmisor y receptor
    USART2->CR1 |= (1 << 2) | (1 << 3);
    // habilitar el periferico
    USART2->CR1 |= 1 << 0;

    // Bucle infinito
    while (1) {
        // Esperar a que llegue un comando por USART
        while (!(USART2->ISR & USART_ISR_RXNE)); // Esperar a que llegue un byte
        char comando = USART2->RDR; // Leer el byte recibido

        // Procesar el comando
        procesarComando(comando);
    }
}

// Función para enviar un carácter a través de USART
void USART_SendChar(char c) {
    while (!(USART2->ISR & USART_ISR_TXE)); // Esperar a que el registro de transmisión esté vacío
    USART2->TDR = c; // Enviar carácter
}
// Función para enviar una cadena de caracteres a través de USART
void USART_SendString(char *str) {
    // Iterar sobre la cadena de caracteres hasta encontrar el carácter nulo '\0'
    while (*str != '\0') {
        // Enviar el carácter actual a través de USART
        USART_SendChar(*str++);
    }
}
// Función para enviar un entero a través de USART
void USART_SendInt(uint16_t num) {
    char buffer[10]; // Se reserva un espacio para el valor máximo de un uint16_t (5 dígitos) y el carácter nulo final
    sprintf(buffer, "%u\n", (unsigned int)num); // Se utiliza %u para enteros sin signo
    USART_SendString(buffer);
}

// Función para procesar el comando recibido por USART
void procesarComando(char comando) {
    switch(comando) {
        case 'A':
            consultarSaldo();
            break;
        case 'L':
            realizarDeposito();
            break;
        case 'N':
            realizarRetiro();
            break;
        default:
            USART_SendString("Comando no valido.\n");
            break;
    }
}

// Función para realizar un depósito
void realizarDeposito() {
    saldo +=60;// Monto fijo para el depósito
    USART_SendString("Deposito realizado con exito. Saldo actual: ");
    USART_SendInt(saldo);
}

// Función para realizar un retiro
void realizarRetiro() {
    int montoRetiro = 130;// Monto fijo para el retiro

    if (saldo >= montoRetiro) {
        saldo -= montoRetiro; // Restar monto del saldo
        USART_SendString("Retiro realizado con exito. Saldo actual: ");
        USART_SendInt(saldo);
    } else {
        USART_SendString("Saldo insuficiente para realizar el retiro.\n");
    }
}

// Función consultar saldo
void consultarSaldo() {
    USART_SendString("Saldo actual: ");
    USART_SendInt(saldo);
}

// Función de interrupción para USART2
void USART2_IRQHandler(void) {
    if (USART2->ISR & USART_ISR_RXNE) { // Hay un byte recibido
        char comando = USART2->RDR; // Leer el byte recibido
        procesarComando(comando); // Procesar el comando recibido
    }
}




