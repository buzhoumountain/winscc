// 《自己动手写编译器、链接器》配套源代码

#include "scc.h"
/***********************************************************
 * 功能:	操作数入栈
 * type:	操作数数据类型
 * r:		操作数存储类型
 * value:	操作数值
 **********************************************************/
void operand_push(Type *type, int r, int value)
{

    if (optop >= opstack + (OPSTACK_SIZE - 1))
        error("内存分配失败");
    optop++;
    optop->type = *type;
    optop->r = r;
    optop->value = value;
}

/***********************************************************
 * 功能:	弹出栈顶操作数
 **********************************************************/
void operand_pop()
{
    optop--;
}

/***********************************************************
 * 功能:	交换栈顶两个操作数顺序
 **********************************************************/
void operand_swap()
{
    Operand tmp;

    tmp = optop[0];
    optop[0] = optop[-1];
    optop[-1] = tmp;
}

/***********************************************************
 * 功能:	检查栈顶操作数是否为左值
 **********************************************************/
void check_lvalue()
{
    if (!(optop->r & SC_LVAL))
        expect("左值");
}

/***********************************************************
 * 功能:	操作数赋值
 * t:		操作数数据类型
 * r:		操作数存储类型
 * value:	操作数值
 **********************************************************/
void operand_assign(Operand *opd, int t, int r, int value)
{
    opd->type.t = t;
    opd->r = r;
    opd->value = value;
}

/***********************************************************
 * 功能:取消栈顶操作数的左值属性
 * 亦即得到栈顶操作数地址，因为后面生成代码时通过左值属性来判断是地址还是数据
 **********************************************************/
void cancel_lvalue()
{
    check_lvalue();
    optop->r &= ~SC_LVAL;
}


/***********************************************************
 功能:	间接寻址，这里的 * 号是指针间接寻址运算符(Indirection Operator)
 **********************************************************/
void indirection()
{
    if ((optop->type.t & T_BTYPE) != T_PTR)
    {
        if ((optop->type.t & T_BTYPE) == T_FUNC)
            return;
        expect("指针");
    }
    if ((optop->r & SC_LVAL))
        load_1(REG_ANY, optop);
    optop->type = *pointed_type(&optop->type);

    // 数组与函数不能为左值
    if (!(optop->type.t & T_ARRAY)
        && (optop->type.t & T_BTYPE) != T_FUNC)
    {
        optop->r |= SC_LVAL;
    }
}