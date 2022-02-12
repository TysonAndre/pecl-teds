--TEST--
Teds\BitSet indexOf/contains
--FILE--
<?php
// discards keys
$it = new Teds\BitSet();
var_dump($it->indexOf(false));
var_dump($it->indexOf(true));
var_dump($it->contains(false));
var_dump($it->contains(true));
echo "Build 1-element\n";
$it = new Teds\BitSet([true]);
var_dump($it->indexOf(false));
var_dump($it->indexOf(true));
var_dump($it->contains(false));
var_dump($it->contains(true));
echo "Build 1-element false\n";
$it = new Teds\BitSet([false]);
var_dump($it->indexOf(false));
var_dump($it->indexOf(true));
var_dump($it->contains(false));
var_dump($it->contains(true));
echo "Build 4-element\n";
$it = new Teds\BitSet([true, true, false, false]);
var_dump($it->indexOf(false));
var_dump($it->indexOf(true));
var_dump($it->contains(false));
var_dump($it->contains(true));
echo "Build large bitset\n";
$it = new Teds\BitSet();
$it->setSize(8, false);
var_dump($it->indexOf(false));
var_dump($it->indexOf(true));
$it->setSize(13, true);
var_dump($it->indexOf(false));
var_dump($it->indexOf(true));
printf("count=%d capacity=%d value=%s\n", $it->count(), $it->capacity(), json_encode($it));
$it = new Teds\BitSet();
$it->setSize(789, true);
var_dump($it->indexOf(false));
var_dump($it->indexOf(true));
$it->setSize(1234, false);
var_dump($it->indexOf(false));
var_dump($it->indexOf(true));
printf("count=%d capacity=%d\n", $it->count(), $it->capacity());
$it->setSize(2, true);
printf("count=%d capacity=%d values=%s\n", $it->count(), $it->capacity(), json_encode($it));
$it->setSize(2, false);
printf("count=%d capacity=%d values=%s\n", $it->count(), $it->capacity(), json_encode($it));
$it->setSize(0, false);
printf("count=%d capacity=%d values=%s\n", $it->count(), $it->capacity(), json_encode($it));
?>
--EXPECT--
NULL
NULL
bool(false)
bool(false)
Build 1-element
int(0)
NULL
bool(true)
bool(false)
Build 1-element false
NULL
int(0)
bool(false)
bool(true)
Build 4-element
int(0)
int(2)
bool(true)
bool(true)
Build large bitset
NULL
int(0)
int(8)
int(0)
count=13 capacity=64 value=[false,false,false,false,false,false,false,false,true,true,true,true,true]
int(0)
NULL
int(0)
int(789)
count=1234 capacity=1600
count=2 capacity=64 values=[true,true]
count=2 capacity=64 values=[true,true]
count=0 capacity=64 values=[]