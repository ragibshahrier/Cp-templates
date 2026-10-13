// ---- divisor count d(x) and divisor sum sigma(x) for all x <= N ----
// O(N log N), harmonic sum. Dead simple, enormously useful.
const long long N = 1e6;
int    dcnt[N+1];
long long dsum[N+1];

void divisor_sieve() {
    for (int d = 1; d <= N; d++)
        for (int m = d; m <= N; m += d) {
            dcnt[m] += 1;
            dsum[m] += d;
        }
}