// Push-button com função toogle no LED onboard
#include "stm32f1xx.h" // Biblioteca STM

char wait = 0;
char yellow = 0;
char red = 0;

/* Tratamento da interrupção do SysTick (ocorre a cada 100 ms)*/
void SysTick_Handler (void) {
    if(wait > 0) {
        wait--;
    } else {
        // Desabilitar o LED Verde

        // Habilitar o LED Amarelo

        // Tempo do LED Amarelo
        yellow = 20;
    }

    if(yellow > 0) {
        yellow--;
    } else {
        // Desabilitar o LED Amarelo

        // Habilitar o LED Vermelho

        // Tempo do LED Vermelho
        red = 30;
    }

    if(red > 0) {
        red--;
    } else {
       // Desabilitar o LED Vermelho

       // Habilitar o LED Verde 
       
       EXTI->IMR |= EXTI_IMR_IM0; // Hab. a int. do EXTI0
    }
}

/* Tratamento da interrupção do EXTI0 */
void EXTI0_IRQHandler (void) {
    EXTI->PR = 1;               // Apaga flag sinalizadora de interrupção
    EXTI->IMR &= ~EXTI_IMR_IM0; // Desab. a int. do EXTI0

    // Tempo para ativar LED Vmarelo
    wait = 30;
}

int main(void){
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN; // Habilita clock do barramento APB2 para GPIOC
     
    // PC13 como saida open-drain de 2 MHz - LED Verde
    GPIOC->CRH = (GPIOC->CRH & ~(GPIO_CRH_MODE13_Msk | GPIO_CRH_CNF13_Msk)) | (0b0110 << GPIO_CRH_MODE13_Pos); 
    // PC14 como saida open-drain de 2 MHz - LED Amarelo

    // PC15 como saida open-drain de 2 MHz - LED Vermelho

    // PA0 como entrada... 
    GPIOA->CRL = (GPIOA->CRL & ~(GPIO_CRL_MODE0_Msk | GPIO_CRL_CNF0_Msk)) | (0b1000 << GPIO_CRL_MODE0_Pos);
    GPIOA->ODR |= GPIO_ODR_ODR0;    // ... e com resistor pull-up interno

    // Habilitar LED Verde???
    GPIOC->ODR ^= (1<<13);

    /* Config. SysTick com interrupção a cada 1OO ms ->  10 Hz */     
    SysTick->LOAD = 800000;     // (default BluePill: HSI com 8MHz)
	SysTick->VAL = 0;		    // Limpa o valor da contagem
    SysTick->CTRL = 0b111; 	    // Clock do processador sem dividir, Hab. IRQ e SysTick */

    /* Config PAO interrupção no EXTI0 */
    AFIO->EXTICR[0] = AFIO_EXTICR1_EXTI0_PA;        // Seleciona PA0 para EXTI0
    EXTI->FTSR = EXTI_FTSR_FT0;                     // Sensível na rampa de descida
    EXTI->IMR = EXTI_IMR_IM0;                       // Hab. máscara de interrup. do EXTI0
    //NVIC->IP[EXTI0_IRQn] = 14;                    // Config. prioridade do EXTI0
    NVIC->ISER[0] = (uint32_t)(1 << EXTI0_IRQn);    // Hab. IRQ do EXTI0 na NVIC
    
    for(;;){ /* Loop infinito */
    }
}