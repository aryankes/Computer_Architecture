; Simple algorithm to load int into Memory
; base array index =200
; arr[0]=5;
;arr[1]=6;
;arr[2]=4;
;loading values to memory
ldc 200
a2sp
;sp loaded to 1000
ldc 5 ; A=5
stl 0 ; arr[0]=5
ldc 6; A=6
stl 1 ; arr[1]=6
ldc 4; A=4
stl 2;
HALT
