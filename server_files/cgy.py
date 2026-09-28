#!/usr/bin/python3
import os

body = b"Hello from the CGI!\n"
response = (
    b"HTTP/1.1 200 OK\r\n"
    b"Content-Type: text/plain; charset=utf-8\r\n"
    + b"Content-Length: " + str(len(body)).encode("ascii") + b"\r\n"
    + b"Connection: close\r\n"
    + b"\r\n"
    + body
)

while response:
    response = response[os.write(1, response):]
