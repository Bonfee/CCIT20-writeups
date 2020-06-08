#!/usr/bin/python3

import string
import itertools
import base64
import sys
import threading


def status():
    global counter
    print(str(counter / 10) + " key/s")
    t = threading.Timer(10.0, status)
    counter = 0
    t.start()

def encrypt(clear, key):
    enc = []
    for i in range(len(clear)):
        key_c = key[i % len(key)]
        enc_c = chr((ord(clear[i]) + ord(key_c)) % 128)
        enc.append(enc_c)
    return str(base64.urlsafe_b64encode("".join(enc).encode('ascii')), 'ascii')

try_size = 8
key_size = 8

counter = 0
t = threading.Timer(10.0, status)

iter_over = itertools.product(string.ascii_lowercase, repeat = try_size)

m = "See you later in the city center"
c = "QSldSTQ7HkpIJj9cQBY3VUhbQ01HXD9VRBVYSkE6UWRQS0NHRVE3VUQrTDE="


t.start()
for key in iter_over:
    counter += 1
    rev_key = "".join(key)[::-1]
    rev_key += "a" * (key_size - try_size)
    k1 = rev_key[0:4]
    k2 = rev_key[4:8]
    d = encrypt(m, k1)
    tmp_c = encrypt(d, k2)
    print(k1 + k2 + ' - ' + d)
    print(k1 + k2 + ' - ' + tmp_c)
    if (tmp_c == c):
        print("-" * 100)
        print(s1 + s2)
        sys.exit(0)
