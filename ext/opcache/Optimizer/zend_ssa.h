/*
   +----------------------------------------------------------------------+
   | Zend Engine, SSA - Static Single Assignment Form                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_SSA_H
#define ZEND_SSA_H

#include "zend_cfg.h"

typedef struct _zend_ssa_range {
	zend_long              min;
	zend_long              max;
	zend_bool              underflow;
	zend_bool              overflow;
} zend_ssa_range;

typedef enum _zend_ssa_negative_lat {
	NEG_NONE      = 0,
	NEG_INIT      = 1,
	NEG_INVARIANT = 2,
	NEG_USE_LT    = 3,
	NEG_USE_GT    = 4,
	NEG_UNKNOWN   = 5
} zend_ssa_negative_lat;

/* Special kind of SSA Phi function used in eSSA */
typedef struct _zend_ssa_pi_range {
	zend_ssa_range         range;       /* simple range constraint */
	int                    min_var;
	int                    max_var;
	int                    min_ssa_var; /* ((min_var>0) ? MIN(ssa_var) : 0) + range.min */
	int                    max_ssa_var; /* ((man_var>0) ? MAX(ssa_var) : 0) + range.man */
	zend_ssa_negative_lat  negative;
} zend_ssa_pi_range;

/* SSA Phi - ssa_var = Phi(source0, source1, ...sourceN) */
typedef struct _zend_ssa_phi zend_ssa_phi;
struct _zend_ssa_phi {
	zend_ssa_phi          *next;          /* next Phi in the same BB */
	int                    pi;            /* if >= 0 this is actually a e-SSA Pi */
	zend_ssa_pi_range      constraint;    /* e-SSA Pi constraint */
	int                    var;           /* Original CV, VAR or TMP variable index */
	int                    ssa_var;       /* SSA variable index */
	int                    block;         /* current BB index */
	int                    visited;       /* flag to avoid recursive processing */
	zend_ssa_phi         **use_chains;
	zend_ssa_phi          *sym_use_chain;
	int                   *sources;       /* Array of SSA IDs that produce this var.
									         As many as this block has
									         predecessors.  */
};

typedef struct _zend_ssa_block {
	zend_ssa_phi          *phis;
} zend_ssa_block;

typedef struct _zend_ssa_op {
	int                    op1_use;
	int                    op2_use;
	int                    result_use;
	int                    op1_def;
	int                    op2_def;
	int                    result_def;
	int                    op1_use_chain;
	int                    op2_use_chain;
	int                    res_use_chain;
} zend_ssa_op;

typedef struct _zend_ssa_var {
	int                    var;            /* original var number; op.var for CVs and following numbers for VARs and TMP_VARs */
	int                    scc;            /* strongly connected component */
	int                    definition;     /* opcode that defines this value */
	zend_ssa_phi          *definition_phi; /* phi that defines this value */
	int                    use_chain;      /* uses of this value, linked through opN_use_chain */
	zend_ssa_phi          *phi_use_chain;  /* uses of this value in Phi, linked through use_chain */
	zend_ssa_phi          *sym_use_chain;  /* uses of this value in Pi constaints */
	unsigned int           no_val : 1;     /* value doesn't mater (used as op1 in ZEND_ASSIGN) */
	unsigned int           scc_entry : 1;
} zend_ssa_var;

typedef struct _zend_ssa {
	int                    vars_count;     /* number of SSA variables        */
	zend_ssa_block        *blocks;         /* array of SSA blocks            */
	zend_ssa_op           *ops;            /* array of SSA instructions      */
	zend_ssa_var          *vars;           /* use/def chain of SSA variables */
} zend_ssa;

BEGIN_EXTERN_C()

int zend_build_ssa(zend_arena **arena, zend_op_array *op_array, zend_cfg *cfg, int rt_constants, zend_ssa *ssa, uint32_t *func_flags);

END_EXTERN_C()

#endif /* ZEND_SSA_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
