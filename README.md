# THỰC TẬP KIẾN TRÚC VÀ TỔ CHỨC MÁY TÍNH 

### 1. MÔ TẢ
- Link tải KeilC(AT89C51) và Proteus: 
https://drive.google.com/drive/folders/1FDUzu3VYYfmMQK9li1eveh-XFw6xOYeM?usp=sharing

- Lập trình 8051 (ams và C)

- Sử dụng kit: https://github.com/giunzz/COOA_LABS/blob/main/proteus/kitTT_ver8.17.pdsprj 
 * Chú ý: chân LED đơn P1, LED7seg: P2,P0
 * Uart P3.0, P3.1, ngắt INT0 P3.2 , INT1 P3.3

- Bảng chuyển đổi cơ số 

### 2. BÁO CÁO 

#### Tuần 1: ASM
- Tổng quan về dòng 8051 (VDK:89S52)

– Tập lệnh ASM của 8051
You can view my full report here: Report (PDF)[]

#### Tuần 2: ASM

- Nhập xuất GPIO

- Timer - Counter

You can view my full report here: Report (PDF)



#### Tuần 3: ASM

- Ngắt 
- Interrupt 

You can view my full report here: Report (PDF)

#### Tuần 4: 
- Lập trình GPIO sử dụng ngôn ngữ C

You can view my full report here: Report (PDF)

#### Tuần 5: 
- Lập trình LED 7 SEG

You can view my full report here: Report (PDF)

#### Tuần 6: 
- Timer/Counter (C)

- Lập trình UART
You can view my full report here: Report (PDF)

#### KIỂM TRA ASM: 

Đề bài: Viết duy nhất 1 chương trình hoàn chỉnh, với các yêu cầu sau:

Cho 4 led tắt dần từ phải sang trái, rồi lặp lại. Có trạng thái “Sáng hết”. Tốc độ T ms (mặc định T = 1000 ms).

    1. Nhấn nút A, T = T + 500 ms (tối đa 3 s).

    2. Nhấn nút B, T = T – 500 ms (tối thiểu 500 ms).

    3. Nhấn nút C, Led sáng hết rồi đứng im. Nhấn nút D, Led chạy lại như yêu cầu 1.

    4. Led 7 đoạn: hiển thị số led đang sáng (2–3–4), hiển thị tốc độ đếm hiện tại.

You can view my code: 
#### KIỂM TRA C LẦN 1: 
Đề bài: 
3 chế độ led:
    sáng dần trái phải x8
    tắt dần phải trái x8
    nháy x8
1 nút chuyển chế độ:
mode 0: lặp lại 3 cái liên tục
mode 1,2,3: cho từng chế độ riêng
2 led 7 đoạn hiển thị mode và số led sáng
You can view my code: 

#### KIỂM TRA C LẦN 2: 
Đề bài: // 1 Led blink 100ms 
// Button 1 : Bật sáng trong T s -> quay về ban đầu (T = 1)
// Button 2: Cấu hình T = T + 0,5 (T = 0,5 -> 2s)
// Hiển thị T lên led7seg (ms)
You can view my code: 
#### Overview: 
Code C & ASM: 



