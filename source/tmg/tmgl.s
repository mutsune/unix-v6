1 ..1
..1:begin:

ignore
blanks
pr1:
comment
salt;pr1
parse
first
salt;pr2
diag
error
pr2:
comment
salt;pr2
parse
line
salt;pr2
diag
error
salt;pr2
putcharcl
1 parse
last
.3=.
&lt;1 \0&gt;;.even
.2=.
.tx;.3
.tp;.byte 1,0;
1 .tn
.1=.
fref
1 trans;1 .2
.4=.
.tp;.byte 2,0;
.txs;&#39;:
1 .tp;.byte 1,0;
first:

parse
.1
getfref
line
1 trans;1 .4
.1=.
1
.2=.
2
.3=.
1 .txs;&#39;;
.5=.
&lt;??? \0&gt;;.even
.4=.
.tn
.tx;.5
.tp;.byte 2,0;
.tp;.byte 1,0;
1 .tn
error:

smark
ignore
none
any
.1
string
.2
scopy
.pxs;&#39;;
alt;..3
trans;1 .3
goto;..2
..3:null
..2:
1 trans;1 .4
.1=.
.tp;.byte 2,0;
.tn
.tp;.byte 1,0;
1 .tn
line:

labels
charcl
alt;..7
.pxs;&#39;;
goto;..4
..7:statement
salt;..5
numbers
salt;..6
trule
.pxs;&#39;;
..6:
..5:
..4:
1 trans;1 .1
.1=.
.tp;.byte 2,0;
.tn
1 .tp;.byte 1,0;
numbers:

number
.pxs;&#39;;
numbers
alt;done
1 trans;1 .1
.1=.
.tp;.byte 2,0;
.tn
1 .tp;.byte 1,0;
labels:

label
labels
alt;done
1 trans;1 .1
.1=.
.tp;.byte 1,0;
1 .txs;&#39;:
label:

name
.pxs;&#39;:
1 trans;1 .1
.2=.
&lt;.pn:1 .pxs;12\0&gt;;.even
.3=.
&lt;.tn:1 .txs;12\0&gt;;.even
.1=.
.tx;.2
.tn
.tx;.3
1 .tn
last:

1 trans;1 .1
.1=.
&lt;/*\0&gt;;.even
.2=.
4
comment:

.px;.1
co1:
ignore
.2
.pxs;&#39;*
ignore
none
.pxs;&#39;/
1 alt;co1
.1=.
0
.2=.
gpar;1
.tp;.byte 2,0;
1 .tp;.byte 1,0;
.3=.
gpar;1
1 generate
.4=.
gpar;1
.tp;.byte 2,0;nil
.tn
1 .tp;.byte 1,0;q1
.5=.
1 .tp;.byte 1,0;xbit
.7=.
&lt;1 succ\0&gt;;.even
.6=.
.tp;.byte 1,0;nil
.tn
1 .tx;.7
statement:

.l;csym
.l;.1
.st
.p
oldtab
dtt
oldtab
pat
proc
alt;..11
plst
tlst
.pxs;&#39;)
trans;1 .2
goto;..10
..11:trans;1 .3
noelem
..10:
stt1:
bundle
frag
alt;..15
trans;1 .4
salt;stt1
goto;..12
..15:.pxs;&#39;;
ifelem
alt;..14
trans;1 .5
goto;..13
..14:trans;1 .6
..13:
..12:
1 succ
.1=.
&lt;proc(\0&gt;;.even
proc:

smark
ignore
none
1 .px;.1
.1=.
1 octal
npa
.3=.
&lt;params;\0&gt;;.even
.2=.
.tx;.3
.tp;.byte 1,0;
1 .tn
plst:

list
pident
alt;null
remote
.1
1 trans;1 .2
pident:

ident
1 newtab
pat
npa
.1=.
0
.2=.
name
.l;i
.ia
1 .p
.3=.
1 octal
i
.5=.
&lt;push;\0&gt;;.even
.4=.
.tx;.5
.tp;.byte 1,0;
.tn
.tp;.byte 2,0;
1 .tn
tlst:

.pxs;&#39;;
alt;null
.l;i
.l;.1
.st
.p
list
.2
remote
.3
1 trans;1 .4
.1=.
gpar;1
1 .tp;.byte 1,0;nil
q1
.2=.
gpar;1
1 .tp;.byte 1,0;
frag:

prule
alt;..17
trans;1 .1
goto;..16
..17:labels
noelem
trans;1 .2
..16:
1 succ
.2=.
&lt;salt;\0&gt;;.even
.1=.
gpar;2
.tp;.byte 3,0;nil
nil
.tn
.tx;.2
.tp;.byte 2,0;
.tn
.tp;.byte 1,0;q2
q1
.tn
.tp;.byte 2,0;
1 .txs;&#39;:
.5=.
&lt;alt;\0&gt;;.even
.4=.
.tn
.tx;.5
1 .tp;.byte 1,0;
.6=.
&lt;goto;\0&gt;;.even
.3=.
gpar;2
.tp;.byte 4,0;.4
q1
.tn
.tx;.6
.tp;.byte 3,0;
.tn
.tp;.byte 1,0;
.txs;&#39;:
.tp;.byte 2,0;q2
q1
.tn
.tp;.byte 3,0;
1 .txs;&#39;:
prule:

.l;sndt
.l;ndt
.st
.p
disj
.pxs;&#39;|
alt;..23
.l;ndt
.l;sndt
.st
.p
fref
ifeasy
alt;..21
prule
trans;1 .1
goto;..20
..21:prule
fref
trans;1 .3
..20:
noelem
goto;..22
..23:
..22:
1 succ
.1=.
gpar;2
.tp;.byte 2,0;q2
nil
.tn
1 .tp;.byte 1,0;nil
q1
disj:

pelem
pdot
disj
alt;..25
trans;1 .1
ifelem
alt;done
ishard
goto;..24
..25:
..24:
1 succ
.1=.
gpar;2
.tp;.byte 1,0;q1
1 .tq;2
.2=.
1
.3=.
gpar;2
1 generate
pelem:

pprim
alt;..31
trans;1 .1
iseasy
goto;..26
..31:.pxs;&#39;(
push
.2
sndt
prule
alt;..30
.pxs;&#39;)
goto;..27
..30:.pxs;&#39;)
trans;1 .3
noelem
..27:
..26:
1 succ
.1=.
gpar;1
.tq;1
.tp;.byte 2,0;
.tn
1 .tp;.byte 1,0;
.2=.
gpar;1
.tq;1
1 .tp;.byte 1,0;
.4=.
&lt;alt;\0&gt;;.even
.3=.
1 .tx;.4
.5=.
&lt;\\\0&gt;;.even
.7=.
&lt;salt;\0&gt;;.even
.6=.
1 .tx;.7
.10=.
gpar;1
.tp;.byte 3,0;nil
.tn
.tq;1
.tp;.byte 2,0;
1 .tp;.byte 1,0;
pprim:

special
salt;..32
rname
.pxs;&#39;:
alt;..37
fail
goto;..33
..37:spdot
salt;..34

..34:
ignore
none
.pxs;&#39;(
alt;..36
ignore
blanks
list
parg
.pxs;&#39;)
trans;1 .1
goto;..35
..36:trans;1 .2
..35:
..33:
..32:
.pxs;&#39;/
alt;..41
trans;1 .3
goto;..40
..41:.px;.5
alt;..43
trans;1 .6
..40:
rname
trans;1 .10
goto;..42
..43:
..42:
1 succ
.2=.
0
pdot:

.pxs;&#39;.
alt;done
ignore
none
ident
salt;alias
.l;dtt
.t
salt;..44
table
dtt
.l;ndt
.l;.2
.st
.p
..44:
.l;ndt
.ia
1 .p
.1=.
1 any
letter
spdot:

.pxs;&#39;.
ignore
none
not
.1
alias:
1 newtab
dtt
ndt
parg:

rname
salt;..45
remote
specparg
..45:
1 succ
.2=.
&lt;\\n\\0\0&gt;;.even
.1=.
1 .tx;.2
.4=.
&lt;1 succ\0&gt;;.even
.3=.
1 .tx;.4
.5=.
3
.6=.
0
.7=.
1 .tp;.byte 1,0;nil
xbit
.11=.
&lt;1 succ\0&gt;;.even
.10=.
.tp;.byte 1,0;nil
nil
.tn
1 .tx;.11
specparg:

number
salt;..46
charcl
salt;..47
.pxs;&#39;&lt;
alt;..57
longlit
goto;..50
..57:.pxs;&#39;*
alt;..56
trans;1 .1
goto;..51
..56:.pxs;&#39;(
.pxs;&#39;)
alt;..55
trans;1 .3
goto;..52
..55:push
.5
dtt
ndt
sndt
.l;dtt
.l;.6
.st
.p
prule
.pxs;&#39;)
oldtab
dtt
ifelem
alt;..54
trans;1 .7
goto;..53
..54:trans;1 .10
..53:
..52:
..51:
..50:
..47:
..46:
1 succ
.1=.
1
iseasy:

.l;easy
.l;.1
.st
1 .p
.1=.
0
ishard:

.l;easy
.l;.1
.st
1 .p
.1=.
2
noelem:

.l;easy
.l;.1
.st
1 .p
.1=.
2
ifelem:

.l;easy
.l;.1
.ne
1 .t
.2=.
1
ifeasy:

.l;easy
.l;.2
.eq
1 .t
.2=.
&lt;trans;1 \0&gt;;.even
.1=.
gpar;1
.tq;1
.tx;.2
1 .tp;.byte 1,0;
.4=.
&lt;.px\0&gt;;.even
.3=.
gpar;1
.tq;1
.tx;.4
1 .tp;.byte 1,0;
.6=.
&lt;.pn\0&gt;;.even
.5=.
gpar;1
.tq;1
1 .tx;.6
.10=.
&lt;.t\0&gt;;.even
.7=.
1 .tx;.10
.12=.
&lt;.p\0&gt;;.even
.11=.
1 .tx;.12
.13=.
gpar;1
.tp;.byte 2,0;
.tn
.tq;1
1 .tp;.byte 1,0;
special:

.pxs;&#39;=
alt;..70
rname
salt;..60
remote
trule
..60:
trans;1 .1
goto;..61
..70:.pxs;&#39;&lt;
alt;..67
literal
trans;1 .3
goto;..62
..67:.pxs;&#39;*
alt;..66
trans;1 .5
goto;..63
..66:.pxs;&#39;[
expr
.pxs;&#39;?
alt;..65
trans;1 .7
goto;..64
..65:trans;1 .11
..64:
.pxs;&#39;]
trans;1 .13
..63:
..62:
..61:
1 succ
.2=.
&lt;[-\0&gt;;.even
.3=.
&lt;\\&lt;1]\0&gt;;.even
.1=.
.tx;.2
.tp;.byte 1,0;
1 .tx;.3
rname:

name
alt;..72
tabval
pat
npa
alt;done
goto;..71
..72:.pxs;&#39;$
number
..71:
1 trans;1 .1
.2=.
&lt;gpar;\0&gt;;.even
.1=.
.tx;.2
.tp;.byte 2,0;
.tn
1 .tp;.byte 1,0;
trule:

oldtab
ptt
tbody
salt;..73
.pxs;&#39;(
number
salt;..74
tra
..74:
.pxs;&#39;)
tbody
trans;1 .1
..73:
1 succ
tra:

list
tident
1 octal
npt
tident:

ident
1 newtab
ptt
npt
.2=.
&lt;1 generate\0&gt;;.even
.1=.
1 .tx;.2
tbody:

.pxs;&#39;{
.pxs;&#39;}
alt;..76
trans;1 .1
goto;..75
..76:trb
..75:
1 succ
.1=.
xbit
1 .tp;.byte 1,0;
.2=.
.tp;.byte 2,0;
.tn
1 .tp;.byte 1,0;
trb:

telem
.pxs;&#39;}
alt;..100
trans;1 .1
goto;..77
..100:trb
trans;1 .2
..77:
1 succ
.2=.
&lt;.tx\0&gt;;.even
.1=.
.tx;.2
1 .tp;.byte 1,0;
.4=.
&lt;.tn\0&gt;;.even
.3=.
1 .tx;.4
.6=.
&lt;.tq;\0&gt;;.even
.5=.
.tx;.6
1 .tp;.byte 1,0;
telem:

.pxs;&#39;&lt;
alt;..110
literal
trans;1 .1
goto;..101
..110:.pxs;&#39;*
alt;..107
trans;1 .3
goto;..102
..107:.pxs;&#39;$
alt;..106
number
trans;1 .5
goto;..103
..106:number
alt;..105
tdot
trans;1 tpt
goto;..104
..105:name
te1
salt;done
te2
salt;done
..104:
..103:
..102:
..101:
1 succ
te1:

tabval
dtt
ndt
tdot
1 trans;1 tpt
.2=.
&lt;.tq;\0&gt;;.even
.1=.
.tx;.2
1 .tp;.byte 1,0;
te2:

tabval
ptt
npt
1 trans;1 .1
.1=.
1 .txs;&#39;0
.2=.
.tp;.byte 2,0;
.txs;&#39;;
1 .tp;.byte 1,0;
tdot:

.pxs;&#39;.
alt;..112
number
goto;..111
..112:trans;1 .1
..111:
.pxs;&#39;(
alt;..114
list
targ
.pxs;&#39;)
goto;..113
..114:null
..113:
1 trans;1 .2
targ:

name
salt;..115
remote
tbody
..115:
1 succ
.1=.
&lt;.tp;.byte \0&gt;;.even
tpt:
.tx;.1
.tp;.byte 2,0;
.txs;&#39;,
1 .tp;.byte 1,0;
.1=.
.txs;&#39;;
1 .tp;.byte 1,0;
literal:

shortlit
salt;..116
remote
longlit
trans;1 .1
..116:
1 succ
.1=.
&lt;\&gt;\0&gt;;.even
.3=.
&lt;s;&#39;\0&gt;;.even
.2=.
.tx;.3
1 .tp;.byte 1,0;
shortlit:

ignore
none
smark
any
litch
.px;.1
scopy
1 trans;1 .2
.1=.
&lt;\&gt;\0&gt;;.even
.3=.
&lt;\\\0&gt;;.even
.4=.
&lt;\&gt;\0&gt;;.even
.2=.
.tx;.3
1 .tx;.4
.5=.
&lt;\&gt;\0&gt;;.even
.7=.
&lt;\\0\0&gt;;.even
.10=.
&lt;\&gt;;.even\0&gt;;.even
.6=.
.txs;&#39;&lt;
.tp;.byte 2,0;
.tp;.byte 1,0;
.tx;.7
1 .tx;.10
longlit:

ignore
none
.px;.1
alt;..120
trans;1 .2
goto;..117
..120:null
..117:
litb
.px;.5
1 trans;1 .6
.1=.
&lt;\\\0&gt;;.even
.3=.
&lt;\\\\\0&gt;;.even
.2=.
.tp;.byte 2,0;
.tx;.3
1 .tp;.byte 1,0;
litb:

smark
string
litch
scopy
.px;.1
alt;done
litb
1 trans;1 .2
expr:

assignment
salt;..121
rv
..121:
1 succ
.1=.
.tp;.byte 3,0;
.tn
.tp;.byte 1,0;
.tn
1 .tp;.byte 2,0;
assignment:

lv
assign
expr
1 trans;1 .1
.1=.
.tp;.byte 3,0;
.tn
.tp;.byte 1,0;
.tn
1 .tp;.byte 2,0;
.2=.
.tp;.byte 2,0;
.tn
1 .tp;.byte 1,0;
rv:

prime
rv1:
bundle
infix
alt;..125
prime
trans;1 .1
salt;rv1
goto;..122
..125:rva
alt;..124
trans;1 .2
goto;..123
..124:
..123:
..122:
1 succ
.2=.
&lt;.t;alt;\0&gt;;.even
.3=.
&lt;salt;\0&gt;;.even
.1=.
.tx;.2
.tp;.byte 2,0;
.tn
.tp;.byte 4,0;
.tn
.tx;.3
.tp;.byte 1,0;
.tn
.tp;.byte 2,0;
.txs;&#39;:
.tp;.byte 3,0;
.tn
.tp;.byte 1,0;
1 .txs;&#39;:
rva:

.pxs;&#39;?
rv
.pxs;&#39;:
rv
fref
fref
1 trans;1 .1
.1=.
.tp;.byte 2,0;
.tn
1 .tp;.byte 1,0;
.2=.
.tp;.byte 1,0;
.tn
1 .tp;.byte 2,0;
.3=.
.tp;.byte 1,0;
.tn
1 .tp;.byte 2,0;
.5=.
&lt;.l;\0&gt;;.even
.4=.
.tx;.5
1 .tp;.byte 1,0;
prime:

lv
alt;..135
suffix
alt;done
trans;1 .1
goto;..126
..135:prefix
alt;..134
lv
trans;1 .2
goto;..127
..134:.pxs;&#39;(
alt;..133
expr
.pxs;&#39;)
goto;..130
..133:unary
alt;..132
prime
trans;1 .3
goto;..131
..132:remote
number
trans;1 .4
..131:
..130:
..127:
..126:
1 succ
.2=.
&lt;.l;\0&gt;;.even
.1=.
.tx;.2
1 .tp;.byte 1,0;
.4=.
&lt;.rv\0&gt;;.even
.3=.
.tp;.byte 1,0;
.tn
1 .tx;.4
.6=.
&lt;.f\0&gt;;.even
.5=.
.tp;.byte 2,0;
.tn
.tp;.byte 1,0;
.tn
1 .tx;.6
lv:

rname
alt;..141
trans;1 .1
goto;..136
..141:.pxs;&#39;(
alt;..140
lv
.pxs;&#39;)
goto;..137
..140:.pxs;&#39;*
prime
trans;1 .3
..137:
..136:
lv1:
.pxs;&#39;[
alt;done
bundle
expr
.pxs;&#39;]
trans;1 .5
1 salt;lv1
.2=.
&lt;.u\0&gt;;.even
.1=.
.tp;.byte 1,0;
.tn
1 .tx;.2
.4=.
&lt;.st\0&gt;;.even
.3=.
1 .tx;.4
assign:

.pxs;&#39;=
ignore
none
infix
alt;..143
trans;1 .1
goto;..142
..143:trans;1 .3
..142:
1 succ
.2=.
1 .pxs;&#39;+
.1=.
.pxs;&#39;+
1 not
.2
.4=.
&lt;.a\0&gt;;.even
.3=.
1 .tx;.4
.6=.
&lt;.s\0&gt;;.even
.5=.
1 .tx;.6
.10=.
&lt;.m\0&gt;;.even
.7=.
1 .tx;.10
.12=.
&lt;.q\0&gt;;.even
.11=.
1 .tx;.12
.14=.
&lt;.r\0&gt;;.even
.13=.
1 .tx;.14
.16=.
&lt;.o\0&gt;;.even
.15=.
1 .tx;.16
.20=.
&lt;.x\0&gt;;.even
.17=.
1 .tx;.20
.22=.
&lt;.n\0&gt;;.even
.21=.
1 .tx;.22
.23=.
&lt;==\0&gt;;.even
.25=.
&lt;.eq\0&gt;;.even
.24=.
1 .tx;.25
.26=.
&lt;!=\0&gt;;.even
.30=.
&lt;.ne\0&gt;;.even
.27=.
1 .tx;.30
.31=.
&lt;&lt;=\0&gt;;.even
.33=.
&lt;.le\0&gt;;.even
.32=.
1 .tx;.33
.34=.
&lt;\&gt;=\0&gt;;.even
.36=.
&lt;.ge\0&gt;;.even
.35=.
1 .tx;.36
.37=.
&lt;&lt;&lt;\0&gt;;.even
.41=.
&lt;.sl\0&gt;;.even
.40=.
1 .tx;.41
.43=.
&lt;.lt\0&gt;;.even
.42=.
1 .tx;.43
.44=.
&lt;\&gt;\0&gt;;.even
.45=.
&lt;\&gt;\0&gt;;.even
.47=.
&lt;.sr\0&gt;;.even
.46=.
1 .tx;.47
.51=.
&lt;.gt\0&gt;;.even
.50=.
1 .tx;.51
infix:

smark
ignore
none
.pxs;&#39;+
alt;..201
not
.1
trans;1 .3
goto;..144
..201:.pxs;&#39;-
alt;..200
trans;1 .5
goto;..145
..200:.pxs;&#39;*
alt;..177
trans;1 .7
goto;..146
..177:.pxs;&#39;/
alt;..176
trans;1 .11
goto;..147
..176:.pxs;&#39;%
alt;..175
trans;1 .13
goto;..150
..175:.pxs;&#39;|
alt;..174
trans;1 .15
goto;..151
..174:.pxs;&#39;^
alt;..173
trans;1 .17
goto;..152
..173:.pxs;&#39;&amp;
alt;..172
trans;1 .21
goto;..153
..172:.px;.23
alt;..171
trans;1 .24
goto;..154
..171:.px;.26
alt;..170
trans;1 .27
goto;..155
..170:.px;.31
alt;..167
trans;1 .32
goto;..156
..167:.px;.34
alt;..166
trans;1 .35
goto;..157
..166:.px;.37
alt;..165
trans;1 .40
goto;..160
..165:.pxs;&#39;&lt;
alt;..164
trans;1 .42
goto;..161
..164:.px;.44
.px;.45
alt;..163
trans;1 .46
goto;..162
..163:trans;1 .50
..162:
..161:
..160:
..157:
..156:
..155:
..154:
..153:
..152:
..151:
..150:
..147:
..146:
..145:
..144:
1 succ
.2=.
&lt;.lv\0&gt;;.even
.1=.
1 .tx;.2
.3=.
&lt;++\0&gt;;.even
.5=.
&lt;.ib\0&gt;;.even
.4=.
1 .tx;.5
.6=.
&lt;--\0&gt;;.even
.10=.
&lt;.db\0&gt;;.even
.7=.
1 .tx;.10
prefix:

smark
ignore
none
.pxs;&#39;&amp;
alt;..205
trans;1 .1
goto;..202
..205:.px;.3
alt;..204
trans;1 .4
goto;..203
..204:.px;.6
trans;1 .7
..203:
..202:
1 succ
.1=.
&lt;++\0&gt;;.even
.3=.
&lt;.ia\0&gt;;.even
.2=.
1 .tx;.3
.4=.
&lt;--\0&gt;;.even
.6=.
&lt;.da\0&gt;;.even
.5=.
1 .tx;.6
suffix:

smark
ignore
none
.px;.1
alt;..207
trans;1 .2
goto;..206
..207:.px;.4
trans;1 .5
..206:
1 succ
.2=.
&lt;.nt\0&gt;;.even
.1=.
1 .tx;.2
.4=.
&lt;.ng\0&gt;;.even
.3=.
1 .tx;.4
.6=.
&lt;.cm\0&gt;;.even
.5=.
1 .tx;.6
unary:

.pxs;&#39;!
alt;..213
trans;1 .1
goto;..210
..213:.pxs;&#39;-
alt;..212
trans;1 .3
goto;..211
..212:.pxs;&#39;~
trans;1 .5
..211:
..210:
1 succ
charcl:

.pxs;&#39;!
alt;..215
ccla
cclb
goto;..214
..215:ccla
..214:
1 octal
classmask
.1=.
&lt;&lt;&lt;\0&gt;;.even
.2=.
1
.4=.
&lt;&lt;&lt;\0&gt;;.even
ccla:

.px;.1
.l;classmask
.l;.2
.l;nclass
.ia
.sl
.st
.p
.l;classmask
.t
alt;cherr
ccl1:
cclc
.px;.4
1 salt;ccl1
.1=.
&lt;\&gt;\0&gt;;.even
cclc:

ignore
none
ccl3:
.px;.1
salt;ccl4
ccle
1 salt;ccl3
.1=.
&lt;\&gt;\0&gt;;.even
.3=.
&lt;\&gt;\0&gt;;.even
.2=.
1 .px;.3
ccl4:

.px;.1
salt;..216
cclx
fail
..216:
not
.2
salt;..217
ccle
..217:
1 succ
.1=.
2
ccle:

char
n
.l;.1
.l;n
.m
.l;classes
.lv
.a
.rv
.l;classmask
.o
.u
1 .p
cclb:

zeron
ccl5:
.l;classes
.lv
.l;n
.a
.rv
.l;classmask
.x
.u
.p
testn
1 salt;ccl5
cclx:

.l;nclass
.da
.p
zeron
ccl6:
.l;classes
.lv
.l;n
.a
.rv
.l;classmask
.cm
.n
.u
.p
testn
1 salt;ccl6
.3=.
&lt;too many char classes\0&gt;;.even
.2=.
1 .tx;.3
.1=.
1 trans;1 .2
cherr:

1 diag
.1
.1=.
0
zeron:

.l;n
.l;.1
.st
1 .p
.1=.
2
.2=.
400
testn:

.l;n
.l;.1
.a
.u
.l;.2
.lt
1 .t
.1=.
0
.4=.
&lt;.globl classtab\0&gt;;.even
.5=.
&lt;classtab:\0&gt;;.even
.3=.
.tn
.tx;.4
.tn
.tx;.5
1 .tn
.2=.
1 trans;1 .3
.7=.
.tp;.byte 1,0;
1 .tn
.6=.
octal
w
1 trans;1 .7
putcharcl:

zeron
.l;classes
.l;.1
.st
.p
parse
.2
ptc1:
.l;w
.l;classes
.lv
.l;n
.a
.rv
.st
.p
parse
.6
bundle
testn
1 salt;ptc1
classmask:
0
nclass:
0
classes:
cl1:
0
0
0
0
0
0
0
0
0
0
0
0
0
0
0
0
cl2:
0
0
0
0
0
0
0
0
0
0
0
0
0
0
0
0
cl3:
0
0
0
0
0
0
0
0
0
0
0
0
0
0
0
0
cl4:
0
0
0
0
0
0
0
0
0
0
0
0
0
0
0
0
cl5:
0
0
0
0
0
0
0
0
0
0
0
0
0
0
0
0
cl6:
0
0
0
0
0
0
0
0
0
0
0
0
0
0
0
0
cl7:
0
0
0
0
0
0
0
0
0
0
0
0
0
0
0
0
cl8:
0
0
0
0
0
0
0
0
0
0
0
0
0
0
0
0
done:

1 succ
.1=.
.txs;&#39;.
1 .tp;.byte 1,0;
create:

.l;csym
.ia
.p
getcsym:
octal
csym
1 trans;1 .1
.2=.
&lt;..\0&gt;;.even
.1=.
.tx;.2
1 .tp;.byte 1,0;
fref:

.l;fsym
.ia
.p
getfref:
octal
fsym
1 trans;1 .1
.1=.
1
not:

params
.1
[-1\&lt;1]
alt;done
1 fail
.1=.
1
.2=.
.tp;.byte 2,0;
.tn
1 .tp;.byte 1,0;
list:

params
.1
[-1\&lt;1]
list1:
bundle
.pxs;&#39;,
alt;done
[-1\&lt;1]
trans;1 .2
1 salt;list1
.1=.
1
remote:

params
.1
create
1 parse
rem1
[-1\&lt;1]
.1=.
1
.3=.
&lt;=.\0&gt;;.even
.2=.
.tp;.byte 2,0;
.tx;.3
.tn
.tp;.byte 1,0;
1 .tn
rem1:

params
.1
getcsym
[-1\&lt;1]
1 trans;1 .2
number:

smark
ignore
none
any
digit
string
digit
1 scopy
name:

ident
1 scopy
ident:

smark
ignore
none
any
letter
1 string
alpha
.1=.
1
.3=.
0
oldtab:

params
.1
.l;[-1\&lt;1]
.t
alt;done
discard
[-1\&lt;1]
.l;[-1\&lt;1]
.l;.3
.st
1 .p
.1=.
2
.3=.
0
newtab:

params
.1
.l;[-2\&lt;1]
.t
salt;..220
table
[-2\&lt;1]
.l;[-1\&lt;1]
.l;.3
.st
.p
..220:
enter
[-2\&lt;1]
i
.l;[-2\&lt;1]
.l;i
.f
.l;[-1\&lt;1]
.ia
.st
1 .p
.1=.
2
tabval:

params
.1
.l;[-2\&lt;1]
.t
find
[-2\&lt;1]
i
.l;i
.l;[-1\&lt;1]
.l;[-2\&lt;1]
.l;i
.f
.s
.st
.p
1 octal
i
null:

1 trans;1 nil
.1=.
&lt;1 \0&gt;;.even
xbit:
1 .tx;.1
q1:
1 .tq;1
q2:
1 .tq;2
nil:
1 generate
blanks:
10
digit:
20
letter:
40
alpha:
100
litch:
200
none:
400
csym:
0
fsym:
0
easy:
0
w:
0
n:
0
dtt:
0
ndt:
0
sndt:
0
pat:
0
npa:
0
ptt:
0
npt:
0
i:
0

.globl classtab
classtab:
0
207
207
207
207
207
207
207
207
217
217
207
207
207
207
207
207
207
207
207
207
207
207
207
207
207
207
207
207
207
207
207
217
207
207
207
207
207
207
207
207
207
203
207
207
207
207
207
327
327
327
327
327
327
327
327
327
327
207
205
207
207
7
207
207
347
347
347
347
347
347
347
347
347
347
347
347
347
347
347
347
347
347
347
347
347
347
347
347
347
347
207
7
207
207
207
207
347
347
347
347
347
347
347
347
347
347
347
347
347
347
347
347
347
347
347
347
347
347
347
347
347
347
207
207
207
207
207
.pn:1 .pxs;12
.tn:1 .txs;12
