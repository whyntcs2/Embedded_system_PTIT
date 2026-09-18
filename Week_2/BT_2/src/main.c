#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "misc.h"
#include "LED.h"
#include "systick.h"


int main (){
    gpio_cfg();
    systick_cfg();
    while(1){
        
    }
}


