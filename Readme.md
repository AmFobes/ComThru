# Comthru

## ComThru is a quickly thrown together utility that utilises ckermit

- An example usage of ComThru is
	`./comthru file.txt`

- An example of a ComThru.cfg file is

```
port=/dev/ttyUSB0
baudrate=115200
```

ComThru has the extended ability to force binary mode transfer using the argument `-b`

&ensp;An example of this is: `./comthru file.txt -b`


## Compiling ComThru is easy; just do 

`gcc ComThru.cpp -o comthru`

and you should be all set

## Note:
- ComThru only works on **posix compliant systems**, MinGW or Crygwin is untested
- Verified working on **Arm64** and **X86_64**
- If you would like custom commands for ckermit, make a pull request / issue or just change the source yourself and recompile
- The CKermit Source can be located [Here](http://www.columbia.edu/kermit/ck90.html#source)

