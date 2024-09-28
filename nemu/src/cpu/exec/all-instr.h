#include "prefix/prefix.h"

#include "data-mov/mov.h"
#include "data-mov/xchg.h"
#include "data-mov/movext.h"
#include "data-mov/cltd.h"
#include "data-mov/push.h" //finish
#include "data-mov/pop.h"//finsih
#include "data-mov/leave.h"//finish

#include "arith/adc.h"
#include "arith/dec.h"
#include "arith/inc.h"
#include "arith/neg.h"
#include "arith/imul.h"
#include "arith/mul.h"
#include "arith/idiv.h"
#include "arith/div.h"
#include "arith/sbb.h"
#include "arith/sub.h"
#include "arith/cmp.h"//finish as same as add
#include "arith/add.h"//finish but eflags change with 2 methods

#include "control/jmp.h"
#include "control/call.h"//finish
#include "control/ret.h"//finish but still have questions!
#include "control/ja.h"//finish
#include "control/jbe.h"//finish
#include "control/jg.h"//finish
#include "control/jge.h"//finish
#include "control/jl.h"//finish
#include "control/jle.h"//finish
#include "control/je.h"//finish
#include "control/jne.h"//finish
#include "control/js.h"//finish
#include "control/jns.h"//finish
#include "control/je.h"//finish
#include "control/jne.h"//finish

#include "logic/and.h"
#include "logic/or.h"
#include "logic/not.h"
#include "logic/xor.h"
#include "logic/sar.h"
#include "logic/shl.h"
#include "logic/shr.h"
#include "logic/shrd.h"
#include "logic/test.h"//finish
#include "logic/setne.h"//finish

#include "string/rep.h"
#include "string/scas.h"
#include "string/stos.h"
#include "string/movs.h"
#include "string/lods.h"//finish?

#include "misc/misc.h"

#include "special/special.h"

