<?php
// TODO benchmark serialize+unserialize

// @phan-file-suppress PhanPossiblyUndeclaredVariable

use Teds\BitVector;
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
function bench_bitvector(int $n, int $iterations) {
    $startTime = hrtime(true);
    $totalReadTime = 0.0;
    $total = 0;
    for ($j = 0; $j < $iterations; $j++) {
        $startMemory = memory_get_usage();
        $values = new BitVector();
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
    printf("Appending to BitVector:       n=%8d iterations=%8d memory=%8d bytes\n => create+destroy time=%.3f read time = %.3f total time = %.3f result=%d\n",
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
    bench_bitvector($n, $iterations);
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
 => create+destroy time=0.777 read time = 0.342 total time = 1.120 result=0
Appending to Vector:          n=       1 iterations= 8000000 memory=     144 bytes
 => create+destroy time=1.162 read time = 0.373 total time = 1.535 result=0
Appending to BitVector:       n=       1 iterations= 8000000 memory=      88 bytes
 => create+destroy time=1.260 read time = 0.397 total time = 1.658 result=0
Appending to LowMemoryVector: n=       1 iterations= 8000000 memory=      88 bytes
 => create+destroy time=1.186 read time = 0.389 total time = 1.575 result=0
Appending to Deque:           n=       1 iterations= 8000000 memory=     144 bytes
 => create+destroy time=1.167 read time = 0.383 total time = 1.550 result=0
Appending to SplStack:        n=       1 iterations= 8000000 memory=     184 bytes
 => create+destroy time=1.839 read time = 0.751 total time = 2.589 result=0
Appending to SplFixedArray:   n=       1 iterations= 8000000 memory=      80 bytes
 => create+destroy time=2.096 read time = 0.439 total time = 2.536 result=0


Appending to array:           n=       4 iterations= 2000000 memory=     216 bytes
 => create+destroy time=0.330 read time = 0.148 total time = 0.478 result=4000000
Appending to Vector:          n=       4 iterations= 2000000 memory=     144 bytes
 => create+destroy time=0.434 read time = 0.175 total time = 0.609 result=4000000
Appending to BitVector:       n=       4 iterations= 2000000 memory=      88 bytes
 => create+destroy time=0.449 read time = 0.186 total time = 0.635 result=4000000
Appending to LowMemoryVector: n=       4 iterations= 2000000 memory=      88 bytes
 => create+destroy time=0.432 read time = 0.187 total time = 0.620 result=4000000
Appending to Deque:           n=       4 iterations= 2000000 memory=     144 bytes
 => create+destroy time=0.453 read time = 0.184 total time = 0.636 result=4000000
Appending to SplStack:        n=       4 iterations= 2000000 memory=     280 bytes
 => create+destroy time=0.836 read time = 0.303 total time = 1.139 result=4000000
Appending to SplFixedArray:   n=       4 iterations= 2000000 memory=     128 bytes
 => create+destroy time=1.330 read time = 0.217 total time = 1.547 result=4000000


Appending to array:           n=       8 iterations= 1000000 memory=     216 bytes
 => create+destroy time=0.250 read time = 0.106 total time = 0.356 result=5000000
Appending to Vector:          n=       8 iterations= 1000000 memory=     208 bytes
 => create+destroy time=0.342 read time = 0.149 total time = 0.491 result=5000000
Appending to BitVector:       n=       8 iterations= 1000000 memory=      88 bytes
 => create+destroy time=0.332 read time = 0.152 total time = 0.485 result=5000000
Appending to LowMemoryVector: n=       8 iterations= 1000000 memory=      88 bytes
 => create+destroy time=0.312 read time = 0.165 total time = 0.477 result=5000000
Appending to Deque:           n=       8 iterations= 1000000 memory=     208 bytes
 => create+destroy time=0.351 read time = 0.155 total time = 0.505 result=5000000
Appending to SplStack:        n=       8 iterations= 1000000 memory=     408 bytes
 => create+destroy time=0.654 read time = 0.258 total time = 0.912 result=5000000
Appending to SplFixedArray:   n=       8 iterations= 1000000 memory=     192 bytes
 => create+destroy time=1.210 read time = 0.190 total time = 1.400 result=5000000


Appending to array:           n=    1024 iterations=    8000 memory=   20536 bytes
 => create+destroy time=0.179 read time = 0.076 total time = 0.255 result=5456000
Appending to Vector:          n=    1024 iterations=    8000 memory=   16464 bytes
 => create+destroy time=0.202 read time = 0.117 total time = 0.320 result=5456000
Appending to BitVector:       n=    1024 iterations=    8000 memory=     240 bytes
 => create+destroy time=0.194 read time = 0.125 total time = 0.319 result=5456000
Appending to LowMemoryVector: n=    1024 iterations=    8000 memory=    1104 bytes
 => create+destroy time=0.184 read time = 0.128 total time = 0.312 result=5456000
Appending to Deque:           n=    1024 iterations=    8000 memory=   16464 bytes
 => create+destroy time=0.208 read time = 0.123 total time = 0.331 result=5456000
Appending to SplStack:        n=    1024 iterations=    8000 memory=   32920 bytes
 => create+destroy time=0.443 read time = 0.154 total time = 0.596 result=5456000
Appending to SplFixedArray:   n=    1024 iterations=    8000 memory=   16448 bytes
 => create+destroy time=1.005 read time = 0.142 total time = 1.147 result=5456000


Appending to array:           n= 1048576 iterations=      20 memory=16781392 bytes
 => create+destroy time=0.665 read time = 0.203 total time = 0.868 result=13981000
Appending to Vector:          n= 1048576 iterations=      20 memory=16777320 bytes
 => create+destroy time=0.742 read time = 0.306 total time = 1.048 result=13981000
Appending to BitVector:       n= 1048576 iterations=      20 memory=  147536 bytes
 => create+destroy time=0.466 read time = 0.308 total time = 0.774 result=13981000
Appending to LowMemoryVector: n= 1048576 iterations=      20 memory= 1048656 bytes
 => create+destroy time=0.438 read time = 0.317 total time = 0.756 result=13981000
Appending to Deque:           n= 1048576 iterations=      20 memory=16777320 bytes
 => create+destroy time=0.757 read time = 0.317 total time = 1.074 result=13981000
Appending to SplStack:        n= 1048576 iterations=      20 memory=33554584 bytes
 => create+destroy time=1.183 read time = 0.412 total time = 1.596 result=13981000
Appending to SplFixedArray:   n= 1048576 iterations=      20 memory=16777304 bytes
 => create+destroy time=2.935 read time = 0.371 total time = 3.306 result=13981000

*/
