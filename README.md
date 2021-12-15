# MuOnArgon
Simulate high energy muon interactions in a large Argon volume with Geant4.

Attempt to research results in G. Zhu, S.W. Li and J.F. Beacom, Phys. Rev. C99 (2019) 055810. 

Set up simplified DarkSide-20k geometry with main structures and liquid argon volumes and simulate Ion production from initial high-energy muons, using underground 
muon event generator. Production yields are the primary target. The simulation builds on the Geant4 framework, version 10.7.

## Build instruction

At Warwick, SCRTP, use cvmfs as the easiest environment setup (with bash):

source /cvmfs/sft.cern.ch/lcg/views/LCG_99/x86_64-centos7-gcc10-opt/setup.sh

which sets up Geant4 10.7 and GCC10 on a CentOS7 background. ROOT 6.22 will also be available. Just create a 'build' directory, then 

```console
$ mkdir build && cd build
$ cmake ..
$ make
```

## Volume Codes

Sensitive detector volumes:
- Outer Buffer volume = 7
- Inner Buffer volume = 9
- TPC volume = 11
