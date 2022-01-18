<?php

/** @generate-class-entries */
// Stub generation requires build/gen_stub.php from php 8.1 or newer.

namespace Teds;

class StableMaxHeap extends SplMaxHeap
{
    public function compare(mixed $value1, mixed $value2): int {
        return stable_compare($value1, $value2);
    }
}

class StableMinHeap extends SplMinHeap
{
    public function compare(mixed $value1, mixed $value2): int {
        return stable_compare($value2, $value1);
    }
}
