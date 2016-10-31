IKBD to USB converter
=====================

IKDB is the keyboard controller found in Atari ST and later Atari computers.


Connector
---------

### Internal connector for ST/Falcon

<pre>
1	+   - Ground
2	-   - NC (key)
3	+   - floppy light
4	+   - +5V
5	+   - Rx data (Atari Rx - keyboard Tx)
6	+   - Tx data (Atari Tx - keyboard Rx)
7	+   - Reset (active low)
8   +   - Ground
</pre>

Protocol
--------
Transmission speed is 7812.5 bit/s

Protocol information can be found here:
<https://www.kernel.org/doc/Documentation/input/atarikbd.txt>


