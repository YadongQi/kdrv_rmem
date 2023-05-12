# How to Use

## Get EPTP

Ref: https://github.com/YadongQi/ebpf_sample
```
sudo apt install bpfcc-tools
sudo trace-bpfcc 'r::construct_eptp "%llx, %x, %llx", arg2, arg3, retval'
```

Launch a VM with QEMU/KVM.

You should get below log of bpf:
```
PID     TID     COMM            FUNC             -
2604510 2604515 qemu-system-x86 construct_eptp   1b9054000, 4, 1b905405e
2604510 2604515 qemu-system-x86 construct_eptp   1b9054000, 4, 1b905405e
2604510 2604515 qemu-system-x86 construct_eptp   13697a000, 4, 13697a05e
2604510 2604515 qemu-system-x86 construct_eptp   13697a000, 4, 13697a05e
2604510 2604515 qemu-system-x86 construct_eptp   1601c6000, 4, 1601c605e
2604510 2604515 qemu-system-x86 construct_eptp   1601c6000, 4, 1601c605e
2604510 2604515 qemu-system-x86 construct_eptp   25195f000, 4, 25195f05e
2604510 2604515 qemu-system-x86 construct_eptp   25195f000, 4, 25195f05e
2604510 2604515 qemu-system-x86 construct_eptp   c24c48000, 4, c24c4805e
```

The log will pause when guest boot done and be stable, the lastest log
indicates its EPTP, e.g.(above as sample): **c24c4805e**

## Build this tool
```
sudo apt install build-essential -y
sudo apt-get install linux-headers-$(uname -r)
make
```

## Load module
```
sudo insmod driver/kdrv_rmem.ko
# Check /dev/rmem
sudo chmod a+rw /dev/rmem
```

## Use dump_ept
```
# input param with EPTP which get from first step
./build/dump_pt 0xc24c4805e
```
