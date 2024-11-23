# Writeup: netflaw

## Step 1: finding the bug
Reversing the binary we can observe that there is an overflow in the `parse_netflow_v5_records` function:
```c
1 : 
2 : void * get_netflow_v5_from_data(long param_1,long param_2)
3 : {
4 :   void *pvVar1;
5 :   void *pvVar2;
6 :  
7 :   pvVar1 = malloc(0x30);
8 :   parse_netflow_v5_header(param_1,param_2,pvVar1);
9 :   pvVar2 = malloc((ulong)*(ushort *)((long)pvVar1 + 8) * 0x30);
10:   *(void **)((long)pvVar1 + 0x28) = pvVar2;
11:   parse_netflow_v5_records(param_1 + 0x18,param_2 + -0x18,*(undefined8 *)((long)pvVar1 + 0x28));
12:   return pvVar1;
13: }
14:
```
On line _9_ a chunk is allocated to contain all the records, and the size is calculated using the `count` field
of the header, which is user controlled (via the UDP packet).
Later when calling the `parse_netflow_v5_records` function, the UDP packet lenght-size(header) is passed
as second parameter, which should be the size of the records chunk, but if it's not, we can overflow the chunk.


```c
01:
02: uint32_t * parse_netflow_v5_records(void *param_1,ulong param_2,uint32_t *param_3)
03:
04: {
05:   uint16_t uVar1;
06:   uint32_t uVar2;
07:   ulong local_28;
08:   void *local_20;
09:   uint32_t *local_10;
10:
11:   local_28 = param_2;
12:   local_20 = param_1;
13:   local_10 = param_3;
14:
15:   while( true ) {
16:     if (local_28 == 0) {
17:       return param_3;
18:     }
19:     if (local_28 < 0x30) break;
20:
21:     memcpy(local_10,local_20,0x30);
22:     uVar2 = ntohl(*local_10);
[...]
46:     local_20 = (void *)((long)local_20 + 0x30);
47:     local_28 = local_28 - 0x30;
48:     param_3 = local_10;
49:     local_10 = local_10 + 0xc;
50:   }
51:   fwrite("Buffer too small to contain a NetFlow v5 record\n",1,0x30,stderr);
52:   return (uint32_t *)0xffffffff;
53: }
54:
```
The lenght is checked to be at least 0x30, and then used to copy the records into a new chunk at line _21_.
Then at line _47_ the lenght is decremented by 0x30 until it reaches a value lower than 0x30.

After this, on line 25 of the main function the `print_netflow_v5` function is called that prints all the records.

This permits us to:
1. **read** past the chunk when the `count` in the Netflow header is **bigger** then 
 the count of real records inside the packet. This gives us a potential _read_ primitive.
2. **write** past the chunk when the `count` in the Netflow header is **smaller** then 
 the count of real records inside the packet. This gives us a potential _write what where_ primitive.

## Step 2: find a way to read the flag (exploit strategy)

When we look at the `main` function we see that in every cycle the program tries to read a config file from disc:
```c
02: undefined8 main(void)
03:
04: {
[...]
14:  local_10 = (ushort *)getDefaultConfig();
15:  local_14 = bind_udp_socket(0x1f90);
16:  local_20 = open_csv_file("netflow.csv");
17:  while( true ) {
18:    try_read_config_file(local_10);
19:    local_28 = next_flow(local_14,local_438,0x400);
[...]
```
On line _14_ a configuration is allocated to memory and then passed to the `try_read_config_file` function.


```c
02: void try_read_config_file(long param_1)
03: 
04: {
05:   int iVar1;
06:   
07:   iVar1 = read_file_to_buffer(param_1 + 0x12,param_1 + 0x22,500);
08:   if (iVar1 == 0) {
09:    parse_config_file_content(param_1 + 0x22,param_1);
10:  }
11:  return;
12:}
```
In this function a file is read we see that the content of a file is read into a buffer (line _7_)
and then parsed (line _9_). You notice that both the filename (`param_1`) and the file content (`param_1 + 0x22`)
point to memory allocated in the `getDefaultConfig` function.

This means, that if we can alter the filename string inside this configuration, the content of this file will be loaded
into that memory. This is a perfect way to read the flag, as we can just overwrite the filename with the path to the flag
using a _write what where_ primitive and read the content using a _read_ primitive.

Therefore we will need to:
1. know the address of the configuration chunk
2. overwrite the filename with the path to the flag
3. read the flag from the configuration chunk


## Step 3: getting the address of the configuration chunk

First of all we need to have a _read_ primitive to get the address of the configuration chunk.
We can achieve this by sending a packet with a `count` field bigger than the actual number of records in the packet.
This can be achieved by sending first a packet with a `count` field of 0x1, and then
a packet with a `count` field of 0x2 but with the actual length of the packet being 0x0.

This way, when the packet is printed by the application, the first record will be printed with the content of
chunk that was not cleaned properly from it's addresses (and leak the heap location). Whit that address we can
calculate the heap base address.

We will then quickly notice that the configuration chunk is always allocated at the same offset from the heap base
and therefore calculate it.

## Step 4: Overwriting the filename

After some investigation you can observe that in the endless loop _3_ chunks of _0x30_ bytes are always allocated and that
the size of the records size is _0x30_ * `count_in_header`. That means that we can
1. allocate another chunk of _0x30_ bytes if we put _1_ as count
2. allocate a chunk of the same size of the configuration chunk if we put _11_ as count (_0x30 * 11 = 0x1e0_)

### Step 4.1: Poison the tcache

By doing some heap fen-shui we can override the next pointer of the tcache entry of size _0x30_
with the address of the configuration chunk and allocate this chunk right after.

### Step 4.2: Overwrite the filename

After another heap fen-shui we can allocate the configuration chunk using a record count of _11_ and overwrite the filename
by putting the data correctly encoded in the records part of the packet.

## Step 5: Read the flag

With the same technique as in step 4.2 we can read the flag from the configuration chunk by sending another packet using
a record count of _11_ but with no content in order to make allocate the configuration chunk and print it's content with
the `print_netflow_v5` function. By reading that output and converting the struct to the right bytes we can decode the flag.