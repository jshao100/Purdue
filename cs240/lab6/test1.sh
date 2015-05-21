#!/usr/bin/expect

spawn telnet localhost 1983
expect "'^]'."
send "ADD-USER user1 123\r\n"
expect "OK\r\n"
