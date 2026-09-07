#include "button.h"

void button_cfg(void){
    RCC -> APB2ENR |= 0x00000008;
    GPIOB->CRH &= ~(0xF << 28);        
    GPIOB -> CRH |= 0x80000000;
    GPIOB->ODR |= (1 << 15);
}

void button_debouce (void){
    static uint8_t bt_pressed = 0;
    // trang thai nhan
    if((GPIOB -> IDR & (1<<15)) == 0){
        delay(20);
        if((GPIOB -> IDR & (1 << 15)) == 0){
            bt_pressed = 1;
        }
    }

    // Trang thai tha
    else if(bt_pressed == 1){
        delay(20);
        if((GPIOB -> IDR & (1<<15)) != 0){
            GPIOA -> ODR ^= (1 << 4);
            bt_pressed = 0;
        }
    }
}