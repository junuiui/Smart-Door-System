    #include <stdint.h>
    #include <stdbool.h>
    #include <pru_cfg.h>
    #include "resource_table_empty.h"
    #include "shared_data_struct.h"

    #define DELAY_1_S 200000000
    #define DELAY_1_MS 200000
    #define STR_LEN 8  // # LEDs our string
    #define oneCyclesOn 700/5   // Stay on 700ns
    #define oneCyclesOff 800/5
    #define zeroCyclesOn 350/5
    #define zeroCyclesOff 600/5
    #define DATA_PIN 15  // Bit number to output on

    volatile register uint32_t __R30;   // Output GPIO register
    volatile register uint32_t __R31;   // Input GPIO register

    // Shared Memory Configuration
    // -----------------------------------------------------------
    #define THIS_PRU_DRAM 0x00000 // Address of DRAM
    #define OFFSET 0x200 // Skip 0x100 for Stack,

    // 0x100 for Heap (from makefile)
    #define THIS_PRU_DRAM_USABLE (THIS_PRU_DRAM + OFFSET)
    // This works for both PRU0 and PRU1 as both map their own memory to 0x0000000
    volatile shared_data_struct* shared_data = (volatile void *) THIS_PRU_DRAM_USABLE;

    void main(){

        shared_data->value_ms = 0;

        while (true) {

            __delay_cycles(DELAY_1_MS);

            if(shared_data->value_ms > 0){
                shared_data->value_ms -= 1;
            }

            /*for(int j = 0; j < STR_LEN; j++) {
                for(int i = 31; i >= 0; i--) {
                    if(shared_data->colors[j] & ((uint32_t)0x1 << i)) {
                        __R30 |= 0x1<<DATA_PIN;      // Set the GPIO pin to 1
                        __delay_cycles(oneCyclesOn-1);
                        __R30 &= ~(0x1<<DATA_PIN);   // Clear the GPIO pin
                        __delay_cycles(oneCyclesOff-2);
                    } else {
                        __R30 |= 0x1<<DATA_PIN;      // Set the GPIO pin to 1
                        __delay_cycles(zeroCyclesOn-1);
                        __R30 &= ~(0x1<<DATA_PIN);   // Clear the GPIO pin
                        __delay_cycles(zeroCyclesOff-2);
                    }
                }
            }

            // Send Reset
            __R30 &= ~(0x1<<DATA_PIN);   // Clear the GPIO pin
            __delay_cycles(DELAY_1_MS);
            */      
        }
        __halt();

    }
