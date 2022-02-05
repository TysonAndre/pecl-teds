<?php

const CHAR_LOOKUP = [
    "\n" => '\n',
    "\t" => '\t',
    "\r" => '\r',
    '"' => '\"',
    '\\' => '\\\\',
    '$' => '\$',
];
function encode_raw_string(string $raw): string {
    return '"' . \preg_replace_callback(
        '/[\\x00-\\x1f\\x7f-\\xff\\\\"$]/',
        /** @param array{0:string} $match */
        static function (array $match): string {
            $char = $match[0];
            return CHAR_LOOKUP[$char] ?? \sprintf('\x%02x', \ord($char));
        },
        $raw
    ) . '"';
}
