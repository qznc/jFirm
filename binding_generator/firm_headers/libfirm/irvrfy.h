/*
 * Copyright (C) 1995-2009 University of Karlsruhe.  All right reserved.
 *
 * This file is part of libFirm.
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.
 *
 * Licensees holding valid libFirm Professional Edition licenses may use
 * this file in accordance with the libFirm Commercial License.
 * Agreement provided with the Software.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.
 */

/**
 * @file
 * @brief    Check irnodes for correctness.
 * @author   Christian Schaefer, Goetz Lindenmaier, Till Riedel
 * @version  $Id$
 */
#ifndef FIRM_IR_IRVRFY_H
#define FIRM_IR_IRVRFY_H

#include "firm_types.h"
#include "begin.h"

/**
 * Tests the modes of checknode and its predecessors.
 * checknode must be in current_ir_graph.
 *
 * @return
 * 	NON-zero on success
 */
FIRM_API int irn_vrfy(ir_node *checknode);

/**
 * Tests the modes of checknode and its predecessors.
 * checknode must be in given ir_graph.
 *
 * @return
 * 	NON-zero on success
 */
FIRM_API int irn_vrfy_irg(ir_node *checknode, ir_graph *irg);

/**
 * Same as irn_vrfy_irg, but temporary sets verification mode to
 * NODE_VERIFICATION_ERROR_ONLY.
 * @return
 * 	NON-zero on success
 */
FIRM_API int irn_vrfy_irg_dump(ir_node *checknode, ir_graph *irg, const char **bad_string);

/**
 * Flags for irg_verify().
 */
typedef enum _irg_verify_flags_t {
	VRFY_NORMAL      = 0,      /**< check SSA property only if dominance information is available */
	VRFY_ENFORCE_SSA = 1       /**< check SSA property by enforcing the dominance information recalculation */
} irg_verify_flags_t;

/**
 * Calls irn_vrfy() for each node in irg.
 * Graph must be in state "op_pin_state_pinned".
 *
 * @param irg    the IR-graph t check
 * @param flags  one of irg_verify_flags_t
 *
 * @return
 * 	NON-zero on success.
 */
FIRM_API int irg_verify(ir_graph *irg, unsigned flags);

/**
 * Compatibility macro. Deprecated soon.
 */
#define irg_vrfy(irg) irg_verify(irg, 0)

/**
 * Creates an ir_graph pass for irg_verify().
 *
 * @param name   the name of this pass or NULL
 * @param flags  one of irg_verify_flags_t
 *
 * @return  the newly created ir_graph pass
 */
FIRM_API ir_graph_pass_t *irg_verify_pass(const char *name, unsigned flags);

/**
 * Possible flags for irg_vrfy_bads().
 */
enum verify_bad_flags_t {
	BAD_CF      = 1,    /**< Bad nodes are allowed as predecessors of Blocks and Phis. */
	BAD_DF      = 2,    /**< Bad nodes are allowed as dataflow predecessors. */
	BAD_BLOCK   = 4,    /**< Bad nodes are allowed as Block input. */
	TUPLE       = 8     /**< Tuple nodes are allowed. */
};

/**
 * Verify occurrence of bad nodes in a graph.
 *
 * @param irg    The graph to verify
 * @param flags  combination of verify_bad_flags_t flags describing
 *               which Bads are allowed
 * @returns      a value combined of verify_bad_flags_t indicating the problems found.
 */
FIRM_API int irg_vrfy_bads(ir_graph *irg, int flags);

/**
 *  Enable/disable verification of Load/Store nodes with
 *  its entities. If disabled, Store(SymConst(array)) will be allowed
 *  (C-frontend builds this :-)
 */
FIRM_API void vrfy_enable_entity_tests(int enable);

#include "end.h"

#endif
