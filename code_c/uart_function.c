// khởi tạo UART với tốc độ baud 9600
void UART_Init(void) {
    SCON = 0x50; // Chế độ 1, 8-bit UART, REN=1 (cho phép nhận)

    TMOD &= 0x0F; // Xóa các bit chế độ Timer 1
    TMOD |= 0x20; // Chọn Timer 1 ở mode 2 (auto-reload)

    TH1 = 0xFD;   // Tốc độ baud 9600 với fosc = 11.0592 MHz,
                  // SMOD = 0 (mặc định không nhân đôi)

    TL1 = 0xFD;
    TR1 = 1;      // Kích hoạt Timer 1

    TI = 1;       // Sẵn sàng để truyền byte đầu tiên
}

// Gửi một ký tự qua UART
void UART_SendChar(char c) {
    while (!TI);  // Chờ cho đến khi cờ TI = 1 (truyền xong byte trước)
    TI = 0;       // Xóa cờ TI
    SBUF = c;     // Gửi ký tự
}

// Nhận một ký tự (dùng kỹ thuật kiểm tra liên tục – Polling)
char UART_GetChar(void) {
    while (!RI);   // Chờ đến khi cờ RI = 1 (nhận xong dữ liệu)
    RI = 0;        // Xóa cờ RI
    return SBUF;   // Trả về ký tự nhận được
}
// Truyền một chuỗi
void UART_SendString(char *str) {
    while (*str) {             // Lặp cho đến khi gặp ký tự NULL
        UART_SendChar(*str);   // Gửi ký tự hiện tại
        str++;                 // Chuyển đến ký tự tiếp theo
    }
}
// Nhận một chuỗi với kích thước cố định
void UART_GetString(char *buffer, unsigned int max_length) {
    unsigned int i = 0;
    char c;

    do {
        c = UART_GetChar();      // Nhận một ký tự

        if (c == '\r') {         // Nếu là Enter thì kết thúc
            buffer[i] = '\0';    // Thêm ký tự kết thúc chuỗi
            break;
        } else {
            buffer[i++] = c;     // Lưu ký tự vào buffer
        }
    }while (i < max_length - 1); // Đảm bảo còn 1 byte cho ký tự '\0'
    // Nếu buffer đầy mà chưa gặp Enter
    if (i == max_length - 1) {
        buffer[i] = '\0'; // Đảm bảo chuỗi được kết thúc
    }
}
// Nhận 1 chuỗi cho đến khi gặp ký tự kết thúc
void UART_GetStringUntil(char *buffer, unsigned int max_length, char end_char) {
    unsigned int i = 0;
    char c;

    do {
        c = UART_GetChar();      // Nhận một ký tự

        if (c == end_char) {     // Nếu là ký tự kết thúc thì dừng
            buffer[i] = '\0';    // Thêm ký tự kết thúc chuỗi
            break;
        } else {
            buffer[i++] = c;     // Lưu ký tự vào buffer
        }

    } while (i < max_length - 1); // Để 1 byte cho ký tự '\0'

    // Nếu buffer đầy mà chưa gặp ký tự kết thúc
    if (i == max_length - 1) {
        buffer[i] = '\0';        // Đảm bảo chuỗi được kết thúc
    }
}
// Truyền nhận dữ liệu số
// 1. Truyền nhận số nguyên
void UART_SendInt(int num) {
    char buffer[7];   // Đủ cho số int 16-bit (-32768 đến 32767) + ký tự '\0'

    sprintf(buffer, "%d", num);  // TV: #include <stdio.h>
    UART_SendString(buffer);
}
// 2. Truyền nhận số thực
void UART_SendFloat(float num) {
    char buffer[15];
    sprintf(buffer, "%f", num);
    UART_SendString(buffer);
}
// Xử lý ngắt UART (Đọc ký tự dùng kỹ thuật Interrupt)
void UART_EnableInterrupt(void) {
    ES = 1;   // Cho phép ngắt UART
    EA = 1;   // Cho phép ngắt toàn cục
}
void UART_ISR(void) interrupt 4 {
    if (RI) {
        // Xử lý nhận dữ liệu
        char received_char = SBUF;
        RI = 0;            // Xóa cờ ngắt nhận

        // Xử lý ký tự nhận được
        // Ví dụ: Echo – phản hồi lại ký tự vừa nhận
        UART_SendChar(received_char);
    }

    // Xử lý truyền dữ liệu (Hiện tại không dùng)
    // if (TI) {
    //    TI = 0;           // Xóa cờ ngắt truyền
    //    // Có thể thêm xử lý sau khi truyền xong
    // }
}
