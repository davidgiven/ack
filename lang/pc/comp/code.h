/*  Copyright (c) 2019 ACK Project.
 *  See the copyright notice in the ACK home directory,
 *  in the file "Copyright".
 *
 */
#ifndef CODE_H_
#define CODE_H_

#include "em_arith.h"
#include "em_label.h"

struct def;
struct node;
struct type;
struct desig;

void routine_label(struct def * df);
void RomString(struct node *nd);
void RomReal(struct node *nd);
void BssVar(void);
arith CodeBeginBlock(struct def *df);
void CodeEndBlock(struct def *df, arith StackAdjustment);
void CodeExpr(struct node *nd, struct desig *ds,
		label true_label);
void CodeCall(struct node *nd);
void RangeCheck(struct type *tpl, struct type *tpr);

/*	Generate code to push the value of the expression "nd"
    on the stack.
*/
void CodePExpr(struct node *nd);

/*	Generate code to push the address of the designator "nd"
    on the stack.
 */
void CodeDAddress(struct node *nd);

/*	Generate code to store the expression on the stack
    into the designator "nd".
 */
void CodeDStore(struct node *nd);

/* Generate code to convert long to int */
void Long2Int(void);
/* Generate code to convert int to long */
void Int2Long(void);
/* Generate code to convert int to real */
void Int2Real(arith size);
/* Generate code to convert real to int */
void Real2Int(void);





#endif /* CODE_H_ */
