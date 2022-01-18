--TEST--
Teds\StableMinHeap and StableMaxHeap are extensible
--FILE--
<?php
class ExtendedStableMinHeap extends Teds\StableMinHeap {
    public function range(int $start, int $end) {
        $count = $this->count();
        if ($end <= $start || $start >= $count) {
            return [];
        }
        if ($count < $end) {
            $end = $count;
        }
        $c = clone($this);
        for ($i = 0; $i < $start; $i++) {
            $c->extract();
        }
        $result = [];
        for (; $i < $end; $i++) {
            $result[] = $c->extract();
        }
        return $result;
    }
}
$minHeap = new ExtendedStableMinHeap();
foreach (['19', '2', false, true, null, []] as $v) {
    $minHeap->insert($v);
}
var_dump($minHeap->range(0, 3));
var_dump($minHeap->range(3, 5));
?>
--EXPECT--
array(3) {
  [0]=>
  NULL
  [1]=>
  bool(false)
  [2]=>
  bool(true)
}
array(2) {
  [0]=>
  string(2) "19"
  [1]=>
  string(1) "2"
}
