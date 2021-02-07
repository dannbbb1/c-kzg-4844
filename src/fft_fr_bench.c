/*
 * Copyright 2021 Benjamin Edgington
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h> // malloc(), free()
#include <stdio.h>  // printf()
#include <assert.h> // assert()
#include "bench_util.h"
#include "fft_fr.h"

// Run the benchmark for `max_seconds` and return the time per iteration in nanoseconds.
long run_bench(int scale, int max_seconds) {
    timespec t0, t1;
    unsigned long total_time = 0, nits = 0;
    FFTSettings fs;

    assert(C_KZG_OK == new_fft_settings(&fs, scale));
    // Allocate on the heap to avoid stack overflow for large sizes
    blst_fr *data, *out;
    data = malloc(fs.max_width * sizeof(blst_fr));
    out = malloc(fs.max_width * sizeof(blst_fr));

    // Fill with randomness
    for (uint64_t i = 0; i < fs.max_width; i++) {
        data[i] = rand_fr();
    }

    while (total_time < max_seconds * NANO) {
        clock_gettime(CLOCK_REALTIME, &t0);
        assert(C_KZG_OK == fft_fr(out, data, &fs, false, fs.max_width));
        clock_gettime(CLOCK_REALTIME, &t1);
        nits++;
        total_time += tdiff(t0, t1);
    }

    free(out);
    free(data);

    return total_time / nits;
}

#define NSEC 1

int main(void) {
    printf("*** Benchmarking FFT_fr, %d second%s per test.\n", NSEC, NSEC == 1 ? "" : "s");
    for (int scale = 4; scale < 16; scale++) {
        printf("fft_fr/scale_%d %lu ns/op\n", scale, run_bench(scale, 1));
    }
}
