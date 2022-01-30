--TEST--
Teds\SortedStrictSet rebalances
--FILE--
<?php
$it = new Teds\SortedStrictSet();
for ($i = 'aa'; $i < 'ak'; $i++) {
    $it->add($i);
}
echo json_encode($it->debugGetTreeRepresentation(), JSON_PRETTY_PRINT), "\n";
var_dump($it);
?>
--EXPECT--
{
    "key": "ad",
    "red": true,
    "left": {
        "key": "ab",
        "red": false,
        "left": {
            "key": "aa",
            "red": false,
            "left": [],
            "right": []
        },
        "right": {
            "key": "ac",
            "red": false,
            "left": [],
            "right": []
        }
    },
    "right": {
        "key": "af",
        "red": false,
        "left": {
            "key": "ae",
            "red": false,
            "left": [],
            "right": []
        },
        "right": {
            "key": "ah",
            "red": true,
            "left": {
                "key": "ag",
                "red": false,
                "left": [],
                "right": []
            },
            "right": {
                "key": "ai",
                "red": false,
                "left": [],
                "right": {
                    "key": "aj",
                    "red": true,
                    "left": [],
                    "right": []
                }
            }
        }
    }
}
object(Teds\SortedStrictSet)#1 (10) {
  [0]=>
  string(2) "aa"
  [1]=>
  string(2) "ab"
  [2]=>
  string(2) "ac"
  [3]=>
  string(2) "ad"
  [4]=>
  string(2) "ae"
  [5]=>
  string(2) "af"
  [6]=>
  string(2) "ag"
  [7]=>
  string(2) "ah"
  [8]=>
  string(2) "ai"
  [9]=>
  string(2) "aj"
}