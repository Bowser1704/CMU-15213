# bomb Lab

利用`gdb`调试，找出要输入的值，总共有6个phase。

## phase 1

```assembly
Dump of assembler code for function phase_1:
=> 0x0000000000400ee0 <+0>:	sub    $0x8,%rsp
   0x0000000000400ee4 <+4>:	mov    $0x402400,%esi
   0x0000000000400ee9 <+9>:	callq  0x401338 <strings_not_equal>
   0x0000000000400eee <+14>:	test   %eax,%eax
   0x0000000000400ef0 <+16>:	je     0x400ef7 <phase_1+23>
   0x0000000000400ef2 <+18>:	callq  0x40143a <explode_bomb>
   0x0000000000400ef7 <+23>:	add    $0x8,%rsp
   0x0000000000400efb <+27>:	retq 
```

可以看到第三行，调用了 `strings_not_equal` 函数， 并且我们可以根据寄存器使用规则知道`%esi`寄存器是第二个``arg`，第一个`arg`就是我们自己的输入，并且第二行把`$0x402400`放入`%esi`中。

所以重点就在`%esi`里面。也就是`0x402400`这里面的东西。

第四五行对返回值进行确定，如果`%eax`是0的话，正常return，否则bomb。

> 这里要明确的是`%rax`为返回值寄存器
>
> `strings_not_equal` 函数 相等返回0，参数为`%esi`和`%rdi`。

```assembly
(gdb) x/s 0x402400
0x402400:	"Border relations with Canada have never been better."
```

答案就是："Border relations with Canada have never been better."

## phase 2

```assembly
Dump of assembler code for function phase_2:
=> 0x0000000000400efc <+0>:	push   %rbp       #栈帧底部，新建栈帧
   0x0000000000400efd <+1>:	push   %rbx       #被调用者保存寄存器
   0x0000000000400efe <+2>:	sub    $0x28,%rsp #申请40个字节扩大栈
   0x0000000000400f02 <+6>:	mov    %rsp,%rsi  
   0x0000000000400f05 <+9>:	callq  0x40145c <read_six_numbers>
   0x0000000000400f0a <+14>:	cmpl   $0x1,(%rsp)   #a[0]==1
   0x0000000000400f0e <+18>:	je     0x400f30 <phase_2+52>
   0x0000000000400f10 <+20>:	callq  0x40143a <explode_bomb>
   0x0000000000400f15 <+25>:	jmp    0x400f30 <phase_2+52>
   0x0000000000400f17 <+27>:	mov    -0x4(%rbx),%eax  #%rbx-%rax = 4 一个int
   0x0000000000400f1a <+30>:	add    %eax,%eax     # 2*(%eax)= %rbx 即a[i+1] = 2*a[i]
   0x0000000000400f1c <+32>:	cmp    %eax,(%rbx)   
   0x0000000000400f1e <+34>:	je     0x400f25 <phase_2+41>
   0x0000000000400f20 <+36>:	callq  0x40143a <explode_bomb>
   0x0000000000400f25 <+41>:	add    $0x4,%rbx     #%rbx+4 即a[i+1]
   0x0000000000400f29 <+45>:	cmp    %rbp,%rbx     #终止条件
   0x0000000000400f2c <+48>:	jne    0x400f17 <phase_2+27>
   0x0000000000400f2e <+50>:	jmp    0x400f3c <phase_2+64>
   0x0000000000400f30 <+52>:	lea    0x4(%rsp),%rbx
   0x0000000000400f35 <+57>:	lea    0x18(%rsp),%rbp	#0x18是16进制，4*6=24即为6个数，不是加20因为a[5]还是要用的，a[6]才可以return。
   0x0000000000400f3a <+62>:	jmp    0x400f17 <phase_2+27>
   0x0000000000400f3c <+64>:	add    $0x28,%rsp
   0x0000000000400f40 <+68>:	pop    %rbx
   0x0000000000400f41 <+69>:	pop    %rbp
   0x0000000000400f42 <+70>:	retq   
End of assembler dump.
```

由最开始一定为1，后面乘2，也就是1 2 4 8 16 32

```c
//phase_2()
if(a[0] != 1) bomb();
for(int i=1; i<6; i++) {
    if(a[i+1] != 2*a[i]) bomb();
}
```

## phase 3

```assembly
Dump of assembler code for function phase_3:
   0x0000000000400f43 <+0>:	sub    $0x18,%rsp
   0x0000000000400f47 <+4>:	lea    0xc(%rsp),%rcx
   0x0000000000400f4c <+9>:	lea    0x8(%rsp),%rdx
   0x0000000000400f51 <+14>:	mov    $0x4025cf,%esi	#第二个输入，应该只有两个输入。
   0x0000000000400f56 <+19>:	mov    $0x0,%eax	#caller-saved register
   0x0000000000400f5b <+24>:	callq  0x400bf0 <__isoc99_sscanf@plt>
   0x0000000000400f60 <+29>:	cmp    $0x1,%eax	#sscanf的返回值是匹配了几个值，eax的值最大为2，输入也就是2个。
   0x0000000000400f63 <+32>:	jg     0x400f6a <phase_3+39>
   0x0000000000400f65 <+34>:	callq  0x40143a <explode_bomb>
   0x0000000000400f6a <+39>:	cmpl   $0x7,0x8(%rsp)			#第一个arg<=7
   0x0000000000400f6f <+44>:	ja     0x400fad <phase_3+106>	#不大于
   0x0000000000400f71 <+46>:	mov    0x8(%rsp),%eax
   0x0000000000400f75 <+50>:	jmpq   *0x402470(,%rax,8)
   0x0000000000400f7c <+57>:	mov    $0xcf,%eax
   0x0000000000400f81 <+62>:	jmp    0x400fbe <phase_3+123>
   0x0000000000400f83 <+64>:	mov    $0x2c3,%eax
   0x0000000000400f88 <+69>:	jmp    0x400fbe <phase_3+123>
   0x0000000000400f8a <+71>:	mov    $0x100,%eax
   0x0000000000400f8f <+76>:	jmp    0x400fbe <phase_3+123>
   0x0000000000400f91 <+78>:	mov    $0x185,%eax
   0x0000000000400f96 <+83>:	jmp    0x400fbe <phase_3+123>
   0x0000000000400f98 <+85>:	mov    $0xce,%eax
   0x0000000000400f9d <+90>:	jmp    0x400fbe <phase_3+123>
   0x0000000000400f9f <+92>:	mov    $0x2aa,%eax
   0x0000000000400fa4 <+97>:	jmp    0x400fbe <phase_3+123>
   0x0000000000400fa6 <+99>:	mov    $0x147,%eax
   0x0000000000400fab <+104>:	jmp    0x400fbe <phase_3+123>
   0x0000000000400fad <+106>:	callq  0x40143a <explode_bomb>
   0x0000000000400fb2 <+111>:	mov    $0x0,%eax
   0x0000000000400fb7 <+116>:	jmp    0x400fbe <phase_3+123>
   0x0000000000400fb9 <+118>:	mov    $0x137,%eax
=> 0x0000000000400fbe <+123>:	cmp    0xc(%rsp),%eax	#第二个arg与eax比较，在这里打印eax的值为682
   0x0000000000400fc2 <+127>:	je     0x400fc9 <phase_3+134>	#相等就可以过
   0x0000000000400fc4 <+129>:	callq  0x40143a <explode_bomb>
   0x0000000000400fc9 <+134>:	add    $0x18,%rsp
   0x0000000000400fcd <+138>:	retq   
End of assembler dump.
```

最终的结果为两个值，第一个<=7，第二个为`%eax`的值。

```assembly
(gdb) p/d $eax
$3 = 682
```

所以答案可以为 `6 682`

## phase 4

```assembly
Dump of assembler code for function phase_4:
=> 0x000000000040100c <+0>:	sub    $0x18,%rsp
   0x0000000000401010 <+4>:	lea    0xc(%rsp),%rcx
   0x0000000000401015 <+9>:	lea    0x8(%rsp),%rdx
   0x000000000040101a <+14>:	mov    $0x4025cf,%esi
   0x000000000040101f <+19>:	mov    $0x0,%eax
   0x0000000000401024 <+24>:	callq  0x400bf0 <__isoc99_sscanf@plt>
   0x0000000000401029 <+29>:	cmp    $0x2,%eax	#eax返回值必须要为2，也就是两个输入，试了一下知道是两个数字。
   0x000000000040102c <+32>:	jne    0x401035 <phase_4+41>
   0x000000000040102e <+34>:	cmpl   $0xe,0x8(%rsp)	#rsp+8为第一个输入,<=0xe才可以
   0x0000000000401033 <+39>:	jbe    0x40103a <phase_4+46>
   0x0000000000401035 <+41>:	callq  0x40143a <explode_bomb>
   0x000000000040103a <+46>:	mov    $0xe,%edx	#func4第三个arg
   0x000000000040103f <+51>:	mov    $0x0,%esi	#func4第二个arg
   0x0000000000401044 <+56>:	mov    0x8(%rsp),%edi	#%edi func4第一个arg，第一个输入
   0x0000000000401048 <+60>:	callq  0x400fce <func4>
   0x000000000040104d <+65>:	test   %eax,%eax	#返回值必须要为0,否则bomb
   0x000000000040104f <+67>:	jne    0x401058 <phase_4+76>	# ~ZF符号位
   0x0000000000401051 <+69>:	cmpl   $0x0,0xc(%rsp)	#rsp+c是第二个输入，为0
   0x0000000000401056 <+74>:	je     0x40105d <phase_4+81>
   0x0000000000401058 <+76>:	callq  0x40143a <explode_bomb>
   0x000000000040105d <+81>:	add    $0x18,%rsp
   0x0000000000401061 <+85>:	retq   
End of assembler dump.
```

所以根据`phase_4`，我们可以判断出来

> - 有两个输入
> - 第一个输入 <= 14
> - 第二个输入 == 0
> - `func4`的三个``arg`分别为，输入一，0， 14、必须要返回0

下面看看``func4`

```assembly
Dump of assembler code for function func4:
=> 0x0000000000400fce <+0>:	sub    $0x8,%rsp
   0x0000000000400fd2 <+4>:	mov    %edx,%eax	#arg3
   0x0000000000400fd4 <+6>:	sub    %esi,%eax	#arg3-arg2
   0x0000000000400fd6 <+8>:	mov    %eax,%ecx	#ecx = arg3-arg2
   0x0000000000400fd8 <+10>:	shr    $0x1f,%ecx	#逻辑右移31位
   0x0000000000400fdb <+13>:	add    %ecx,%eax	#eax = arg3-arg2 + (arg3 - arg2) >> 31 /逻辑右移	也就是差大于0还是小于0 答案应该是14
   0x0000000000400fdd <+15>:	sar    %eax
   0x0000000000400fdf <+17>:	lea    (%rax,%rsi,1),%ecx
   0x0000000000400fe2 <+20>:	cmp    %edi,%ecx	#比较edi与ecx，输出ecx为7.这里是<=7,下面是>=7
   0x0000000000400fe4 <+22>:	jle    0x400ff2 <func4+36>
   0x0000000000400fe6 <+24>:	lea    -0x1(%rcx),%edx
   0x0000000000400fe9 <+27>:	callq  0x400fce <func4>
   0x0000000000400fee <+32>:	add    %eax,%eax
   0x0000000000400ff0 <+34>:	jmp    0x401007 <func4+57>
   0x0000000000400ff2 <+36>:	mov    $0x0,%eax
   0x0000000000400ff7 <+41>:	cmp    %edi,%ecx	#>=7就出去，返回0
   0x0000000000400ff9 <+43>:	jge    0x401007 <func4+57>
   0x0000000000400ffb <+45>:	lea    0x1(%rcx),%esi
   0x0000000000400ffe <+48>:	callq  0x400fce <func4>
   0x0000000000401003 <+53>:	lea    0x1(%rax,%rax,1),%eax
   0x0000000000401007 <+57>:	add    $0x8,%rsp
   0x000000000040100b <+61>:	retq   
End of assembler dump.
```

综上所述，答案为`7 0`  。

## phase 5

```assembly
Dump of assembler code for function phase_5:
   0x0000000000401062 <+0>:	push   %rbx
   0x0000000000401063 <+1>:	sub    $0x20,%rsp
   0x0000000000401067 <+5>:	mov    %rdi,%rbx
   0x000000000040106a <+8>:	mov    %fs:0x28,%rax
   0x0000000000401073 <+17>:	mov    %rax,0x18(%rsp)
   0x0000000000401078 <+22>:	xor    %eax,%eax
   0x000000000040107a <+24>:	callq  0x40131b <string_length>
=> 0x000000000040107f <+29>:	cmp    $0x6,%eax	#返回值要等于6，字符串长度为6，并且x/s $rbx是自己输入的字符串。
   0x0000000000401082 <+32>:	je     0x4010d2 <phase_5+112>
   0x0000000000401084 <+34>:	callq  0x40143a <explode_bomb>
   0x0000000000401089 <+39>:	jmp    0x4010d2 <phase_5+112>
   ------循环
   0x000000000040108b <+41>:	movzbl (%rbx,%rax,1),%ecx	#%rax=0，上一步置0了，rbx为输入值,ecx为输入字符的ascii码。循环%rax递增
   0x000000000040108f <+45>:	mov    %cl,(%rsp)	#cl/rcx
   0x0000000000401092 <+48>:	mov    (%rsp),%rdx
   0x0000000000401096 <+52>:	and    $0xf,%edx	#每一个做一次and 0xf。这里是索引最大就是16，我们输入的值，得到索引，按顺序必须为flyers。
   0x0000000000401099 <+55>:	movzbl 0x4024b0(%rdx),%edx	#这是给定的字符串
   0x00000000004010a0 <+62>:	mov    %dl,0x10(%rsp,%rax,1)
   0x00000000004010a4 <+66>:	add    $0x1,%rax
   0x00000000004010a8 <+70>:	cmp    $0x6,%rax	#估计是循环6次
   0x00000000004010ac <+74>:	jne    0x40108b <phase_5+41>
   ------结束	
   0x00000000004010ae <+76>:	movb   $0x0,0x16(%rsp)
   0x00000000004010b3 <+81>:	mov    $0x40245e,%esi	#第二个arg，输出值为flyers，6个。
   0x00000000004010b8 <+86>:	lea    0x10(%rsp),%rdi
   0x00000000004010bd <+91>:	callq  0x401338 <strings_not_equal>
   0x00000000004010c2 <+96>:	test   %eax,%eax
   0x00000000004010c4 <+98>:	je     0x4010d9 <phase_5+119>
   0x00000000004010c6 <+100>:	callq  0x40143a <explode_bomb>
   0x00000000004010cb <+105>:	nopl   0x0(%rax,%rax,1)
   0x00000000004010d0 <+110>:	jmp    0x4010d9 <phase_5+119>
   0x00000000004010d2 <+112>:	mov    $0x0,%eax
   0x00000000004010d7 <+117>:	jmp    0x40108b <phase_5+41>
   0x00000000004010d9 <+119>:	mov    0x18(%rsp),%rax
   0x00000000004010de <+124>:	xor    %fs:0x28,%rax
   0x00000000004010e7 <+133>:	je     0x4010ee <phase_5+140>
   0x00000000004010e9 <+135>:	callq  0x400b30 <__stack_chk_fail@plt>
   0x00000000004010ee <+140>:	add    $0x20,%rsp
   0x00000000004010f2 <+144>:	pop    %rbx
   0x00000000004010f3 <+145>:	retq   
End of assembler dump.
```

上面有个放入`%esi`的，感觉是答案，输出一看。这是`strings_not_equal` 函数的第二个参数。另一个是`%rdi`。

```assembly
(gdb) x/s 0x40245e
0x40245e:	"flyers"
```

~~感觉答案就是这个了 `flyers`，然后肯定是bomb了，前面的6次循环是干什么的呢？~~

由上可以知道索引最大为16，``0x0f`

```assembly
(gdb) x/16c 0x4024b0
0x4024b0 <array.3449>:	109 'm'	97 'a'	100 'd'	117 'u'	105 'i'	101 'e'	114 'r'	115 's'
0x4024b8 <array.3449+8>:	110 'n'	102 'f'	111 'o'	116 't'	118 'v'	98 'b'	121 'y'	108 'l'
```

可以知道顺序为，10、16、15、 6、 7、 8。索引要减一，则6个数的后四位为：

```c
//1001 1111 1110 0101 0110 0111	//肯定只能为ascii码了，不然数字没那么大。
//)	/ . % & '
```

答案可以是`)/.%&'`，这六个，当然答案有6^6种。`ascii`码后四位固定，前三位可以改变。

## phase 6

最后一个代码太长了。。。。

```assembly
Dump of assembler code for function phase_6:
   -----第一段代码
   0x00000000004010f4 <+0>:	push   %r14
   0x00000000004010f6 <+2>:	push   %r13
   0x00000000004010f8 <+4>:	push   %r12
   0x00000000004010fa <+6>:	push   %rbp
   0x00000000004010fb <+7>:	push   %rbx
   0x00000000004010fc <+8>:	sub    $0x50,%rsp
   0x0000000000401100 <+12>:	mov    %rsp,%r13	#第一个数字
   0x0000000000401103 <+15>:	mov    %rsp,%rsi
   0x0000000000401106 <+18>:	callq  0x40145c <read_six_numbers>
=> 0x000000000040110b <+23>:	mov    %rsp,%r14	#读取6个数字
   0x000000000040110e <+26>:	mov    $0x0,%r12d
   -----循环1
   0x0000000000401114 <+32>:	mov    %r13,%rbp
   0x0000000000401117 <+35>:	mov    0x0(%r13),%eax
   0x000000000040111b <+39>:	sub    $0x1,%eax	#第一个减一
   0x000000000040111e <+42>:	cmp    $0x5,%eax	#与5比较
   0x0000000000401121 <+45>:	jbe    0x401128 <phase_6+52> #<=
   0x0000000000401123 <+47>:	callq  0x40143a <explode_bomb>
   0x0000000000401128 <+52>:	add    $0x1,%r12d	#加一
   0x000000000040112c <+56>:	cmp    $0x6,%r12d	#与6比较
   0x0000000000401130 <+60>:	je     0x401153 <phase_6+95>
   0x0000000000401132 <+62>:	mov    %r12d,%ebx	#ebx最大为5,6则跳出
   ----循环2
   0x0000000000401135 <+65>:	movslq %ebx,%rax
   0x0000000000401138 <+68>:	mov    (%rsp,%rax,4),%eax	#应该还是数组，这里是a+i
   0x000000000040113b <+71>:	cmp    %eax,0x0(%rbp)	#rbp还是第一个数,即数组内所有数字不能相等。
   0x000000000040113e <+74>:	jne    0x401145 <phase_6+81>
   0x0000000000401140 <+76>:	callq  0x40143a <explode_bomb>
   0x0000000000401145 <+81>:	add    $0x1,%ebx
   0x0000000000401148 <+84>:	cmp    $0x5,%ebx
   0x000000000040114b <+87>:	jle    0x401135 <phase_6+65>
   -----循环2终止
   0x000000000040114d <+89>:	add    $0x4,%r13	#加4个字节
   0x0000000000401151 <+93>:	jmp    0x401114 <phase_6+32>
   -----循环1终止
   # 上面的条件就是 6个数字，每个数字<=6，并且都不相等。
   0x0000000000401153 <+95>:	lea    0x18(%rsp),%rsi #rsi数组末尾
   0x0000000000401158 <+100>:	mov    %r14,%rax
   0x000000000040115b <+103>:	mov    $0x7,%ecx
   -----循环3
   0x0000000000401160 <+108>:	mov    %ecx,%edx
   0x0000000000401162 <+110>:	sub    (%rax),%edx	#这里(rax)=7-(rax)
   0x0000000000401164 <+112>:	mov    %edx,(%rax)
   0x0000000000401166 <+114>:	add    $0x4,%rax
   0x000000000040116a <+118>:	cmp    %rsi,%rax
   0x000000000040116d <+121>:	jne    0x401160 <phase_6+108>
   -----循环3终止
   
   -----第二段代码
   0x000000000040116f <+123>:	mov    $0x0,%esi
   0x0000000000401174 <+128>:	jmp    0x401197 <phase_6+163>
   0x0000000000401176 <+130>:	mov    0x8(%rdx),%rdx
   0x000000000040117a <+134>:	add    $0x1,%eax
   0x000000000040117d <+137>:	cmp    %ecx,%eax
   0x000000000040117f <+139>:	jne    0x401176 <phase_6+130>
   0x0000000000401181 <+141>:	jmp    0x401188 <phase_6+148>
   0x0000000000401183 <+143>:	mov    $0x6032d0,%edx	#重点
   0x0000000000401188 <+148>:	mov    %rdx,0x20(%rsp,%rsi,2)
   0x000000000040118d <+153>:	add    $0x4,%rsi
   0x0000000000401191 <+157>:	cmp    $0x18,%rsi
   0x0000000000401195 <+161>:	je     0x4011ab <phase_6+183>
   0x0000000000401197 <+163>:	mov    (%rsp,%rsi,1),%ecx
   0x000000000040119a <+166>:	cmp    $0x1,%ecx
   0x000000000040119d <+169>:	jle    0x401183 <phase_6+143>
   0x000000000040119f <+171>:	mov    $0x1,%eax
   0x00000000004011a4 <+176>:	mov    $0x6032d0,%edx
   0x00000000004011a9 <+181>:	jmp    0x401176 <phase_6+130>
   0x00000000004011ab <+183>:	mov    0x20(%rsp),%rbx
   ----
   0x00000000004011b0 <+188>:	lea    0x28(%rsp),%rax
   0x00000000004011b5 <+193>:	lea    0x50(%rsp),%rsi
   0x00000000004011ba <+198>:	mov    %rbx,%rcx
   0x00000000004011bd <+201>:	mov    (%rax),%rdx
   0x00000000004011c0 <+204>:	mov    %rdx,0x8(%rcx)
   0x00000000004011c4 <+208>:	add    $0x8,%rax
   0x00000000004011c8 <+212>:	cmp    %rsi,%rax
   0x00000000004011cb <+215>:	je     0x4011d2 <phase_6+222>
   0x00000000004011cd <+217>:	mov    %rdx,%rcx
   0x00000000004011d0 <+220>:	jmp    0x4011bd <phase_6+201>
   0x00000000004011d2 <+222>:	movq   $0x0,0x8(%rdx)
   0x00000000004011da <+230>:	mov    $0x5,%ebp
   -----
   0x00000000004011df <+235>:	mov    0x8(%rbx),%rax
   0x00000000004011e3 <+239>:	mov    (%rax),%eax
   0x00000000004011e5 <+241>:	cmp    %eax,(%rbx)	#rbx >= rbx+8
   0x00000000004011e7 <+243>:	jge    0x4011ee <phase_6+250>
   0x00000000004011e9 <+245>:	callq  0x40143a <explode_bomb>
   0x00000000004011ee <+250>:	mov    0x8(%rbx),%rbx	#rbx+8
   0x00000000004011f2 <+254>:	sub    $0x1,%ebp		#ebp循环条件,非零着跳转
   0x00000000004011f5 <+257>:	jne    0x4011df <phase_6+235>
   0x00000000004011f7 <+259>:	add    $0x50,%rsp
   0x00000000004011fb <+263>:	pop    %rbx
   0x00000000004011fc <+264>:	pop    %rbp
   0x00000000004011fd <+265>:	pop    %r12
   0x00000000004011ff <+267>:	pop    %r13
   0x0000000000401201 <+269>:	pop    %r14
   0x0000000000401203 <+271>:	retq   
End of assembler dump.
```

`%rsp`为输入值，一个4字节。假设我输入1 2 3 4 5 6

```assembly
(gdb) x/6w $rsp
0x7fffffffd5d0:	1	2	3	4
0x7fffffffd5e0:	5	6
```

由第一段代码知道输入为6个数，每个数<=6，且都不能相等，第一段最后还对每个数``a = 7 - a`操作。

第二段有点复杂，没屡的很清楚。

```assembly
(gdb) x/24w 0x6032d0
0x6032d0 <node1>:	332	1	6304480	0
0x6032e0 <node2>:	168	2	6304496	0
0x6032f0 <node3>:	924	3	6304512	0
0x603300 <node4>:	691	4	6304528	0
0x603310 <node5>:	477	5	6304544	0
0x603320 <node6>:	443	6	0	0
```

他是从``0x603320`开始的，每次循环输出`%eax`为输入顺序对应的数字，假设我输入4，对应的就是691，然后我们需要的是`rbx >= rbx+8`，`rbx`存的是地址。所以我们需要从大到小排序输入、

也就是 3 4 5 6 1 2，但是前面我们有一次``a = 7 - a`操作。

答案是 4 3 2 1 6 5

