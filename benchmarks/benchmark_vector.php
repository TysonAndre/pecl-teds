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
(Note that LowMemoryVector/IntVector has specialized representations for integers and small integers)

Appending to array:           n=       1 iterations= 8000000 memory=     216 bytes
 => create+destroy time=0.741 read time = 0.369 total time = 1.110 result=0
Appending to Vector:          n=       1 iterations= 8000000 memory=     144 bytes
 => create+destroy time=1.074 read time = 0.374 total time = 1.448 result=0
Appending to IntVector:       n=       1 iterations= 8000000 memory=      88 bytes
 => create+destroy time=1.062 read time = 0.380 total time = 1.442 result=0
Appending to LowMemoryVector: n=       1 iterations= 8000000 memory=      88 bytes
 => create+destroy time=1.126 read time = 0.392 total time = 1.517 result=0
Appending to Deque:           n=       1 iterations= 8000000 memory=     144 bytes
 => create+destroy time=1.084 read time = 0.376 total time = 1.460 result=0
Appending to SplStack:        n=       1 iterations= 8000000 memory=     184 bytes
 => create+destroy time=1.835 read time = 0.817 total time = 2.653 result=0
Appending to SplFixedArray:   n=       1 iterations= 8000000 memory=      80 bytes
 => create+destroy time=1.932 read time = 0.440 total time = 2.372 result=0


Appending to array:           n=       4 iterations= 2000000 memory=     216 bytes
 => create+destroy time=0.224 read time = 0.123 total time = 0.347 result=12000000
Appending to Vector:          n=       4 iterations= 2000000 memory=     144 bytes
 => create+destroy time=0.349 read time = 0.180 total time = 0.529 result=12000000
Appending to IntVector:       n=       4 iterations= 2000000 memory=      88 bytes
 => create+destroy time=0.343 read time = 0.186 total time = 0.529 result=12000000
Appending to LowMemoryVector: n=       4 iterations= 2000000 memory=      88 bytes
 => create+destroy time=0.347 read time = 0.192 total time = 0.540 result=12000000
Appending to Deque:           n=       4 iterations= 2000000 memory=     144 bytes
 => create+destroy time=0.359 read time = 0.182 total time = 0.540 result=12000000
Appending to SplStack:        n=       4 iterations= 2000000 memory=     280 bytes
 => create+destroy time=0.782 read time = 0.324 total time = 1.106 result=12000000
Appending to SplFixedArray:   n=       4 iterations= 2000000 memory=     128 bytes
 => create+destroy time=1.264 read time = 0.248 total time = 1.511 result=12000000


Appending to array:           n=       8 iterations= 1000000 memory=     216 bytes
 => create+destroy time=0.151 read time = 0.089 total time = 0.240 result=28000000
Appending to Vector:          n=       8 iterations= 1000000 memory=     208 bytes
 => create+destroy time=0.248 read time = 0.163 total time = 0.412 result=28000000
Appending to IntVector:       n=       8 iterations= 1000000 memory=      88 bytes
 => create+destroy time=0.228 read time = 0.166 total time = 0.394 result=28000000
Appending to LowMemoryVector: n=       8 iterations= 1000000 memory=      88 bytes
 => create+destroy time=0.226 read time = 0.172 total time = 0.397 result=28000000
Appending to Deque:           n=       8 iterations= 1000000 memory=     208 bytes
 => create+destroy time=0.259 read time = 0.168 total time = 0.427 result=28000000
Appending to SplStack:        n=       8 iterations= 1000000 memory=     408 bytes
 => create+destroy time=0.565 read time = 0.256 total time = 0.822 result=28000000
Appending to SplFixedArray:   n=       8 iterations= 1000000 memory=     192 bytes
 => create+destroy time=1.151 read time = 0.226 total time = 1.377 result=28000000


Appending to array:           n=    1024 iterations=    8000 memory=   20536 bytes
 => create+destroy time=0.100 read time = 0.070 total time = 0.170 result=4190208000
Appending to Vector:          n=    1024 iterations=    8000 memory=   16464 bytes
 => create+destroy time=0.128 read time = 0.139 total time = 0.266 result=4190208000
Appending to IntVector:       n=    1024 iterations=    8000 memory=    2128 bytes
 => create+destroy time=0.117 read time = 0.150 total time = 0.267 result=4190208000
Appending to LowMemoryVector: n=    1024 iterations=    8000 memory=    2128 bytes
 => create+destroy time=0.099 read time = 0.140 total time = 0.239 result=4190208000
Appending to Deque:           n=    1024 iterations=    8000 memory=   16464 bytes
 => create+destroy time=0.115 read time = 0.127 total time = 0.242 result=4190208000
Appending to SplStack:        n=    1024 iterations=    8000 memory=   32920 bytes
 => create+destroy time=0.361 read time = 0.156 total time = 0.517 result=4190208000
Appending to SplFixedArray:   n=    1024 iterations=    8000 memory=   16448 bytes
 => create+destroy time=0.917 read time = 0.169 total time = 1.085 result=4190208000


Appending to array:           n= 1048576 iterations=      20 memory=16781392 bytes
 => create+destroy time=0.439 read time = 0.162 total time = 0.601 result=10995105792000
Appending to Vector:          n= 1048576 iterations=      20 memory=16777320 bytes
 => create+destroy time=0.539 read time = 0.324 total time = 0.864 result=10995105792000
Appending to IntVector:       n= 1048576 iterations=      20 memory= 4194408 bytes
 => create+destroy time=0.289 read time = 0.322 total time = 0.611 result=10995105792000
Appending to LowMemoryVector: n= 1048576 iterations=      20 memory= 4194408 bytes
 => create+destroy time=0.274 read time = 0.344 total time = 0.618 result=10995105792000
Appending to Deque:           n= 1048576 iterations=      20 memory=16777320 bytes
 => create+destroy time=0.544 read time = 0.321 total time = 0.866 result=10995105792000
Appending to SplStack:        n= 1048576 iterations=      20 memory=33554584 bytes
 => create+destroy time=0.973 read time = 0.427 total time = 1.401 result=10995105792000
Appending to SplFixedArray:   n= 1048576 iterations=      20 memory=16777304 bytes
 => create+destroy time=2.669 read time = 0.428 total time = 3.097 result=10995105792000


*/
