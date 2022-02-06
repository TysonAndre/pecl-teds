<?php

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
 => create+destroy time=0.671 read time = 0.298 total time = 0.969 result=0
Appending to Vector:          n=       1 iterations= 8000000 memory=     128 bytes
 => create+destroy time=1.108 read time = 0.336 total time = 1.444 result=0
Appending to LowMemoryVector: n=       1 iterations= 8000000 memory=      88 bytes
 => create+destroy time=1.072 read time = 0.352 total time = 1.424 result=0
Appending to Deque:           n=       1 iterations= 8000000 memory=     144 bytes
 => create+destroy time=1.047 read time = 0.337 total time = 1.384 result=0
Appending to SplStack:        n=       1 iterations= 8000000 memory=     184 bytes
 => create+destroy time=1.629 read time = 0.660 total time = 2.289 result=0
Appending to SplFixedArray:   n=       1 iterations= 8000000 memory=      80 bytes
 => create+destroy time=1.800 read time = 0.372 total time = 2.173 result=0


Appending to array:           n=       4 iterations= 2000000 memory=     216 bytes
 => create+destroy time=0.283 read time = 0.125 total time = 0.407 result=4000000
Appending to Vector:          n=       4 iterations= 2000000 memory=     128 bytes
 => create+destroy time=0.424 read time = 0.159 total time = 0.582 result=4000000
Appending to LowMemoryVector: n=       4 iterations= 2000000 memory=      88 bytes
 => create+destroy time=0.389 read time = 0.168 total time = 0.557 result=4000000
Appending to Deque:           n=       4 iterations= 2000000 memory=     144 bytes
 => create+destroy time=0.398 read time = 0.161 total time = 0.559 result=4000000
Appending to SplStack:        n=       4 iterations= 2000000 memory=     280 bytes
 => create+destroy time=0.738 read time = 0.275 total time = 1.013 result=4000000
Appending to SplFixedArray:   n=       4 iterations= 2000000 memory=     128 bytes
 => create+destroy time=1.184 read time = 0.189 total time = 1.373 result=4000000


Appending to array:           n=       8 iterations= 1000000 memory=     216 bytes
 => create+destroy time=0.222 read time = 0.096 total time = 0.318 result=5000000
Appending to Vector:          n=       8 iterations= 1000000 memory=     192 bytes
 => create+destroy time=0.328 read time = 0.134 total time = 0.462 result=5000000
Appending to LowMemoryVector: n=       8 iterations= 1000000 memory=      88 bytes
 => create+destroy time=0.283 read time = 0.149 total time = 0.432 result=5000000
Appending to Deque:           n=       8 iterations= 1000000 memory=     208 bytes
 => create+destroy time=0.307 read time = 0.135 total time = 0.442 result=5000000
Appending to SplStack:        n=       8 iterations= 1000000 memory=     408 bytes
 => create+destroy time=0.560 read time = 0.227 total time = 0.787 result=5000000
Appending to SplFixedArray:   n=       8 iterations= 1000000 memory=     192 bytes
 => create+destroy time=1.077 read time = 0.168 total time = 1.245 result=5000000


Appending to array:           n=    1024 iterations=    8000 memory=   20536 bytes
 => create+destroy time=0.156 read time = 0.069 total time = 0.225 result=5456000
Appending to Vector:          n=    1024 iterations=    8000 memory=   16448 bytes
 => create+destroy time=0.213 read time = 0.105 total time = 0.318 result=5456000
Appending to LowMemoryVector: n=    1024 iterations=    8000 memory=    1104 bytes
 => create+destroy time=0.168 read time = 0.115 total time = 0.284 result=5456000
Appending to Deque:           n=    1024 iterations=    8000 memory=   16464 bytes
 => create+destroy time=0.185 read time = 0.108 total time = 0.294 result=5456000
Appending to SplStack:        n=    1024 iterations=    8000 memory=   32920 bytes
 => create+destroy time=0.393 read time = 0.138 total time = 0.530 result=5456000
Appending to SplFixedArray:   n=    1024 iterations=    8000 memory=   16448 bytes
 => create+destroy time=0.891 read time = 0.129 total time = 1.020 result=5456000


Appending to array:           n= 1048576 iterations=      20 memory=16781392 bytes
 => create+destroy time=0.597 read time = 0.184 total time = 0.781 result=13981000
Appending to Vector:          n= 1048576 iterations=      20 memory=16777304 bytes
 => create+destroy time=0.760 read time = 0.273 total time = 1.033 result=13981000
Appending to LowMemoryVector: n= 1048576 iterations=      20 memory= 1048656 bytes
 => create+destroy time=0.400 read time = 0.281 total time = 0.682 result=13981000
Appending to Deque:           n= 1048576 iterations=      20 memory=16777320 bytes
 => create+destroy time=0.686 read time = 0.277 total time = 0.963 result=13981000
Appending to SplStack:        n= 1048576 iterations=      20 memory=33554584 bytes
 => create+destroy time=1.058 read time = 0.373 total time = 1.432 result=13981000
Appending to SplFixedArray:   n= 1048576 iterations=      20 memory=16777304 bytes
 => create+destroy time=2.591 read time = 0.334 total time = 2.926 result=13981000

*/
