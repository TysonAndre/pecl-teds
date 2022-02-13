<?php

use Teds\Deque;
use Teds\IntVector;
use Teds\LowMemoryVector;
use Teds\Vector;

// @phan-file-suppress PhanPossiblyUndeclaredVariable

function bench_array(int $n, int $iterations) {
    $totalReadTime = 0.0;
    $startTime = hrtime(true);
    $total = 0;
    for ($j = 0; $j < $iterations; $j++) {
        $startMemory = memory_get_usage();
        $values = [];
        for ($i = 0; $i < $n; $i++) {
            $values[] = $i;
        }
        $startReadTime = hrtime(true);
        for ($i = 0; $i < $n; $i++) {
            $total += $values[$i];
        }
        $endReadTime = hrtime(true);
        $totalReadTime += $endReadTime - $startReadTime;

        $endMemory = memory_get_usage();
        unset($values);
    }
    $endTime = hrtime(true);

    $totalTime = ($endTime - $startTime) / 1000000000;
    $totalReadTimeSeconds = $totalReadTime / 1000000000;
    printf("Appending to array:           n=%8d iterations=%8d memory=%8d bytes\n => create+destroy time=%.3f read time = %.3f total time = %.3f result=%d\n",
        $n, $iterations, $endMemory - $startMemory, $totalTime - $totalReadTimeSeconds, $totalReadTimeSeconds, $totalTime, $total);
}
function bench_vector(int $n, int $iterations) {
    $startTime = hrtime(true);
    $totalReadTime = 0.0;
    $total = 0;
    for ($j = 0; $j < $iterations; $j++) {
        $startMemory = memory_get_usage();
        $values = new Vector();
        for ($i = 0; $i < $n; $i++) {
            $values[] = $i;
        }

        $startReadTime = hrtime(true);
        for ($i = 0; $i < $n; $i++) {
            $total += $values[$i];
        }
        $endReadTime = hrtime(true);
        $totalReadTime += $endReadTime - $startReadTime;

        $endMemory = memory_get_usage();
        unset($values);
    }
    $endTime = hrtime(true);
    $totalTime = ($endTime - $startTime) / 1000000000;
    $totalReadTimeSeconds = $totalReadTime / 1000000000;
    printf("Appending to Vector:          n=%8d iterations=%8d memory=%8d bytes\n => create+destroy time=%.3f read time = %.3f total time = %.3f result=%d\n",
        $n, $iterations, $endMemory - $startMemory, $totalTime - $totalReadTimeSeconds, $totalReadTimeSeconds, $totalTime, $total);
}
function bench_int_vector(int $n, int $iterations) {
    $startTime = hrtime(true);
    $totalReadTime = 0.0;
    $total = 0;
    for ($j = 0; $j < $iterations; $j++) {
        $startMemory = memory_get_usage();
        $values = new IntVector();
        for ($i = 0; $i < $n; $i++) {
            $values[] = $i;
        }

        $startReadTime = hrtime(true);
        for ($i = 0; $i < $n; $i++) {
            $total += $values[$i];
        }
        $endReadTime = hrtime(true);
        $totalReadTime += $endReadTime - $startReadTime;

        $endMemory = memory_get_usage();
        unset($values);
    }
    $endTime = hrtime(true);
    $totalTime = ($endTime - $startTime) / 1000000000;
    $totalReadTimeSeconds = $totalReadTime / 1000000000;
    printf("Appending to IntVector:       n=%8d iterations=%8d memory=%8d bytes\n => create+destroy time=%.3f read time = %.3f total time = %.3f result=%d\n",
        $n, $iterations, $endMemory - $startMemory, $totalTime - $totalReadTimeSeconds, $totalReadTimeSeconds, $totalTime, $total);
}
function bench_low_memory_vector(int $n, int $iterations) {
    $startTime = hrtime(true);
    $totalReadTime = 0.0;
    $total = 0;
    for ($j = 0; $j < $iterations; $j++) {
        $startMemory = memory_get_usage();
        $values = new LowMemoryVector();
        for ($i = 0; $i < $n; $i++) {
            $values[] = $i;
        }

        $startReadTime = hrtime(true);
        for ($i = 0; $i < $n; $i++) {
            $total += $values[$i];
        }
        $endReadTime = hrtime(true);
        $totalReadTime += $endReadTime - $startReadTime;

        $endMemory = memory_get_usage();
        unset($values);
    }
    $endTime = hrtime(true);
    $totalTime = ($endTime - $startTime) / 1000000000;
    $totalReadTimeSeconds = $totalReadTime / 1000000000;
    printf("Appending to LowMemoryVector: n=%8d iterations=%8d memory=%8d bytes\n => create+destroy time=%.3f read time = %.3f total time = %.3f result=%d\n",
        $n, $iterations, $endMemory - $startMemory, $totalTime - $totalReadTimeSeconds, $totalReadTimeSeconds, $totalTime, $total);
}
function bench_queue(int $n, int $iterations) {
    $startTime = hrtime(true);
    $totalReadTime = 0.0;
    $total = 0;
    for ($j = 0; $j < $iterations; $j++) {
        $startMemory = memory_get_usage();
        $values = new Deque();
        for ($i = 0; $i < $n; $i++) {
            $values[] = $i;
        }

        $startReadTime = hrtime(true);
        for ($i = 0; $i < $n; $i++) {
            $total += $values[$i];
        }
        $endReadTime = hrtime(true);
        $totalReadTime += $endReadTime - $startReadTime;

        $endMemory = memory_get_usage();
        unset($values);
    }
    $endTime = hrtime(true);
    $totalTime = ($endTime - $startTime) / 1000000000;
    $totalReadTimeSeconds = $totalReadTime / 1000000000;
    printf("Appending to Deque:           n=%8d iterations=%8d memory=%8d bytes\n => create+destroy time=%.3f read time = %.3f total time = %.3f result=%d\n",
        $n, $iterations, $endMemory - $startMemory, $totalTime - $totalReadTimeSeconds, $totalReadTimeSeconds, $totalTime, $total);
}
// SplStack is a subclass of SplDoublyLinkedList, so it is a linked list that takes more memory than needed.
// Access to values in the middle of the SplStack is also less efficient.
function bench_spl_stack(int $n, int $iterations) {
    $startTime = hrtime(true);
    $totalReadTime = 0.0;
    $total = 0;
    for ($j = 0; $j < $iterations; $j++) {
        $startMemory = memory_get_usage();
        $values = new SplStack();
        for ($i = 0; $i < $n; $i++) {
            // XXX: For SplStack, $values[] = $value is unexpectedly twice as slow as push($value)
            // due to the lack of custom handlers and falling back to the default ArrayAccess->offsetSet call.
            // This could probably be improved.
            $values->push($i);
        }
        $startReadTime = hrtime(true);
        // Random access is linear time in a linked list, so use foreach instead
        foreach ($values as $value) {
            $total += $value;
        }
        $endReadTime = hrtime(true);
        $totalReadTime += $endReadTime - $startReadTime;
        $endMemory = memory_get_usage();
        unset($values);
    }
    $endTime = hrtime(true);
    $totalTime = ($endTime - $startTime) / 1000000000;
    $totalReadTimeSeconds = $totalReadTime / 1000000000;
    printf("Appending to SplStack:        n=%8d iterations=%8d memory=%8d bytes\n => create+destroy time=%.3f read time = %.3f total time = %.3f result=%d\n",
        $n, $iterations, $endMemory - $startMemory, $totalTime - $totalReadTimeSeconds, $totalReadTimeSeconds, $totalTime, $total);
}
function bench_spl_fixed_array(int $n, int $iterations) {
    $startTime = hrtime(true);
    $totalReadTime = 0.0;
    $total = 0;
    for ($j = 0; $j < $iterations; $j++) {
        $startMemory = memory_get_usage();
        $values = new SplFixedArray();
        for ($i = 0; $i < $n; $i++) {
            // Imitate how push() would be implemented in a situation
            // where the number of elements wasn't actually known ahead of time.
            // erealloc() tends to extend the existing array when possible.
            $size = $values->getSize();
            $values->setSize($size + 1);
            $values->offsetSet($size, $i);
        }
        $startReadTime = hrtime(true);
        for ($i = 0; $i < $n; $i++) {
            $total += $values[$i];
        }
        $endReadTime = hrtime(true);
        $totalReadTime += $endReadTime - $startReadTime;
        $endMemory = memory_get_usage();
        unset($values);
    }
    $endTime = hrtime(true);
    $totalTime = ($endTime - $startTime) / 1000000000;
    $totalReadTimeSeconds = $totalReadTime / 1000000000;
    printf("Appending to SplFixedArray:   n=%8d iterations=%8d memory=%8d bytes\n => create+destroy time=%.3f read time = %.3f total time = %.3f result=%d\n\n",
        $n, $iterations, $endMemory - $startMemory, $totalTime - $totalReadTimeSeconds, $totalReadTimeSeconds, $totalTime, $total);
}
$n = 2**20;
$iterations = 10;
$sizes = [
    [1, 8000000],
    [4, 2000000],
    [8, 1000000],
    [1024, 8000],
    [2**20, 20],
];
printf(
    "Results for php %s debug=%s with opcache enabled=%s\n",
    PHP_VERSION,
    PHP_DEBUG ? 'true' : 'false',
    json_encode(function_exists('opcache_get_status') && (opcache_get_status(false)['opcache_enabled'] ?? false))
);
echo "(Note that LowMemoryVector/IntVector has specialized representations for integers and small integers)\n\n";

foreach ($sizes as [$n, $iterations]) {
    bench_array($n, $iterations);
    bench_vector($n, $iterations);
    bench_int_vector($n, $iterations);
    bench_low_memory_vector($n, $iterations);
    bench_queue($n, $iterations);
    bench_spl_stack($n, $iterations);
    bench_spl_fixed_array($n, $iterations);
    echo "\n";
}
/*
Results for php 8.2.0-dev debug=false with opcache enabled=true
(Note that LowMemoryVector has specialized representations for integers and small integers)

Appending to array:           n=       1 iterations= 8000000 memory=     216 bytes
 => create+destroy time=0.581 read time = 0.290 total time = 0.871 result=0
Appending to Vector:          n=       1 iterations= 8000000 memory=     128 bytes
 => create+destroy time=0.977 read time = 0.334 total time = 1.310 result=0
Appending to LowMemoryVector: n=       1 iterations= 8000000 memory=      88 bytes
 => create+destroy time=1.000 read time = 0.354 total time = 1.354 result=0
Appending to Deque:           n=       1 iterations= 8000000 memory=     144 bytes
 => create+destroy time=0.962 read time = 0.337 total time = 1.299 result=0
Appending to SplStack:        n=       1 iterations= 8000000 memory=     184 bytes
 => create+destroy time=1.607 read time = 0.668 total time = 2.275 result=0
Appending to SplFixedArray:   n=       1 iterations= 8000000 memory=      80 bytes
 => create+destroy time=1.719 read time = 0.397 total time = 2.115 result=0


Appending to array:           n=       4 iterations= 2000000 memory=     216 bytes
 => create+destroy time=0.202 read time = 0.112 total time = 0.314 result=12000000
Appending to Vector:          n=       4 iterations= 2000000 memory=     128 bytes
 => create+destroy time=0.318 read time = 0.161 total time = 0.479 result=12000000
Appending to LowMemoryVector: n=       4 iterations= 2000000 memory=      88 bytes
 => create+destroy time=0.318 read time = 0.171 total time = 0.489 result=12000000
Appending to Deque:           n=       4 iterations= 2000000 memory=     144 bytes
 => create+destroy time=0.313 read time = 0.161 total time = 0.474 result=12000000
Appending to SplStack:        n=       4 iterations= 2000000 memory=     280 bytes
 => create+destroy time=0.680 read time = 0.267 total time = 0.948 result=12000000
Appending to SplFixedArray:   n=       4 iterations= 2000000 memory=     128 bytes
 => create+destroy time=1.113 read time = 0.216 total time = 1.329 result=12000000


Appending to array:           n=       8 iterations= 1000000 memory=     216 bytes
 => create+destroy time=0.137 read time = 0.081 total time = 0.218 result=28000000
Appending to Vector:          n=       8 iterations= 1000000 memory=     192 bytes
 => create+destroy time=0.223 read time = 0.147 total time = 0.370 result=28000000
Appending to LowMemoryVector: n=       8 iterations= 1000000 memory=      88 bytes
 => create+destroy time=0.210 read time = 0.150 total time = 0.360 result=28000000
Appending to Deque:           n=       8 iterations= 1000000 memory=     208 bytes
 => create+destroy time=0.221 read time = 0.148 total time = 0.370 result=28000000
Appending to SplStack:        n=       8 iterations= 1000000 memory=     408 bytes
 => create+destroy time=0.494 read time = 0.217 total time = 0.711 result=28000000
Appending to SplFixedArray:   n=       8 iterations= 1000000 memory=     192 bytes
 => create+destroy time=0.989 read time = 0.194 total time = 1.183 result=28000000


Appending to array:           n=    1024 iterations=    8000 memory=   20536 bytes
 => create+destroy time=0.079 read time = 0.055 total time = 0.134 result=4190208000
Appending to Vector:          n=    1024 iterations=    8000 memory=   16448 bytes
 => create+destroy time=0.103 read time = 0.111 total time = 0.213 result=4190208000
Appending to LowMemoryVector: n=    1024 iterations=    8000 memory=    4176 bytes
 => create+destroy time=0.096 read time = 0.118 total time = 0.214 result=4190208000
Appending to Deque:           n=    1024 iterations=    8000 memory=   16464 bytes
 => create+destroy time=0.101 read time = 0.111 total time = 0.212 result=4190208000
Appending to SplStack:        n=    1024 iterations=    8000 memory=   32920 bytes
 => create+destroy time=0.318 read time = 0.137 total time = 0.455 result=4190208000
Appending to SplFixedArray:   n=    1024 iterations=    8000 memory=   16448 bytes
 => create+destroy time=0.825 read time = 0.153 total time = 0.977 result=4190208000


Appending to array:           n= 1048576 iterations=      20 memory=16781392 bytes
 => create+destroy time=0.405 read time = 0.146 total time = 0.551 result=10995105792000
Appending to Vector:          n= 1048576 iterations=      20 memory=16777304 bytes
 => create+destroy time=0.481 read time = 0.280 total time = 0.761 result=10995105792000
Appending to LowMemoryVector: n= 1048576 iterations=      20 memory= 4194408 bytes
 => create+destroy time=0.266 read time = 0.284 total time = 0.551 result=10995105792000
Appending to Deque:           n= 1048576 iterations=      20 memory=16777320 bytes
 => create+destroy time=0.479 read time = 0.279 total time = 0.758 result=10995105792000
Appending to SplStack:        n= 1048576 iterations=      20 memory=33554584 bytes
 => create+destroy time=0.865 read time = 0.377 total time = 1.242 result=10995105792000
Appending to SplFixedArray:   n= 1048576 iterations=      20 memory=16777304 bytes
 => create+destroy time=2.378 read time = 0.377 total time = 2.754 result=10995105792000
Results for php 8.2.0-dev debug=false with opcache enabled=true
(Note that LowMemoryVector/IntVector has specialized representations for integers and small integers)

Appending to array:           n=       1 iterations= 8000000 memory=     216 bytes
 => create+destroy time=0.581 read time = 0.289 total time = 0.870 result=0
Appending to Vector:          n=       1 iterations= 8000000 memory=     128 bytes
 => create+destroy time=0.971 read time = 0.346 total time = 1.316 result=0
Appending to IntVector:       n=       1 iterations= 8000000 memory=      88 bytes
 => create+destroy time=1.013 read time = 0.360 total time = 1.373 result=0
Appending to LowMemoryVector: n=       1 iterations= 8000000 memory=      88 bytes
 => create+destroy time=0.990 read time = 0.353 total time = 1.343 result=0
Appending to Deque:           n=       1 iterations= 8000000 memory=     144 bytes
 => create+destroy time=0.973 read time = 0.351 total time = 1.324 result=0
Appending to SplStack:        n=       1 iterations= 8000000 memory=     184 bytes
 => create+destroy time=1.661 read time = 0.714 total time = 2.374 result=0
Appending to SplFixedArray:   n=       1 iterations= 8000000 memory=      80 bytes
 => create+destroy time=1.770 read time = 0.417 total time = 2.187 result=0


Appending to array:           n=       4 iterations= 2000000 memory=     216 bytes
 => create+destroy time=0.210 read time = 0.113 total time = 0.323 result=12000000
Appending to Vector:          n=       4 iterations= 2000000 memory=     128 bytes
 => create+destroy time=0.321 read time = 0.166 total time = 0.487 result=12000000
Appending to IntVector:       n=       4 iterations= 2000000 memory=      88 bytes
 => create+destroy time=0.313 read time = 0.173 total time = 0.487 result=12000000
Appending to LowMemoryVector: n=       4 iterations= 2000000 memory=      88 bytes
 => create+destroy time=0.317 read time = 0.173 total time = 0.489 result=12000000
Appending to Deque:           n=       4 iterations= 2000000 memory=     144 bytes
 => create+destroy time=0.319 read time = 0.168 total time = 0.486 result=12000000
Appending to SplStack:        n=       4 iterations= 2000000 memory=     280 bytes
 => create+destroy time=0.703 read time = 0.282 total time = 0.985 result=12000000
Appending to SplFixedArray:   n=       4 iterations= 2000000 memory=     128 bytes
 => create+destroy time=1.137 read time = 0.225 total time = 1.361 result=12000000


Appending to array:           n=       8 iterations= 1000000 memory=     216 bytes
 => create+destroy time=0.142 read time = 0.082 total time = 0.223 result=28000000
Appending to Vector:          n=       8 iterations= 1000000 memory=     192 bytes
 => create+destroy time=0.230 read time = 0.150 total time = 0.380 result=28000000
Appending to IntVector:       n=       8 iterations= 1000000 memory=      88 bytes
 => create+destroy time=0.205 read time = 0.152 total time = 0.357 result=28000000
Appending to LowMemoryVector: n=       8 iterations= 1000000 memory=      88 bytes
 => create+destroy time=0.209 read time = 0.151 total time = 0.360 result=28000000
Appending to Deque:           n=       8 iterations= 1000000 memory=     208 bytes
 => create+destroy time=0.224 read time = 0.154 total time = 0.378 result=28000000
Appending to SplStack:        n=       8 iterations= 1000000 memory=     408 bytes
 => create+destroy time=0.509 read time = 0.227 total time = 0.736 result=28000000
Appending to SplFixedArray:   n=       8 iterations= 1000000 memory=     192 bytes
 => create+destroy time=1.017 read time = 0.200 total time = 1.217 result=28000000


Appending to array:           n=    1024 iterations=    8000 memory=   20536 bytes
 => create+destroy time=0.079 read time = 0.056 total time = 0.134 result=4190208000
Appending to Vector:          n=    1024 iterations=    8000 memory=   16448 bytes
 => create+destroy time=0.108 read time = 0.116 total time = 0.223 result=4190208000
Appending to IntVector:       n=    1024 iterations=    8000 memory=    2128 bytes
 => create+destroy time=0.091 read time = 0.122 total time = 0.213 result=4190208000
Appending to LowMemoryVector: n=    1024 iterations=    8000 memory=    2128 bytes
 => create+destroy time=0.093 read time = 0.116 total time = 0.209 result=4190208000
Appending to Deque:           n=    1024 iterations=    8000 memory=   16464 bytes
 => create+destroy time=0.102 read time = 0.114 total time = 0.216 result=4190208000
Appending to SplStack:        n=    1024 iterations=    8000 memory=   32920 bytes
 => create+destroy time=0.327 read time = 0.142 total time = 0.469 result=4190208000
Appending to SplFixedArray:   n=    1024 iterations=    8000 memory=   16448 bytes
 => create+destroy time=0.830 read time = 0.153 total time = 0.983 result=4190208000


Appending to array:           n= 1048576 iterations=      20 memory=16781392 bytes
 => create+destroy time=0.399 read time = 0.143 total time = 0.542 result=10995105792000
Appending to Vector:          n= 1048576 iterations=      20 memory=16777304 bytes
 => create+destroy time=0.512 read time = 0.295 total time = 0.808 result=10995105792000
Appending to IntVector:       n= 1048576 iterations=      20 memory= 4194408 bytes
 => create+destroy time=0.266 read time = 0.300 total time = 0.566 result=10995105792000
Appending to LowMemoryVector: n= 1048576 iterations=      20 memory= 4194408 bytes
 => create+destroy time=0.258 read time = 0.288 total time = 0.546 result=10995105792000
Appending to Deque:           n= 1048576 iterations=      20 memory=16777320 bytes
 => create+destroy time=0.476 read time = 0.285 total time = 0.761 result=10995105792000
Appending to SplStack:        n= 1048576 iterations=      20 memory=33554584 bytes
 => create+destroy time=0.873 read time = 0.379 total time = 1.252 result=10995105792000
Appending to SplFixedArray:   n= 1048576 iterations=      20 memory=16777304 bytes
 => create+destroy time=2.411 read time = 0.384 total time = 2.795 result=10995105792000



*/
