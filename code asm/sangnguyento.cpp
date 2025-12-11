#include <stdio.h>
#include <windows.h>

inline int fastscan() {
    int c = getchar();
    int x = 0, neg = 0;
    while (c < '0' || c > '9') {
        if (c == '-') neg = 1;
        c = getchar();
    }
    for (; c >= '0' && c <= '9'; c = getchar())
        x = x * 10 + c - '0';
    return neg ? -x : x;
}

int main() {
    LARGE_INTEGER freq, start, end;
    QueryPerformanceFrequency(&freq);

    int n = fastscan();
    int *isPrime = (int*)malloc((n + 1) * sizeof(int));

    // start timer
    QueryPerformanceCounter(&start);

    for (int i = 0; i <= n; i++) isPrime[i] = 1;

    isPrime[0] = isPrime[1] = 0;

    for (int i = 2; i * i <= n; i++) {
        if (isPrime[i]) {
            for (int j = i * i; j <= n; j += i)
                isPrime[j] = 0;
        }
    }

    long long sum = 0;
    for (int i = 2; i <= n; i++)
        if (isPrime[i]) sum += i;

    // end timer
    QueryPerformanceCounter(&end);

    double elapsed_us = (double)(end.QuadPart - start.QuadPart) * 1e6 / freq.QuadPart;
    double elapsed_ms = elapsed_us / 1000.0;

    printf("Sum: %lld\n", sum);
    printf("Time: %.6f ms\n", elapsed_ms);

    free(isPrime);
    return 0;
}
