#include <REGX52.H>

/* ============================================================
   HARDWARE DEFINITION
   ============================================================ */
#define LED_PORT     P1      // 8 LED active-low: 0 = ON, 1 = OFF
#define SEG_PORT     P0      // 7-segment data port
#define SEL_PORT     P2      // 7-segment digit selection

/* ============================================================
   7-SEGMENT DISPLAY PATTERNS
   ============================================================ */
unsigned char code seg_table[11] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,  // 0-4
    0x6D, 0x7D, 0x07, 0x7F, 0x6F,  // 5-9
    0x00                            // blank
};

// Buffer for 8 digits (right to left: [0] = rightmost)
volatile unsigned char disp_buf[8] = {10,10,10,10,10,10,10,10};
volatile unsigned char disp_idx = 0;

/* ============================================================
   CONTROL VARIABLES
   ============================================================ */
volatile unsigned int sys_ms = 0;           // System tick counter
volatile unsigned char mode = 0;            // 0=AUTO, 1-5=individual effects
volatile unsigned int speed_ms = 100;       // Effect step delay (100-1500ms)

/* Button control */
volatile bit btn_mode_req = 0;
volatile bit btn_time_req = 0;
volatile bit btn_mode_lock = 0;
volatile bit btn_time_lock = 0;
volatile unsigned int btn_mode_ts = 0;
volatile unsigned int btn_time_ts = 0;

/* UART control */
volatile unsigned char uart_char = 0;
volatile bit uart_ready = 0;

/* ============================================================
   EFFECT STATE VARIABLES
   ============================================================ */

// Effect 1: Shift Left - LED chay tu phai sang trai
static unsigned char e1_pos = 0x01;

// Effect 2: Waterfall - LED do dan nhu thac nuoc
static unsigned char e2_stage = 0;
static unsigned char e2_fill = 0x00;
static unsigned char e2_run = 0x01;

// Effect 3: Fade Out - Tat dan tu tat ca xuong 0
static unsigned char e3_pat = 0xFF;

// Effect 4: Converge - Sang don vao tu 2 ben
static unsigned char e4_left = 0x01;
static unsigned char e4_right = 0x80;
//static unsigned char e4_state = 0;
//static unsigned char e4_count = 0;

// Effect 5: Fade In - Sang dan tu 0 len tat ca
static unsigned char e5_pat = 0x00;

// AUTO Mode control
static unsigned char auto_eff = 1;      // Hieu ung dang chay (1-5)
static unsigned char auto_done = 0;     // Co bao hieu ung hoan thanh

/* ============================================================
   EFFECT INITIALIZATION
   ============================================================ */
void reset_effects(void) {
    e1_pos = 0x01;
    e2_stage = 0;
    e2_fill = 0x00;
    e2_run = 0x01;
    e3_pat = 0xFF;
    e4_left = 0x01;
    e4_right = 0x80;
		//e4_state = 0;
		//e4_count = 0;
    e5_pat = 0x00;
    auto_eff = 1;
    auto_done = 0;
}

/* ============================================================
   EFFECT FUNCTIONS
   ============================================================ */

/* EFF1: SHIFT LEFT
   Hoat dong: LED chay tu phai sang trai
   0000 0001 -> 0000 0010 -> ... -> 1000 0000 -> loop
*/
bit eff1(void) {
    LED_PORT = ~e1_pos;
    e1_pos <<= 1;
    
    if (e1_pos == 0) {
        e1_pos = 0x01;
        return 1;  // Done
    }
    return 0;
}

/* EFF2: WATERFALL
   Hoat dong: LED do dan nhu thac nuoc
   Stage 0: LED chay tu phai sang trai
   Stage 1-7: "Do day" dan dan tu trai sang phai
*/
bit eff2(void) {
    if (e2_stage == 0) {
        LED_PORT = ~e2_run;
        e2_run <<= 1;
        
        if (e2_run == 0) {
            e2_stage = 1;
            e2_fill = 0x80;
            e2_run = 0x01;
        }
        return 0;
    }
    
    LED_PORT = ~(e2_fill | e2_run);
    e2_run <<= 1;
    
    if (e2_run == (1 << (8 - e2_stage))) {
        e2_fill |= (e2_fill >> 1);
        e2_stage++;
        e2_run = 0x01;
        
        if (e2_stage >= 8) {
            e2_stage = 0;
            e2_fill = 0x00;
            e2_run = 0x01;
            return 1;  // Done
        }
    }
    return 0;
}

/* EFF3: FADE OUT
   Hoat dong: Tat dan tu tat ca xuong 0
   1111 1111 -> 0111 1111 -> ... -> 0000 0000
*/
bit eff3(void) {
    LED_PORT = ~e3_pat;
    
    if (e3_pat == 0x00) {
        e3_pat = 0xFF;
        return 1;  // Done
    }
    
    e3_pat >>= 1;
    return 0;
}

/* EFF4: CONVERGE
   Hoat dong: Sang don vao tu 2 ben vao giua
   1000 0001 -> 0100 0010 -> 0010 0100 -> 0001 1000 -> reset
*/
bit eff4(void) {
    LED_PORT = ~(e4_left | e4_right);
    
    e4_left <<= 1;
    e4_right >>= 1;
    
    // Kiem tra 2 LED gap nhau o giua
    if (e4_left >= 0x10 || e4_right <= 0x08) {
        e4_left = 0x01;
        e4_right = 0x80;
        return 1;  // Done
    }
		/* blink
		e4_state = !e4_state;
		if(e4_state) LED_PORT = 0x00;
		else LED_PORT = 0xFF;
		e4_count++;
		if(e4_count>16){
				e4_count = 0;
				return 1;
		}
		*/
    return 0;
}

/* EFF5: FADE IN
   Hoat dong: Sang dan tu 0 len tat ca
   0000 0000 -> 0000 0001 -> 0000 0011 -> ... -> 1111 1111
*/
bit eff5(void) {
    LED_PORT = ~e5_pat;
    
    if (e5_pat == 0xFF) {
        e5_pat = 0x00;
        return 1;  // Done
    }
    
    e5_pat = (e5_pat << 1) | 0x01;
    return 0;
}

/* ============================================================
   EFFECT CONTROLLER
   ============================================================ */
void run_effect(void) {
    if (mode == 0) {
        /* AUTO MODE: chay lan luot 5 hieu ung */
        switch (auto_eff) {
            case 1: auto_done = eff1(); break;
            case 2: auto_done = eff2(); break;
            case 3: auto_done = eff3(); break;
            case 4: auto_done = eff4(); break;
            case 5: auto_done = eff5(); break;
        }
        
        if (auto_done) {
            auto_done = 0;
            auto_eff++;
            
            if (auto_eff > 5) {
                auto_eff = 1;
            }
            
            // Reset trang thai hieu ung tiep theo
            switch (auto_eff) {
                case 1: e1_pos = 0x01; break;
                case 2: 
                    e2_stage = 0;
                    e2_fill = 0x00;
                    e2_run = 0x01;
                    break;
                case 3: e3_pat = 0xFF; break;
                case 4: 
                    e4_left = 0x01;
                    e4_right = 0x80;
                    break;
                case 5: e5_pat = 0x00; break;
            }
        }
    } else {
        /* MANUAL MODE: chay 1 hieu ung co dinh */
        switch (mode) {
            case 1: eff1(); break;
            case 2: eff2(); break;
            case 3: eff3(); break;
            case 4: eff4(); break;
            case 5: eff5(); break;
        }
    }
}

/* ============================================================
   UART FUNCTIONS
   ============================================================ */
void uart_init(void) {
    SCON = 0x50;
    TMOD &= 0x0F;
    TMOD |= 0x20;
    TH1 = 0xFD;
    TL1 = 0xFD;
    TR1 = 1;
    ES = 1;
}

void uart_putc(char c) {
    SBUF = c;
    while (!TI);
    TI = 0;
}

void uart_puts(char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

void uart_nl(void) {
    uart_putc('\r');
    uart_putc('\n');
}

void uart_menu(void) {
    uart_nl();
    uart_puts("=== LED CONTROL v1.0 ===");
    uart_nl();
    uart_puts("[0] AUTO  [1] Shift");
    uart_nl();
    uart_puts("[2] Water [3] FadeOut");
    uart_nl();
    uart_puts("[4] Convg [5] FadeIn");
    uart_nl();
    uart_puts("[+] Inc   [-] Dec");
    uart_nl();
    uart_puts("[M] Menu  [S] Status");
    uart_nl();
}

void uart_status(void) {
    uart_nl();
    uart_puts("Mode:");
    uart_putc('0' + mode);
    uart_puts(" Delay:");
    uart_putc('0' + (speed_ms / 1000));
    uart_putc('0' + ((speed_ms / 100) % 10));
    uart_putc('0' + ((speed_ms / 10) % 10));
    uart_putc('0' + (speed_ms % 10));
    uart_puts("ms");
    uart_nl();
}

void uart_cmd(char cmd) {
    if (cmd >= '0' && cmd <= '5') {
        mode = cmd - '0';
        reset_effects();
        uart_puts("> Mode:");
        uart_putc(cmd);
        uart_nl();
    }
    else if (cmd == '+') {
        if (speed_ms < 1500) {
            speed_ms += 200;
            uart_puts("> Time Inc");
            uart_nl();
        }
    }
    else if (cmd == '-') {
        if (speed_ms > 100) {
            speed_ms -= 200;
            uart_puts("> Time Dec");
            uart_nl();
        }
    }
    else if (cmd == 'M' || cmd == 'm') {
        uart_menu();
    }
    else if (cmd == 'S' || cmd == 's') {
        uart_status();
    }
}

/* ============================================================
   7-SEGMENT DISPLAY
   ============================================================ */
unsigned char count_led(void) {
    unsigned char i, cnt = 0;
    unsigned char val = LED_PORT;
    
    for (i = 0; i < 8; i++) {
        if ((val & (1 << i)) == 0) {
            cnt++;
        }
    }
    return cnt;
}

void update_display(void) {
    unsigned int t = speed_ms;
    unsigned char n = count_led();
    
    disp_buf[7] = mode;
    disp_buf[6] = 10;
    disp_buf[5] = (n <= 9) ? n : 9;
    disp_buf[4] = 10;
    disp_buf[3] = (unsigned char)(t / 1000);
    disp_buf[2] = (unsigned char)((t / 100) % 10);
    disp_buf[1] = (unsigned char)((t / 10) % 10);
    disp_buf[0] = (unsigned char)(t % 10);
}

/* ============================================================
   INTERRUPT SERVICE ROUTINES
   ============================================================ */

/* Timer0: 7-segment multiplexing + time base */
void timer0_isr(void) interrupt 1 {
    TH0 = 0xF8;
    TL0 = 0x30;
    
    SEL_PORT = (disp_idx << 2);
    SEG_PORT = seg_table[disp_buf[disp_idx]];
    
    disp_idx++;
    disp_idx &= 0x07;
    
    sys_ms += 2;
}

/* INT0: Mode button (P3.2) */
void int0_isr(void) interrupt 0 {
    if (!btn_mode_lock) {
        btn_mode_lock = 1;
        btn_mode_ts = sys_ms;
        btn_mode_req = 1;
    }
}

/* INT1: Time button (P3.3) */
void int1_isr(void) interrupt 2 {
    if (!btn_time_lock) {
        btn_time_lock = 1;
        btn_time_ts = sys_ms;
        btn_time_req = 1;
    }
}

/* UART: Receive character */
void uart_isr(void) interrupt 4 {
    if (RI) {
        RI = 0;
        uart_char = SBUF;
        uart_ready = 1;
    }
}

/* ============================================================
   TIMER INITIALIZATION
   ============================================================ */
void timer0_init(void) {
    TMOD &= 0xF0;
    TMOD |= 0x01;
    TH0 = 0xF8;
    TL0 = 0x30;
    ET0 = 1;
    TR0 = 1;
}

/* ============================================================
   MAIN PROGRAM
   ============================================================ */
void main(void) {
    unsigned int last_t = 0;

    /* Hardware init */
    LED_PORT = 0xFF;
    SEG_PORT = 0x00;
    SEL_PORT = 0x00;
    P3 |= 0x0C;

    /* Interrupt config */
    IT0 = 1;
    IT1 = 1;
    EX0 = 1;
    EX1 = 1;

    /* System init */
    reset_effects();
    timer0_init();
    uart_init();
    EA = 1;

    /* Welcome */
    uart_menu();
    uart_status();

    /* Main loop */
    while (1) {
        /* Button debouncing */
        if (btn_mode_lock) {
            if ((unsigned int)(sys_ms - btn_mode_ts) >= 30) {
                if (P3_2 == 1) {
                    btn_mode_lock = 0;
                }
            }
        }
        
        if (btn_time_lock) {
            if ((unsigned int)(sys_ms - btn_time_ts) >= 30) {
                if (P3_3 == 1) {
                    btn_time_lock = 0;
                }
            }
        }

        /* Button processing */
        if (btn_mode_req) {
            btn_mode_req = 0;
            mode++;
            if (mode > 5) mode = 0;
            reset_effects();
            last_t = sys_ms;
            uart_status();
        }

        if (btn_time_req) {
            btn_time_req = 0;
            if (speed_ms < 1500) {
                speed_ms += 200;
            } else {
                speed_ms = 100;
            }
            uart_status();
        }

        /* UART processing */
        if (uart_ready) {
            uart_ready = 0;
            uart_cmd(uart_char);
        }

        /* Effect timing */
        if ((unsigned int)(sys_ms - last_t) >= speed_ms) {
            last_t = sys_ms;
            run_effect();
        }

        /* Display update */
        update_display();
    }
}