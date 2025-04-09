#include <stdio.h>
#include <time.h>

unsigned long long fact(int n) {
    if (n == 1 || n == 0) return 1;
    return n * fact(n-1); 
}

int main() {
    int m_values[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int num_values = sizeof(m_values) / sizeof(m_values[0]);

    for (int i = 0; i < num_values; i++) {
        int m = m_values[i];
        double total = 0;

        for (int j = 0; j < 10; j++) {
            double start_time = (double) clock() / CLOCKS_PER_SEC;
            printf("%d", fact(m));
            double end_time = (double) clock() / CLOCKS_PER_SEC;
            total += end_time - start_time;
        }
        double res = (total / 10) * 1000;
        printf("n: %d, time: %lf ms\n", m, res);
    }

    return 0;
}