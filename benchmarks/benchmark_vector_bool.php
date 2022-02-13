<?php
// TODO benchmark serialize+unserialize

// @phan-file-suppress PhanPossiblyUndeclaredVariable

use Teds\BitSet;
use Teds\Deque;
use Teds\LowMemoryVector;
use Teds\Vector;

function bench_array(int $n, int $iterations) {
    $totalReadTime = 0.0;
    $startTime = hrtime(true);
    $total = 0;
    for ($j = 0; $j < $iterations; $j++) {
        $startMemory = memory_get_usage();
        $values = [];
        for ($i = 0; $i < $n; $i++) {
            $values[] = $i % 3 > 0;
        }
        $startReadTime = hrtime(true);
        for ($i = 0; $i < $n; $i++) {
            $total += $values[$i] ? 1 : 0;
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
            $values[] = $i % 3 > 0;
        }

        $startReadTime = hrtime(true);
        for ($i = 0; $i < $n; $i++) {
            $total += $values[$i] ? 1 : 0;
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
function bench_bitset(int $n, int $iterations) {
    $startTime = hrtime(true);
    $totalReadTime = 0.0;
    $total = 0;
    for ($j = 0; $j < $iterations; $j++) {
        $startMemory = memory_get_usage();
        $values = new BitSet();
        for ($i = 0; $i < $n; $i++) {
            $values[] = $i % 3 > 0;
        }

        $startReadTime = hrtime(true);
        for ($i = 0; $i < $n; $i++) {
            $total += $values[$i] ? 1 : 0;
        }
        $endReadTime = hrtime(true);
        $totalReadTime += $endReadTime - $startReadTime;

        $endMemory = memory_get_usage();
        unset($values);
    }
    $endTime = hrtime(true);
    $totalTime = ($endTime - $startTime) / 1000000000;
    $totalReadTimeSeconds = $totalReadTime / 1000000000;
    printf("Appending to BitSet:          n=%8d iterations=%8d memory=%8d bytes\n => create+destroy time=%.3f read time = %.3f total time = %.3f result=%d\n",
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
            $values[] = $i % 3 > 0;
        }

        $startReadTime = hrtime(true);
        for ($i = 0; $i < $n; $i++) {
            $total += $values[$i] ? 1 : 0;
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
            $values[] = $i % 3 > 0;
        }

        $startReadTime = hrtime(true);
        for ($i = 0; $i < $n; $i++) {
            $total += $values[$i] ? 1 : 0;
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
            $values->push($i % 3 > 0);
        }
        $startReadTime = hrtime(true);
        // Random access is linear time in a linked list, so use foreach instead
        foreach ($values as $value) {
            $total += $value ? 1 : 0;
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
            $values->offsetSet($size, $i % 3 > 0);
        }
        $startReadTime = hrtime(true);
        for ($i = 0; $i < $n; $i++) {
            $total += $values[$i] ? 1 : 0;
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
echo "(Note that LowMemoryVector has specialized representations for collections of bool and null)\n\n";

foreach ($sizes as [$n, $iterations]) {
    bench_array($n, $iterations);
    bench_vector($n, $iterations);
    bench_bitset($n, $iterations);
    bench_low_memory_vector($n, $iterations);
    bench_queue($n, $iterations);
    bench_spl_stack($n, $iterations);
    bench_spl_fixed_array($n, $iterations);
    echo "\n";
}
/*
Results for php 8.2.0-dev debug=false with opcache enabled=true
(Note that LowMemoryVector has specialized representations for collections of bool and null)

Appending to array:           n=       1 iterations= 8000000 memory=     216 bytes
 => create+destroy time=0.670 read time = 0.299 total time = 0.970 result=0
Appending to Vector:          n=       1 iterations= 8000000 memory=     120 bytes
 => create+destroy time=1.035 read time = 0.334 total time = 1.369 result=0
Appending to BitSet:          n=       1 iterations= 8000000 memory=      72 bytes
 => create+destroy time=1.000 read time = 0.340 total time = 1.340 result=0
Appending to LowMemoryVector: n=       1 iterations= 8000000 memory=      72 bytes
 => create+destroy time=1.032 read time = 0.351 total time = 1.383 result=0
Appending to Deque:           n=       1 iterations= 8000000 memory=     144 bytes
 => create+destroy time=1.030 read time = 0.340 total time = 1.370 result=0
Appending to SplStack:        n=       1 iterations= 8000000 memory=     184 bytes
 => create+destroy time=1.628 read time = 0.659 total time = 2.287 result=0
Appending to SplFixedArray:   n=       1 iterations= 8000000 memory=      80 bytes
 => create+destroy time=1.736 read time = 0.365 total time = 2.101 result=0


Appending to array:           n=       4 iterations= 2000000 memory=     216 bytes
 => create+destroy time=0.282 read time = 0.124 total time = 0.407 result=4000000
Appending to Vector:          n=       4 iterations= 2000000 memory=     120 bytes
 => create+destroy time=0.415 read time = 0.165 total time = 0.580 result=4000000
Appending to BitSet:          n=       4 iterations= 2000000 memory=      72 bytes
 => create+destroy time=0.378 read time = 0.169 total time = 0.547 result=4000000
Appending to LowMemoryVector: n=       4 iterations= 2000000 memory=      72 bytes
 => create+destroy time=0.387 read time = 0.167 total time = 0.554 result=4000000
Appending to Deque:           n=       4 iterations= 2000000 memory=     144 bytes
 => create+destroy time=0.394 read time = 0.161 total time = 0.556 result=4000000
Appending to SplStack:        n=       4 iterations= 2000000 memory=     280 bytes
 => create+destroy time=0.736 read time = 0.264 total time = 1.000 result=4000000
Appending to SplFixedArray:   n=       4 iterations= 2000000 memory=     128 bytes
 => create+destroy time=1.180 read time = 0.191 total time = 1.372 result=4000000


Appending to array:           n=       8 iterations= 1000000 memory=     216 bytes
 => create+destroy time=0.224 read time = 0.096 total time = 0.320 result=5000000
Appending to Vector:          n=       8 iterations= 1000000 memory=     184 bytes
 => create+destroy time=0.304 read time = 0.132 total time = 0.436 result=5000000
Appending to BitSet:          n=       8 iterations= 1000000 memory=      72 bytes
 => create+destroy time=0.278 read time = 0.140 total time = 0.418 result=5000000
Appending to LowMemoryVector: n=       8 iterations= 1000000 memory=      72 bytes
 => create+destroy time=0.288 read time = 0.148 total time = 0.436 result=5000000
Appending to Deque:           n=       8 iterations= 1000000 memory=     208 bytes
 => create+destroy time=0.321 read time = 0.137 total time = 0.457 result=5000000
Appending to SplStack:        n=       8 iterations= 1000000 memory=     408 bytes
 => create+destroy time=0.560 read time = 0.219 total time = 0.779 result=5000000
Appending to SplFixedArray:   n=       8 iterations= 1000000 memory=     192 bytes
 => create+destroy time=1.098 read time = 0.167 total time = 1.265 result=5000000


Appending to array:           n=    1024 iterations=    8000 memory=   20536 bytes
 => create+destroy time=0.157 read time = 0.069 total time = 0.225 result=5456000
Appending to Vector:          n=    1024 iterations=    8000 memory=   16440 bytes
 => create+destroy time=0.185 read time = 0.105 total time = 0.290 result=5456000
Appending to BitSet:          n=    1024 iterations=    8000 memory=     224 bytes
 => create+destroy time=0.170 read time = 0.118 total time = 0.289 result=5456000
Appending to LowMemoryVector: n=    1024 iterations=    8000 memory=    1088 bytes
 => create+destroy time=0.172 read time = 0.114 total time = 0.286 result=5456000
Appending to Deque:           n=    1024 iterations=    8000 memory=   16464 bytes
 => create+destroy time=0.184 read time = 0.109 total time = 0.293 result=5456000
Appending to SplStack:        n=    1024 iterations=    8000 memory=   32920 bytes
 => create+destroy time=0.395 read time = 0.138 total time = 0.533 result=5456000
Appending to SplFixedArray:   n=    1024 iterations=    8000 memory=   16448 bytes
 => create+destroy time=0.899 read time = 0.127 total time = 1.026 result=5456000


Appending to array:           n= 1048576 iterations=      20 memory=16781392 bytes
 => create+destroy time=0.593 read time = 0.181 total time = 0.774 result=13981000
Appending to Vector:          n= 1048576 iterations=      20 memory=16777296 bytes
 => create+destroy time=0.686 read time = 0.268 total time = 0.954 result=13981000
Appending to BitSet:          n= 1048576 iterations=      20 memory=  147520 bytes
 => create+destroy time=0.412 read time = 0.292 total time = 0.703 result=13981000
Appending to LowMemoryVector: n= 1048576 iterations=      20 memory= 1048640 bytes
 => create+destroy time=0.409 read time = 0.281 total time = 0.690 result=13981000
Appending to Deque:           n= 1048576 iterations=      20 memory=16777320 bytes
 => create+destroy time=0.677 read time = 0.279 total time = 0.955 result=13981000
Appending to SplStack:        n= 1048576 iterations=      20 memory=33554584 bytes
 => create+destroy time=1.038 read time = 0.363 total time = 1.401 result=13981000
Appending to SplFixedArray:   n= 1048576 iterations=      20 memory=16777304 bytes
 => create+destroy time=2.594 read time = 0.321 total time = 2.915 result=13981000

*/
