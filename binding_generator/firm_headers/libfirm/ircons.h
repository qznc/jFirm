/*
 * Copyright (C) 1995-2010 University of Karlsruhe.  All right reserved.
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
 * @brief   Various irnode constructors. Automatic construction of SSA
 *          representation.
 * @author  Martin Trapp, Christian Schaefer, Goetz Lindenmaier, Boris Boesler,
 *          Michael Beck
 * @version $Id: ircons.h 28097 2010-10-11 16:52:36Z matze $
 */

/**
 *  @file
 *
 *  documentation no more supported since 2001
 *
 *  IR node construction.
 *
 *    This file documents all datatypes and constructors needed to
 *    build a FIRM representation of a procedure.  The constructors are
 *    also implemented in this file.
 *
 *    The documentation also gives a short manual how to use the library.
 *
 *    For extensive documentation of FIRM see UKA Techreport 1999-14.
 *
 *
 *    Three kinds of nodes
 *    --------------------
 *
 *      There are three kinds of nodes known to the IR:  entities,
 *      types, and ir_nodes
 *
 *      + ir_nodes are the actual nodes of the FIRM intermediate representation.
 *        They represent operations on the data of the program and control flow
 *        operations.
 *
 *      + entity ==> implemented in entity.h
 *        Refers to a single entity of the compiled program, e.g. a field of a
 *        class or a method.  If a method or variable can not be assigned to
 *        a method or class or the like, it is a global object.
 *
 *      + types ==> implemented in type.h
 *        With types type information is represented.  There are several type
 *       nodes.
 *
 *    Implementation of the FIRM operations: ir_node
 *    ----------------------------------------------
 *
 *      Ir_nodes represent operations on the data of the program and control flow
 *      operations.  Examples of ir_nodes:  Add, Jmp, Cmp
 *
 *      FIRM is a dataflow graph.  A dataflow graph is a directed graph,
 *      so that every node has incoming and outgoing edges.  A node is
 *      executable if every input at its incoming edges is available.
 *      Execution of the dataflow graph is started at the Start node which
 *      has no incoming edges and ends when the End node executes, even if
 *      there are still executable or not executed nodes.  (Is this true,
 *      or must all executable nodes be executed?)  (There are exceptions
 *      to the dataflow paradigma that all inputs have to be available
 *      before a node can execute: Phi, Block.  See UKA Techreport
 *      1999-14.)
 *
 *      The implementation of FIRM differs from the view as a dataflow
 *      graph.  To allow fast traversion of the graph edges are
 *      implemented as C-pointers.  Inputs to nodes are not ambiguous, the
 *      results can be used by several other nodes.  Each input can be
 *      implemented as a single pointer to a predecessor node, outputs
 *      need to be lists of pointers to successors.  Therefore a node
 *      contains pointers to its predecessors so that the implementation is a
 *      dataflow graph with reversed edges.  It has to be traversed bottom
 *      up.
 *
 *      All nodes of the IR have the same basic structure.  They are
 *      distinguished by a field containing the opcode.
 *
 *      The fields of an ir_node:
 *
 *      kind             A firm_kind tag containing k_ir_node.  This is useful for
 *                       dynamically checking the type of a node.
 *
 *      *op              This ir_op gives the opcode as a tag and a string
 *                       and the number of attributes of an ir_node.  There is
 *                       one statically allocated struct ir_op for each opcode.
 *
 *      *mode            The ir_mode of the operation represented by this firm
 *                       node.  The mode of the operation is the mode of its
 *                       result.  A Firm mode is a datatype as known to the
 *                       target, not a type of the source language.
 *
 *      visit            A flag for traversing the IR.
 *
 *      **in             An array with pointers to the node's predecessors.
 *
 *      *link            A pointer to an ir_node.  With this pointer all Phi nodes
 *                       are attached to a Block, i.e. a Block points to its
 *                       first Phi node, this node points to the second Phi node
 *                       in the Block and so forth.  Used in mature_immBlock
 *                       to find all Phi nodes to be matured.  It's also used to
 *                       annotate a node with a better, optimized version of it.
 *
 *      attr             An attr struct containing the attributes of the nodes. The
 *                       attributes depend on the opcode of the node.  The number
 *                       of these attributes is given in op.
 *
 *    The struct ir_op
 *    ----------------
 *                       Not yet documented. See irop.h.
 *
 *    The struct ir_mode
 *    ------------------
 *                       Not yet documented. See irmode.h.
 *
 *    GLOBAL VARIABLES -- now also fields of ir_graph.
 *    ================
 *
 *    current_ir_graph   Points to the current ir_graph.  All constructors for
 *                       nodes add nodes to this graph.
 *
 *    ir_visited         An int used as flag to traverse the ir_graph.
 *
 *    block_visited      An int used as a flag to traverse block nodes in the
 *                       graph.
 *
 *                       Others not yet documented.
 *
 *
 *
 *    CONSTRUCTOR FOR IR_GRAPH --> see irgraph.h
 *    ========================
 *
 *
 *    PROCEDURE TO CONSTRUCT AN IR GRAPH --> see also Firm tutorial
 *    ==================================
 *
 *    This library supplies several interfaces to construct a FIRM graph for
 *    a program:
 *    - A "comfortable" interface generating SSA automatically.  Automatically
 *      computed predecessors of nodes need not be specified in the constructors.
 *      (new_<Node> constructurs and a set of additional routines.)
 *    - A less comfortable interface where all predecessors except the block
 *      an operation belongs to need to be specified.  SSA must be constructed
 *      by hand.  (new_<Node> constructors and set_cur_block()).  This interface
 *      is called "block oriented".  It automatically calles the local optimizations
 *      for each new node.
 *    - An even less comfortable interface where the block needs to be specified
 *      explicitly.  This is called the "raw" interface. (new_r_<Node>
 *      constructors).  These nodes are not optimized.
 *
 *    To use the functionality of the comfortable interface correctly the Front
 *    End needs to follow certain protocols.  This is explained in the following.
 *    To build a correct IR with the other interfaces study the semantics of
 *    the firm node (See tech-reprot UKA 1999-14).  For the construction of
 *    types and entities see the documentation in those modules.
 *
 *    First the Frontend needs to decide which variables and values used in
 *    a procedure can be represented by dataflow edges.  These are variables
 *    that need not be saved to memory as they cause no side effects visible
 *    out of the procedure.  Often these are all compiler generated
 *    variables and simple local variables of the procedure as integers,
 *    reals and pointers.  The frontend has to count and number these variables.
 *
 *    First an ir_graph needs to be constructed with new_ir_graph.  The
 *    constructor gets the number of local variables.  The graph is held in the
 *    global variable irg.
 *
 *    Now the construction of the procedure can start.  Several basic blocks can
 *    be constructed in parallel, but the code within each block needs to
 *    be constructed (almost) in program order.
 *
 *    A global variable holds the current basic block.  All (non block) nodes
 *    generated are added to this block.  The current block can be set with
 *    set_cur_block(block).  If several blocks are constructed in parallel block
 *    switches need to be performed constantly.
 *
 *    To generate a Block node (with the comfortable interface), its predecessor
 *    control flow nodes need not be known.  In case of cyclic control flow these
 *    can not be known when the block is constructed.  With add_immBlock_pred(block,
 *    cfnode) predecessors can be added to the block.  If all predecessors are
 *    added to the block mature_immBlock(b) needs to be called.  Calling mature_immBlock
 *    early improves the efficiency of the Phi node construction algorithm.
 *    But if several  blocks are constructed at once, mature_immBlock must only
 *    be called after performing all set_values and set_stores in the block!
 *    (See documentation of new_immBlock constructor.)
 *
 *    The constructors of arithmetic nodes require that their predecessors
 *    are mentioned.  Sometimes these are available in the Frontend as the
 *    predecessors have just been generated by the frontend.  If they are local
 *    values, the predecessors can be obtained from the library with a call to
 *    get_value(local_val_nr).  (local_val_nr needs to be administered by
 *    the Frontend.)  A call to get_value triggers the generation of Phi nodes.
 *    If an arithmetic operation produces a local value, this value needs to be
 *    passed to the library by set_value(node, local_val_nr).
 *    In straight line code these two operations just remember and return the
 *    pointer to nodes producing the value.  If the value passes block boundaries
 *    Phi nodes can be inserted.
 *    Similar routines exist to manage the Memory operands: set_store and
 *    get_store.
 *
 *    Several nodes produce more than one result.  An example is the Div node.
 *    Such nodes return tuples of values.  From these individual values can be
 *    extracted by proj nodes.
 *
 *    The following example illustrates the construction of a simple basic block
 *    with two predecessors stored in variables cf_pred1 and cf_pred2, containing
 *    the code
 *      a = a div a;
 *    and finally jumping to an other block.  The variable a got the local_val_nr
 *    42 by the frontend.
 *
 *    ir_node *this_block, *cf_pred1, *cf_pred2, *a_val, *mem, *div, *res, *cf_op;
 *
 *    this_block = new_immBlock();
 *    add_immBlock_pred(this_block, cf_pred1);
 *    add_immBlock_pred(this_block, cf_pred2);
 *    mature_immBlock(this_block);
 *    a_val = get_value(42, mode_Iu);
 *    mem = get_store();
 *    div = new_Div(mem, a_val, a_val, mode_Iu);
 *    mem = new_Proj(div, mode_M, pn_Div_M);   * for the numbers for Proj see docu *
 *    res = new_Proj(div, mode_Iu, pn_Div_res);
 *    set_store(mem);
 *    set_value(res, 42);
 *    cf_op = new_Jmp();
 *
 *    For further information look at the documentation of the nodes and
 *    constructors and at the paragraph COPING WITH DATA OBJECTS at the
 *    end of this documentation.
 *
 *    The comfortable interface contains the following routines further explained
 *    below:
 *
 *    ir_node *new_immBlock (void);
 *    ir_node *new_Start    (void);
 *    ir_node *new_End      (void);
 *    ir_node *new_Jmp      (void);
 *    ir_node *new_IJmp     (ir_node *tgt);
 *    ir_node *new_Cond     (ir_node *c);
 *    ir_node *new_Return   (ir_node *store, int arity, ir_node **in);
 *    ir_node *new_Const    (ir_tarval *con);
 *    ir_node *new_SymConst (ir_mode *mode, symconst_symbol value, symconst_kind kind);
 *    ir_node *new_simpleSel (ir_node *store, ir_node *objptr, ir_entity *ent);
 *    ir_node *new_Sel    (ir_node *store, ir_node *objptr, int arity,
 *                         ir_node **in, ir_entity *ent);
 *    ir_node *new_Call   (ir_node *store, ir_node *callee, int arity,
 *                         ir_node **in, type_method *type);
 *    ir_node *new_Builtin(ir_node *store, ir_builtin_kind kind, int arity,
 *                         ir_node **in, type_method *type);
 *    ir_node *new_Add    (ir_node *op1, ir_node *op2, ir_mode *mode);
 *    ir_node *new_Sub    (ir_node *op1, ir_node *op2, ir_mode *mode);
 *    ir_node *new_Minus  (ir_node *op,  ir_mode *mode);
 *    ir_node *new_Mul    (ir_node *op1, ir_node *op2, ir_mode *mode);
 *    ir_node *new_Mulh   (ir_node *op1, ir_node *op2, ir_mode *mode);
 *    ir_node *new_Quot   (ir_node *memop, ir_node *op1, ir_node *op2, ir_mode *mode, op_pin_state state);
 *    ir_node *new_DivMod (ir_node *memop, ir_node *op1, ir_node *op2, ir_mode *mode, op_pin_state state);
 *    ir_node *new_Div    (ir_node *memop, ir_node *op1, ir_node *op2, ir_mode *mode, op_pin_state state);
 *    ir_node *new_Mod    (ir_node *memop, ir_node *op1, ir_node *op2, ir_mode *mode, op_pin_state state;
 *    ir_node *new_And    (ir_node *op1, ir_node *op2, ir_mode *mode);
 *    ir_node *new_Or     (ir_node *op1, ir_node *op2, ir_mode *mode);
 *    ir_node *new_Eor    (ir_node *op1, ir_node *op2, ir_mode *mode);
 *    ir_node *new_Not    (ir_node *op,                ir_mode *mode);
 *    ir_node *new_Shl    (ir_node *op,  ir_node *k,   ir_mode *mode);
 *    ir_node *new_Shr    (ir_node *op,  ir_node *k,   ir_mode *mode);
 *    ir_node *new_Shrs   (ir_node *op,  ir_node *k,   ir_mode *mode);
 *    ir_node *new_Rotl   (ir_node *op,  ir_node *k,   ir_mode *mode);
 *    ir_node *new_Cmp    (ir_node *op1, ir_node *op2);
 *    ir_node *new_Conv   (ir_node *op, ir_mode *mode);
 *    ir_node *new_Cast   (ir_node *op, ir_type *to_tp);
 *    ir_node *new_Carry  (ir_node *op1, ir_node *op2, ir_mode *mode);
 *    ir_node *new_Borrow (ir_node *op1, ir_node *op2, ir_mode *mode);
 *    ir_node *new_Load   (ir_node *store, ir_node *addr, ir_mode *mode, ir_cons_flags flags);
 *    ir_node *new_Store  (ir_node *store, ir_node *addr, ir_node *val, ir_cons_flags flags);
 *    ir_node *new_Alloc  (ir_node *store, ir_node *count, ir_type *alloc_type,
 *                         where_alloc where);
 *    ir_node *new_Free   (ir_node *store, ir_node *ptr, ir_node *size,
 *               ir_type *free_type, where_alloc where);
 *    ir_node *new_Proj   (ir_node *arg, ir_mode *mode, long proj);
 *    ir_node *new_NoMem  (void);
 *    ir_node *new_Mux    (ir_node *sel, ir_node *ir_false, ir_node *ir_true, ir_mode *mode);
 *    ir_node *new_CopyB  (ir_node *store, ir_node *dst, ir_node *src, ir_type *data_type);
 *    ir_node *new_InstOf (ir_node *store, ir_node obj, ir_type *ent);
 *    ir_node *new_Raise  (ir_node *store, ir_node *obj);
 *    ir_node *new_Bound  (ir_node *store, ir_node *idx, ir_node *lower, ir_node *upper);
 *    ir_node *new_Pin    (ir_node *node);
 *
 *    void add_immBlock_pred (ir_node *block, ir_node *jmp);
 *    void mature_immBlock (ir_node *block);
 *    void set_cur_block (ir_node *target);
 *    ir_node *get_value (int pos, ir_mode *mode);
 *    void set_value (int pos, ir_node *value);
 *    ir_node *get_store (void);
 *    void set_store (ir_node *store);
 *    keep_alive (ir_node ka)
 *
 *    IR_NODES AND CONSTRUCTORS FOR IR_NODES
 *    =======================================
 *
 *    All ir_nodes are defined by a common data structure.  They are distinguished
 *    by their opcode and differ in the number of their attributes.
 *
 *    Const nodes are always added to the start block.
 *    All other constructors add the created node to the current_block.
 *    swich_block(block) allows to set the current block to block.
 *
 *    Watch for my inconsistent use of input and predecessor (dataflow view)
 *    and `the node points to' (implementation view).
 *
 *    The following description of the nodes lists four properties them if these
 *    are of interest:
 *     - the parameters to the constructor
 *     - the inputs of the Firm node
 *     - the outputs of the Firm node
 *     - attributes to the node
 *
 *    ------------
 *
 *    ir_node *new_immBlock (void)
 *    ----------------------------
 *
 *    Creates a new block. When a new block is created it cannot be known how
 *    many predecessors this block will have in the control flow graph.
 *    Therefore the list of inputs can not be fixed at creation.  Predecessors
 *    can be added with add_immBlock_pred (block, control flow operation).
 *    With every added predecessor the number of inputs to Phi nodes also
 *    changes.
 *
 *    The block can be completed by mature_immBlock(block) if all predecessors are
 *    known.  If several blocks are built at once, mature_immBlock can only be called
 *    after set_value has been called for all values that are life at the end
 *    of the block.  This is necessary so that Phi nodes created mature_immBlock
 *    get the right predecessors in case of cyclic dependencies.  If all set_values
 *    of this block are called after maturing it and before calling get_value
 *    in some block that is control flow dependent on this block, the construction
 *    is correct.
 *
 *    Example for faulty IR construction:  (draw the graph on a paper and you'll
 *                                          get it ;-)
 *
 *      block_before_loop = new_immBlock();
 *      set_cur_block(block_before_loop);
 *      set_value(x);
 *      mature_immBlock(block_before_loop);
 *      before2header = new_Jmp;
 *
 *      loop_header = new_immBlock ();
 *      set_cur_block(loop_header);
 *      header2body - new_Jmp();
 *
 *      loop_body = new_immBlock ();
 *      set_cur_block(loop_body);
 *      body2header = new_Jmp();
 *
 *      add_immBlock_pred(loop_header, before2header);
 *      add_immBlock_pred(loop_header, body2header);
 *      add_immBlock_pred(loop_body, header2body);
 *
 *      mature_immBlock(loop_header);
 *      mature_immBlock(loop_body);
 *
 *      get_value(loop_body, x);   //  gets the Phi in loop_header
 *      set_value(loop_header, x); //  sets the value the above get_value should
 *                                 //  have returned!!!
 *
 *    Mature_immBlock also fixes the number of inputs to the Phi nodes.  Mature_immBlock
 *    should be called as early as possible, as afterwards the generation of Phi
 *    nodes is more efficient.
 *
 *    Inputs:
 *      There is an input for each control flow predecessor of the block.
 *      The input points to an instruction producing an output of type X.
 *      Possible predecessors:  Start, Jmp, Cond, Raise or Return or any node
 *      possibly causing an exception.  (Often the real predecessors are Projs.)
 *    Output:
 *      Mode BB (R), all nodes belonging to this block should consume this output.
 *      As they are strict (except Block and Phi node) it is a necessary condition
 *      that the block node executed before any other node in this block executes.
 *    Attributes:
 *      block.matured  Indicates whether the block is mature.
 *      block.**graph_arr
 *                      This attribute contains all local values valid in this
 *                      block. This is needed to build the Phi nodes and removed
 *                      if the graph is complete.  This field is used by the
 *              internal construction algorithm and should not be accessed
 *              from outside.
 *
 *
 *    ir_node *new_Block (int arity, ir_node **in)
 *    --------------------------------------------
 *
 *    Creates a new Block with the given list of predecessors.  This block
 *    is mature.  As other constructors calls optimization and verify for the
 *    block.  If one of the predecessors is Unknown (as it has to be filled in
 *    later) optimizations are skipped.  This is necessary to
 *    construct Blocks in loops.
 *
 *
 *    CONTROL FLOW OPERATIONS
 *    -----------------------
 *
 *    In each block there must be exactly one of the control flow
 *    operations Start, End, Jmp, Cond, Return or Raise.  The output of a
 *    control flow operation points to the block to be executed next.
 *
 *    ir_node *new_Start (void)
 *    -------------------------
 *
 *    Creates a start node.  Not actually needed public.  There is only one such
 *   node in each procedure which is automatically created by new_ir_graph.
 *
 *    Inputs:
 *      No inputs except the block it belongs to.
 *    Output:
 *      A tuple of 4 (5, 6) distinct values. These are labeled by the following
 *      projection numbers (pn_Start):
 *      * pn_Start_X_initial_exec    mode X, points to the first block to be exe *                                   cuted.
 *      * pn_Start_M                 mode M, the global store
 *      * pn_Start_P_frame_base      mode P, a pointer to the base of the proce  *                                   dures stack frame.
 *      * pn_Start_P_globals         mode P, a pointer to the part of the memory *                                   containing_all_ global things.
 *      * pn_Start_T_args            mode T, a tuple containing all arguments of *                                   the procedure.
 *
 *
 *    ir_node *new_End (void)
 *    -----------------------
 *
 *    Creates an end node.  Not actually needed public.  There is only one such
 *   node in each procedure which is automatically created by new_ir_graph.
 *
 *    Inputs:
 *      No inputs except the block it belongs to.
 *    Output:
 *      No output.
 *
 *    ir_node *new_Jmp (void)
 *    -----------------------
 *
 *    Creates a Jmp node.
 *
 *    Inputs:
 *      The block the node belongs to
 *    Output:
 *      Control flow to the next block.
 *
 *    ir_node *new_IJmp (ir_node *tgt)
 *    -----------------------
 *
 *    Creates an IJmp node.
 *
 *    Inputs:
 *      The node that represents the target jump address
 *    Output:
 *      Control flow to an unknown target, must be pinned by
 *      the End node.
 *
 *    ir_node *new_Cond (ir_node *c)
 *    ------------------------------
 *
 *    Creates a Cond node.  There are two versions of this node.
 *
 *    The Boolean Cond:
 *    Input:
 *      A value of mode b.
 *    Output:
 *      A tuple of two control flows.  The first is taken if the input is
 *      false, the second if it is true.
 *
 *    The Switch Cond:
 *    Input:
 *      A value of mode I_u. (i)
 *    Output:
 *      A tuple of n control flows.  If the Cond's input is i, control
 *      flow will proceed along output i. If the input is >= n control
 *      flow proceeds along output n.
 *
 *    ir_node *new_Return (ir_node *store, int arity, ir_node **in)
 *    -------------------------------------------------------------
 *
 *    The Return node has as inputs the results of the procedure.  It
 *    passes the control flow to the end_block.
 *
 *    Inputs:
 *      The memory state.
 *      All results.
 *    Output
 *      Control flow to the end block.
 *
 *
 *    ir_node *new_Const (ir_tarval *con)
 *    -----------------------------------------------
 *
 *    Creates a constant in the constant table and adds a Const node
 *    returning this value to the start block. The mode is derived
 *    from the tarval.
 *
 *    Parameters:
 *      *con             Points to an entry in the constant table.
 *                       This pointer is added to the attributes of
 *                       the node (self->attr.con)
 *    Inputs:
 *      No inputs except the block it belogns to.
 *    Output:
 *      The constant value.
 *    Attribute:
 *      attr.con   A tarval* pointer to the proper entry in the constant
 *                 table.
 *
 *    ir_node *new_SymConst (ir_mode *mode, union symconst_symbol value, symconst_addr_ent kind)
 *    -----------------------------------------------------------------------------------------
 *
 *    There are several symbolic constants:
 *     symconst_type_tag   The symbolic constant represents a type tag.
 *     symconst_type_size  The symbolic constant represents the size of a type.
 *     symconst_type_align The symbolic constant represents the alignment of a type.
 *     symconst_addr_ent   The symbolic constant represents the address of an entity.
 *     symconst_ofs_ent    The symbolic constant represents the offset of an
 *                         entity in its owner type.
 *     symconst_enum_const The symbolic constant is a enumeration constant of an
 *                         enumeration type.
 *
 *    Parameters
 *      mode        P for SymConsts representing addresses, Iu otherwise.
 *      value       The type, ident, entity or enum constant, depending on the
 *                  kind
 *      kind        The kind of the symbolic constant, see the list above.
 *
 *    Inputs:
 *      No inputs except the block it belongs to.
 *    Output:
 *      A symbolic constant.
 *
 *    Attributes:
 *      attr.i.num       The symconst_addr_ent, i.e. one of
 *                        -symconst_type_tag
 *                        -symconst_type_size
 *                        -symconst_type_align
 *                        -symconst_addr_ent
 *
 *    If the attr.i.num is symconst_type_tag, symconst_type_size or symconst_type_align,
 *    the node contains an attribute:
 *
 *      attr.i.*type,    a pointer to a type_class.
 *        if it is linkage_ptr_info it contains
 *      attr.i.*ptrinfo,  an ident holding information for the linker.
 *
 *    ---------------
 *
 *    ir_node *new_simpleSel (ir_node *store, ir_node *frame, ir_entity *sel)
 *    -----------------------------------------------------------------------
 *
 *
 *    Selects an entity from a compound type. This entity can be a field or
 *    a method.
 *
 *    Parameters:
 *      *store     The memory in which the object the entity should be selected
 *                 from is allocated.
 *      *frame     The pointer to the object.
 *      *sel       The entity to select.
 *
 *    Inputs:
 *      The memory containing the object.
 *      A pointer to the object.
 *      An unsigned integer.
 *    Output:
 *      A pointer to the selected entity.
 *    Attributes:
 *      attr.sel   Pointer to the entity
 *
 *
 *    ir_node *new_Sel (ir_node *store, ir_node *frame, int arity, ir_node **in,
 *    --------------------------------------------------------------------------
 *                      ir_entity *sel)
 *                      ---------------
 *
 *    Selects a field from an array type.  The entity has as owner the array, as
 *    type the arrays element type.  The indices to access an array element are
 *    given also.
 *
 *    Parameters:
 *      *store     The memory in which the object the entity should be selected from
 *                 is allocated.
 *      *frame     The pointer to the object.
 *      *arity     number of array indices.
 *      *in        array with index inputs to the node.
 *      *sel       The entity to select.
 *
 *    Inputs:
 *      The memory containing the object.
 *      A pointer to the object.
 *      As much unsigned integer as there are array expressions.
 *    Output:
 *      A pointer to the selected entity.
 *    Attributes:
 *      attr.sel   Pointer to the entity
 *
 *    The constructors new_Sel and new_simpleSel generate the same IR nodes.
 *    simpleSel just sets the arity of the index inputs to zero.
 *
 *
 *    ARITHMETIC OPERATIONS
 *    ---------------------
 *
 *    ir_node *new_Call (ir_node *store, ir_node *callee, int arity, ir_node **in,
 *    ----------------------------------------------------------------------------
 *                       type_method *type)
 *                       ------------------
 *
 *    Creates a procedure call.
 *
 *    Parameters
 *      *store           The actual store.
 *      *callee          A pointer to the called procedure.
 *      arity            The number of procedure parameters.
 *      **in             An array with the pointers to the parameters.
 *                       The constructor copies this array.
 *      *type            Type information of the procedure called.
 *
 *    Inputs:
 *      The store, the callee and the parameters.
 *    Output:
 *      A tuple containing the eventually changed store and the procedure
 *      results.
 *    Attributes:
 *      attr.call        Contains the attributes for the procedure.
 *
 *    ir_node *new_Builtin(ir_node *store, ir_builtin_kind kind, int arity, ir_node **in,
 *    -----------------------------------------------------------------------------------
 *                       type_method *type)
 *                       ------------------
 *
 *    Creates a builtin call.
 *
 *    Parameters
 *      *store           The actual store.
 *      kind             Describes the called builtin.
 *      arity            The number of procedure parameters.
 *      **in             An array with the pointers to the parameters.
 *                       The constructor copies this array.
 *      *type            Type information of the procedure called.
 *
 *    Inputs:
 *      The store, the kind and the parameters.
 *    Output:
 *      A tuple containing the eventually changed store and the procedure
 *      results.
 *    Attributes:
 *      attr.builtin     Contains the attributes for the called builtin.
 *
 *    ir_node *new_Add (ir_node *op1, ir_node *op2, ir_mode *mode)
 *    ------------------------------------------------------------
 *
 *    Trivial.
 *
 *    ir_node *new_Sub (ir_node *op1, ir_node *op2, ir_mode *mode)
 *    ------------------------------------------------------------
 *
 *    Trivial.
 *
 *    ir_node *new_Minus (ir_node *op, ir_mode *mode)
 *    -----------------------------------------------
 *
 *    Unary Minus operations on integer and floating point values.
 *
 *    ir_node *new_Mul (ir_node *op1, ir_node *op2, ir_mode *mode)
 *    ------------------------------------------------------------
 *
 *    Trivial.
 *
 *    ir_node *new_Mulh (ir_node *op1, ir_node *op2, ir_mode *mode)
 *    ------------------------------------------------------------
 *
 *    Returns the high order bits of a n*n=2n multiplication.
 *
 *    ir_node *new_Quot (ir_node *memop, ir_node *op1, ir_node *op2, ir_mode *mode, op_pin_state state)
 *    -------------------------------------------------------------------------------------------------
 *
 *    Quot performs exact division of floating point numbers.  It's mode
 *    is Tuple, the mode of the result must match the Proj mode
 *    that extracts the result of the arithmetic operations.
 *
 *    Inputs:
 *      The store needed to model exceptions and the two operands.
 *    Output:
 *      A tuple containing a memory and a execution for modeling exceptions
 *      and the result of the arithmetic operation.
 *
 *    ir_node *new_DivMod (ir_node *memop, ir_node *op1, ir_node *op2, ir_mode *mode, op_pin_state state)
 *    ---------------------------------------------------------------------------------------------------
 *
 *    Performs Div and Mod on integer values.
 *
 *    Output:
 *      A tuple containing a memory and a execution for modeling exceptions
 *      and the two result of the arithmetic operations.
 *
 *    ir_node *new_Div (ir_node *memop, ir_node *op1, ir_node *op2, ir_mode *mode, op_pin_state state)
 *    ------------------------------------------------------------------------------------------------
 *
 *    Trivial.
 *
 *    ir_node *new_Mod (ir_node *memop, ir_node *op1, ir_node *op2, ir_mode *mode, op_pin_state state)
 *    ------------------------------------------------------------------------------------------------
 *
 *    Trivial.
 *
 *    ir_node *new_And (ir_node *op1, ir_node *op2, ir_mode *mode)
 *    ------------------------------------------------------------
 *
 *    Trivial.
 *
 *    ir_node *new_Or (ir_node *op1, ir_node *op2, ir_mode *mode)
 *    -----------------------------------------------------------
 *
 *    Trivial.
 *
 *    ir_node *new_Eor (ir_node *op1, ir_node *op2, ir_mode *mode)
 *    ------------------------------------------------------------
 *
 *    Trivial.
 *
 *    ir_node *new_Not (ir_node *op, ir_mode *mode)
 *    ---------------------------------------------
 *
 *    This node constructs a constant where all bits are set to one
 *    and a Eor of this constant and the operator.  This simulates a
 *    Not operation.
 *
 *    ir_node *new_Shl (ir_node *op, ir_node *k, ir_mode *mode)
 *    ---------------------------------------------------------
 *
 *    Trivial.
 *
 *    ir_node *new_Shr (ir_node *op, ir_node *k, ir_mode *mode)
 *    ---------------------------------------------------------
 *
 *    Logic shift right, i.e., zero extended.
 *
 *
 *    ir_node *new_Shrs (ir_node *op, ir_node *k, ir_mode *mode)
 *    ----------------------------------------------------------
 *
 *    Arithmetic shift right, i.e., sign extended.
 *
 *    ir_node *new_Rotl (ir_node *op, ir_node *k, ir_mode *mode)
 *    ---------------------------------------------------------
 *
 *    Rotates the operand to the left by k bits.
 *
 *    ir_node *new_Carry (ir_node *op1, ir_node *op2, ir_mode *mode)
 *    ------------------------------------------------------------
 *
 *    Calculates the Carry value for integer addition. Used only
 *    in lowering code.
 *
 *    ir_node *new_Borrow (ir_node *op1, ir_node *op2, ir_mode *mode)
 *    ------------------------------------------------------------
 *
 *    Calculates the Borrow value for integer substraction. Used only
 *    in lowering code.
 *
 *    ir_node *new_Conv (ir_node *op, ir_mode *mode)
 *    ---------------------------------------------
 *
 *    Mode conversion.  For allowed conversions see UKA Tech Report
 *    1999-14.
 *
 *    ir_node *new_Cmp (ir_node *op1, ir_node *op2)
 *    ---------------------------------------------
 *
 *    Input:
 *      The two values to be compared.
 *    Output:
 *      A 16-tuple containing the results of the 16 different comparisons.
 *      The following is a list giving the comparisons and a projection
 *      number (pn_Cmp) to use in Proj nodes to extract the proper result.
 *        pn_Cmp_False false
 *        pn_Cmp_Eq    equal
 *        pn_Cmp_Lt    less
 *        pn_Cmp_Le    less or equal
 *        pn_Cmp_Gt    greater
 *        pn_Cmp_Ge    greater of equal
 *        pn_Cmp_Lg    less or greater
 *        pn_Cmp_Leg   less, equal or greater = ordered
 *        pn_Cmp_Uo    unordered
 *        pn_Cmp_Ue    unordered or equal
 *        pn_Cmp_Ul    unordered or less
 *        pn_Cmp_Ule   unordered, less or equal
 *        pn_Cmp_Ug    unordered or greater
 *        pn_Cmp_Uge   unordered, greater or equal
 *        pn_Cmp_Ne    unordered, less or greater = not equal
 *        pn_Cmp_True  true
 *
 *
 *
 *    ------------
 *
 *    In general, Phi nodes are automaitcally inserted.  In some cases, if
 *    all predecessors of a block are known, an explicit Phi node constructor
 *    is needed.  E.g., to construct a FIRM graph for a statement as
 *      a = (b==c) ? 2 : 5;
 *
 *    ir_node *new_Phi (int arity, ir_node **in, ir_mode *mode)
 *    ---------------------------------------------------------
 *
 *    Creates a Phi node. The in's order has to correspond to the order
 *    of in's of current_block.  This is not checked by the library!
 *    If one of the predecessors is Unknown (as it has to be filled in
 *    later) optimizations are skipped.  This is necessary to
 *    construct Phi nodes in loops.
 *
 *    Parameter
 *      arity            number of predecessors
 *      **in             array with predecessors
 *      *mode            The mode of it's inputs and output.
 *    Inputs:
 *      A Phi node has as many inputs as the block it belongs to.
 *      Each input points to a definition of the same value on a
 *      different path in the control flow.
 *    Output
 *      The definition valid in this block.
 *
 *    ir_node *new_Mux (ir_node *sel, ir_node *ir_false, ir_node *ir_true, ir_mode *mode)
 *    -----------------------------------------------------------------------------------
 *
 *    Creates a Mux node. This node implements the following semantic:
 *    If the sel node (which must be of mode_b) evaluates to true, its value is
 *    ir_true, else ir_false;
 *
 *
 *
 *    OPERATIONS TO MANAGE MEMORY EXPLICITLY
 *    --------------------------------------
 *
 *    ir_node *new_Load (ir_node *store, ir_node *addr, ir_mode *mode, ir_cons_flags flags)
 *    -------------------------------------------------------------------------------------
 *
 *    The Load operation reads a value from memory.
 *
 *    Parameters:
 *    *store        The current memory.
 *    *addr         A pointer to the variable to be read in this memory.
 *    *mode         The mode of the value to be loaded.
 *     flags        Additional flags for alignment, volatility and pin state.
 *
 *    Inputs:
 *      The memory and a pointer to a variable in this memory.
 *    Output:
 *      A tuple of the memory, a control flow to be taken in case of
 *      an exception and the loaded value.
 *
 *    ir_node *new_Store (ir_node *store, ir_node *addr, ir_node *val, ir_cons_flags flags)
 *    -------------------------------------------------------------------------------------
 *
 *    The Store operation writes a value to a variable in memory.
 *
 *    Inputs:
 *      The memory, a pointer to a variable in this memory and the value
 *      to write to this variable.
 *    Output:
 *      A tuple of the changed memory and a control flow to be taken in
 *      case of an exception.
 *
 *    ir_node *new_Alloc (ir_node *store, ir_node *count, ir_type *alloc_type,
 *    -----------------------------------------------------------------------
 *                        where_alloc where)
 *                        ------------------
 *
 *    The Alloc node allocates a new variable.  It can be specified whether the
 *    variable should be allocated to the stack or to the heap.
 *
 *    Parameters:
 *      *store       The memory which shall contain the new variable.
 *      *count       This field is for allocating arrays, it specifies how
 *                   many array elements are to be allocated.
 *      *alloc_type  The type of the allocated variable. In case of allocating
 *                   arrays this has to be the array type, not the type of the
 *                   array elements.
 *      where        Where to allocate the variable, either heap_alloc or stack_alloc.
 *
 *    Inputs:
 *      A memory and an unsigned integer.
 *    Output:
 *      A tuple of the changed memory, a control flow to be taken in
 *      case of an exception and the pointer to the new variable.
 *    Attributes:
 *      a.where          Indicates where the variable is allocated.
 *      a.*type          A pointer to the class the allocated data object
 *                       belongs to.
 *
 *    ir_node *new_Free (ir_node *store, ir_node *ptr, ir_node *size, ir_type *free_type,
 *    -----------------------------------------------------------------------------------
 *                        where_alloc where)
 *                        ------------------
 *
 *    The Free node frees memory of the given variable.
 *
 *    Parameters:
 *      *store       The memory which shall contain the new variable.
 *      *ptr         The pointer to the object to free.
 *      *size        The number of objects of type free_type to free in a sequence.
 *      *free_type   The type of the freed variable.
 *      where        Where the variable was allocated, either heap_alloc or stack_alloc.
 *
 *    Inputs:
 *      A memory, a pointer and an unsigned integer.
 *    Output:
 *      The changed memory.
 *    Attributes:
 *      f.*type          A pointer to the type information of the freed data object.
 *
 *    Not Implemented!
 *
 *    ir_node *new_Sync (int arity, ir_node **in)
 *    -------------------------------------------
 *
 *    The Sync operation unifies several partial memory blocks.  These blocks
 *    have to be pairwise disjunct or the values in common locations have to
 *    be identical.  This operation allows to specify all operations that eventually
 *    need several partial memory blocks as input with a single entrance by
 *    unifying the memories with a preceding Sync operation.
 *
 *    Parameters
 *      arity    The number of memories to synchronize.
 *      **in     An array of pointers to nodes that produce an output of
 *               type memory.
 *    Inputs
 *      Several memories.
 *    Output
 *      The unified memory.
 *
 *
 *    SPECIAL OPERATIONS
 *    ------------------
 *
 *    ir_node *new_Bad (void)
 *    -----------------------
 *
 *    Returns the unique Bad node current_ir_graph->bad.
 *    This node is used to express results of dead code elimination.
 *
 *    ir_node *new_NoMem (void)
 *    -----------------------------------------------------------------------------------
 *
 *    Returns the unique NoMem node current_ir_graph->no_mem.
 *    This node is used as input for operations that need a Memory, but do not
 *    change it like Div by const != 0, analyzed calls etc.
 *
 *    ir_node *new_Proj (ir_node *arg, ir_mode *mode, long proj)
 *    ----------------------------------------------------------
 *
 *    Selects one entry of a tuple.  This is a hidden edge with attributes.
 *
 *    Parameters
 *      *arg      A node producing a tuple.
 *      *mode     The mode of the value to project.
 *      proj      The position of the value in the tuple.
 *    Input:
 *      The tuple.
 *    Output:
 *      The value.
 *
 *    ir_node *new_Tuple (int arity, ir_node **in)
 *    --------------------------------------------
 *
 *    Builds a Tuple from single values.  This is needed to implement
 *    optimizations that remove a node that produced a tuple.  The node can be
 *    replaced by the Tuple operation so that the following Proj nodes have not to
 *    be changed.  (They are hard to find due to the implementation with pointers
 *    in only one direction.)  The Tuple node is smaller than any other
 *    node, so that a node can be changed into a Tuple by just changing it's
 *    opcode and giving it a new in array.
 *
 *    Parameters
 *      arity    The number of tuple elements.
 *      **in     An array containing pointers to the nodes producing the
 *               tuple elements.
 *
 *    ir_node *new_Id (ir_node *val, ir_mode *mode)
 *    ---------------------------------------------
 *
 *    The single output of the Id operation is it's input.  Also needed
 *    for optimizations.
 *
 *
 *    HIGH LEVEL OPERATIONS
 *    ---------------------
 *
 *    ir_node *new_CopyB (ir_node *store, ir_node *dst, ir_node *src, ir_type *data_type)
 *    -----------------------------------------------------------------------------------
 *
 *    Describes a high level block copy of a compound type from address src to
 *    address dst. Must be lowered to a Call to a runtime memory copy function.
 *
 *
 *    HIGH LEVEL OPERATIONS: Exception Support
 *    ----------------------------------------
 *    See TechReport 1999-14, chapter Exceptions.
 *
 *    ir_node *new_InstOf(ir_node *store, ir_node *ptr, ir_type *type);
 *    -----------------------------------------------------------------------------------
 *
 *    Describes a high level type check. Must be lowered to a Call to a runtime check
 *    function.
 *
 *    ir_node *new_Raise (ir_node *store, ir_node *obj)
 *    -------------------------------------------------
 *
 *    Raises an exception.  Unconditional change of control flow.  Writes
 *    an explicit Except variable to memory to pass it to the exception
 *    handler.  Must be lowered to a Call to a runtime check
 *    function.
 *
 *    Inputs:
 *      The memory state.
 *      A pointer to the Except variable.
 *    Output:
 *      A tuple of control flow and the changed memory state.  The control flow
 *      points to the exception handler if it is definied in this procedure,
 *      else it points to the end_block.
 *
 *    ir_node *new_Bound  (ir_node *store, ir_node *idx, ir_node *lower, ir_node *upper);
 *    -----------------------------------------------------------------------------------
 *
 *    Describes a high level bounds check. Must be lowered to a Call to a runtime check
 *    function.
 *
 *    ir_node *new_Pin  (ir_node *node);
 *    -----------------------------------------------------------------------------------
 *
 *    Pin the value of the node node in the current block  No users of the Pin node can
 *    float above the Block of the Pin. The node cannot float behind this block. Often
 *    used to Pin the NoMem node.
 *
 *
 *    COPING WITH DATA OBJECTS
 *    ========================
 *
 *    Two kinds of data objects have to be distinguished for generating
 *    FIRM.  First there are local variables other than arrays that are
 *    known to be alias free.  Second there are all other data objects.
 *    For the first a common SSA representation is built, the second
 *    are modeled by saving them to memory.  The memory is treated as
 *    a single local variable, the alias problem is hidden in the
 *    content of this variable.
 *
 *    All values known in a Block are listed in the block's attribute,
 *    block.**graph_arr which is used to automatically insert Phi nodes.
 *    The following two functions can be used to add a newly computed value
 *    to the array, or to get the producer of a value, i.e., the current
 *    live value.
 *
 *    inline void set_value (int pos, ir_node *value)
 *    -----------------------------------------------
 *
 *    Has to be called for every assignment to a local variable.  It
 *    adds the value to the array of used values at position pos.  Pos
 *    has to be a unique identifier for an entry in the procedure's
 *    definition table.  It can be used to access the value again.
 *    Requires current_block to be set correctly.
 *
 *    ir_node *get_value (int pos, ir_mode *mode)
 *    -------------------------------------------
 *
 *    Returns the node defining the value referred to by pos. If the
 *    value is not defined in this block a Phi node is generated and
 *    all definitions reaching this Phi node are collected.  It can
 *    happen that the algorithm allocates an unnecessary Phi node,
 *    e.g. if there is only one definition of this value, but this
 *    definition reaches the currend block on several different
 *    paths.  This Phi node will be eliminated if optimizations are
 *    turned on right after it's creation.
 *    Requires current_block to be set correctly.
 *
 *    There are two special routines for the global store:
 *
 *    void set_store (ir_node *store)
 *    -------------------------------
 *
 *    Adds the store to the array of known values at a reserved
 *    position.
 *    Requires current_block to be set correctly.
 *
 *    ir_node *get_store (void)
 *    -------------------------
 *
 *    Returns the node defining the actual store.
 *    Requires current_block to be set correctly.
 *
 *
 *    inline void keep_alive (ir_node *ka)
 *    ------------------------------------
 *
 *    Keep this node alive because it is (might be) not in the control
 *    flow from Start to End.  Adds the node to the list in the end
 *   node.
 *
 */
#ifndef FIRM_IR_IRCONS_H
#define FIRM_IR_IRCONS_H

#include "firm_types.h"
#include "begin.h"
#include "irnode.h"

/**
 * constrained flags for memory operations.
 */
typedef enum ir_cons_flags {
	cons_none      = 0,        /**< No constrains. */
	cons_volatile  = 1U << 0,  /**< Memory operation is volatile. */
	cons_unaligned = 1U << 1,  /**< Memory operation is unaligned. */
	cons_floats    = 1U << 2   /**< Memory operation can float. */
} ir_cons_flags;

/*-------------------------------------------------------------------------*/
/* The raw interface                                                       */
/*-------------------------------------------------------------------------*/

/** Constructor for a Block node.
 *
 * Constructs a mature block with the given predecessors.
 *
 * @param *db    A Pointer for  debug information.
 * @param irg    The IR graph the block belongs to.
 * @param arity  The number of control predecessors.
 * @param in[]   An array of control predecessors.  The length of
 *               the array must be 'arity'.  The constructor copies this array.
 */
FIRM_API ir_node *new_rd_Block(dbg_info *db, ir_graph *irg, int arity, ir_node *in[]);

/** Constructor for a Start node.
 *
 * @param *db    A pointer for debug information.
 * @param *irg   The IR graph the node belongs to.
 */
FIRM_API ir_node *new_rd_Start(dbg_info *db, ir_graph *irg);

/** Constructor for a End node.
 *
 * @param *db    A pointer for  debug information.
 * @param *irg   The IR graph the node  belongs to.
 */
FIRM_API ir_node *new_rd_End(dbg_info *db, ir_graph *irg, int arity, ir_node *in[]);

/** Constructor for a Jmp node.
 *
 * Jmp represents control flow to a single control successor.
 *
 * @param *db     A pointer for debug information.
 * @param *block  The IR block the node belongs to.
 */
FIRM_API ir_node *new_rd_Jmp(dbg_info *db, ir_node *block);

/** Constructor for an IJmp node.
 *
 * IJmp represents control flow to a single control successor not
 * statically known i.e. an indirect Jmp.
 *
 * @param *db     A pointer for debug information.
 * @param *block  The IR block the node belongs to.
 * @param *tgt    The IR node representing the target address.
 */
FIRM_API ir_node *new_rd_IJmp(dbg_info *db, ir_node *block, ir_node *tgt);

/** Constructor for a Cond node.
 *
 * If c is mode_b represents a conditional branch (if/else). If c is
 * mode_Is/mode_Iu (?) represents a switch.  (Allocates dense Cond
 * node, default Proj is 0.)
 *
 * This is not consistent:  Input to Cond is Is, Proj has as proj number
 * longs.
 *
 * @param *db    A pointer for debug information.
 * @param *block The IR block the node belongs to.
 * @param *c     The conditions parameter. Can be of mode b or I_u.
 */
FIRM_API ir_node *new_rd_Cond(dbg_info *db, ir_node *block, ir_node *c);

/** Constructor for a Return node.
 *
 * Returns the memory and zero or more return values.  Only node that
 * can end regular control flow.
 *
 * @param *db    A pointer for debug information.
 * @param *block The IR block the node belongs to.
 * @param *store The state of memory.
 * @param arity  Number of return values.
 * @param *in    Array of length arity with return values.  The constructor copies this array.
 */
FIRM_API ir_node *new_rd_Return(dbg_info *db, ir_node *block,
                                ir_node *store, int arity, ir_node *in[]);

/** Constructor for a Const node.
 *
 * Adds the node to the start block.
 *
 * Constructor for a Const node. The constant represents a target
 * value.  Sets the type information to type_unknown.  (No more
 * supported: If tv is entity derives a somehow useful type.)
 * Derives mode from passed tarval.
 *
 * @param *db    A pointer for debug information.
 * @param *irg   The IR graph the node  belongs to.
 * @param *con   Points to an entry in the constant table.
 */
FIRM_API ir_node *new_rd_Const(dbg_info *db, ir_graph *irg, ir_tarval *con);

/**
 * Constructor for a Const node.
 *
 * Adds the node to the start block.
 *
 * Constructor for a Const node. The constant represents a target
 * value.  Sets the type information to type_unknown.  (No more
 * supported: If tv is entity derives a somehow useful type.)
 *
 * @param *db    A pointer for debug information.
 * @param *irg   The IR graph the node  belongs to.
 * @param *mode  The mode of the operands and results.
 * @param value  A value from which the tarval is made.
 */
FIRM_API ir_node *new_rd_Const_long(dbg_info *db, ir_graph *irg,
                                    ir_mode *mode, long value);

/** Constructor for a SymConst node.
 *
 *  This is the constructor for a symbolic constant.
 *    There are several kinds of symbolic constants:
 *    - symconst_type_tag   The symbolic constant represents a type tag.  The
 *                          type the tag stands for is given explicitly.
 *    - symconst_type_size  The symbolic constant represents the size of a type.
 *                          The type of which the constant represents the size
 *                          is given explicitly.
 *    - symconst_type_align The symbolic constant represents the alignment of a
 *                          type.  The type of which the constant represents the
 *                          size is given explicitly.
 *    - symconst_addr_ent   The symbolic constant represents the address of an
 *                          entity (variable or method).  The variable is given
 *                          explicitly by a firm entity.
 *    - symconst_ofs_ent    The symbolic constant represents the offset of an
 *                          entity in its owner type.
 *    - symconst_enum_const The symbolic constant is a enumeration constant of
 *                          an enumeration type.
 *
 *    Inputs to the node:
 *      No inputs except the block it belongs to.
 *    Outputs of the node.
 *      An unsigned integer (I_u) or a pointer (P).
 *
 *    Mention union in declaration so that the firmjni generator recognizes that
 *    it can not cast the argument to an int.
 *
 * @param *db     A pointer for debug information.
 * @param *irg    The IR graph the node  belongs to.
 * @param mode    The mode for the SymConst.
 * @param val     A type, ident, entity or enum constant depending on the
 *                SymConst kind.
 * @param kind    The kind of the symbolic constant, see the list above
 */
FIRM_API ir_node *new_rd_SymConst(dbg_info *db, ir_graph *irg, ir_mode *mode,
                                  union symconst_symbol value,
                                  symconst_kind kind);

/** Constructor for a SymConst addr_ent node.
 *
 * Same as new_rd_SymConst, except that the constructor is tailored for
 * symconst_addr_ent.
 * Adds the SymConst to the start block of irg. */
FIRM_API ir_node *new_rd_SymConst_addr_ent(dbg_info *db, ir_graph *irg,
                                           ir_mode *mode, ir_entity *symbol);

/** Constructor for a SymConst ofs_ent node.
 *
 * Same as new_rd_SymConst, except that the constructor is tailored for
 * symconst_ofs_ent.
 * Adds the SymConst to the start block of irg.
 */
FIRM_API ir_node *new_rd_SymConst_ofs_ent(dbg_info *db, ir_graph *irg,
                                          ir_mode *mode, ir_entity *symbol);

/** Constructor for a SymConst type_tag node.
 *
 * Same as new_rd_SymConst, except that the constructor is tailored for
 * symconst_type_tag.
 * Adds the SymConst to the start block of irg.
 */
FIRM_API ir_node *new_rd_SymConst_type_tag(dbg_info *db, ir_graph *irg,
                                           ir_mode *mode, ir_type *symbol);

/** Constructor for a SymConst size node.
 *
 * Same as new_rd_SymConst, except that the constructor is tailored for
 * symconst_type_size.
 * Adds the SymConst to the start block of irg. */
FIRM_API ir_node *new_rd_SymConst_size(dbg_info *db, ir_graph *irg,
                                       ir_mode *mode, ir_type *symbol);

/** Constructor for a SymConst size node.
 *
 * Same as new_rd_SymConst, except that the constructor is tailored for
 * symconst_type_align.
 * Adds the SymConst to the start block of irg.
 */
FIRM_API ir_node *new_rd_SymConst_align(dbg_info *db, ir_graph *irg,
                                        ir_mode *mode, ir_type *symbol);

/** Constructor for a simpleSel node.
 *
 *  This is a shortcut for the new_rd_Sel() constructor.  To be used for
 *  Sel nodes that do not select from an array, i.e., have no index
 *  inputs.  It adds the two parameters 0, NULL.
 *
 * @param   *db        A pointer for debug information.
 * @param   *block     The IR block the node belongs to.
 * @param   *store     The memory in which the object the entity should be
 *                     selected from is allocated.
 * @param   *objptr    The object from that the Sel operation selects a
 *                     single attribute out.
 * @param   *ent       The entity to select.
 */
FIRM_API ir_node *new_rd_simpleSel(dbg_info *db, ir_node *block, ir_node *store,
                                   ir_node *objptr, ir_entity *ent);

/** Constructor for a Sel node.
 *
 * The select node selects an entity (field or method) from an entity
 * with a compound type.  It explicitly specifies the entity selected.
 * Dynamically the node may select entities that overwrite the given
 * entity.  If the selected entity is an array element entity the Sel
 * node takes the required array indices as inputs.
 *
 * @param   *db        A pointer for debug information.
 * @param   *block     The IR block the node belongs to.
 * @param   *store     The memory in which the object the entity should be selected
 *                     from is allocated.
 * @param   *objptr    A pointer to a compound entity the Sel operation selects a
 *                     single attribute from.
 * @param   *n_index   The number of array indices needed to select an array element entity.
 * @param   *index[]   If the compound entity is an array the indices of the selected
 *                     element entity.  The constructor copies this array.
 * @param   *ent       The entity to select.
 */
FIRM_API ir_node *new_rd_Sel(dbg_info *db, ir_node *block, ir_node *store,
                             ir_node *objptr, int n_index, ir_node *index[],
                             ir_entity *ent);

/** Constructor for a Call node.
 *
 * Represents all kinds of method and function calls.
 *
 * @param   *db     A pointer for debug information.
 * @param   *block  The IR block the node belongs to.
 * @param   *store  The current memory state.
 * @param   *callee A pointer to the called procedure.
 * @param   arity   The number of procedure parameters.
 * @param   *in[]   An array with the procedure parameters. The constructor copies this array.
 * @param   *tp     Type information of the procedure called.
 */
FIRM_API ir_node *new_rd_Call(dbg_info *db, ir_node *block, ir_node *store,
                              ir_node *callee, int arity, ir_node *in[],
                              ir_type *tp);

/** Constructor for a Builtin node.
 *
 * Represents a call of a backend-specific builtin..
 *
 * @param   *db     A pointer for debug information.
 * @param   *block  The IR block the node belongs to.
 * @param   *store  The current memory state.
 * @param   arity   The number of procedure parameters.
 * @param   *in[]   An array with the procedure parameters. The constructor copies this array.
 * @param   kind    The kind of the called builtin.
 * @param   *tp     Type information of the procedure called.
 */
FIRM_API ir_node *new_rd_Builtin(dbg_info *db, ir_node *block, ir_node *store,
                                 int arity, ir_node *in[], ir_builtin_kind kind,
                                 ir_type *tp);

/** Constructor for a Add node.
 *
 * @param   *db    A pointer for debug information.
 * @param   *block The IR block the node belongs to.
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_rd_Add(dbg_info *db, ir_node *block, ir_node *op1,
                             ir_node *op2, ir_mode *mode);

/** Constructor for a Sub node.
 *
 * @param   *db    A pointer for debug information.
 * @param   *block The IR block the node belongs to.
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_rd_Sub(dbg_info *db, ir_node *block,
                             ir_node *op1, ir_node *op2, ir_mode *mode);

/** Constructor for a Minus node.
 *
 * @param   *db    A pointer for debug information.
 * @param   *block The IR block the node belongs to.
 * @param   *op    The operand .
 * @param   *mode  The mode of the operand and the result.
 */
FIRM_API ir_node *new_rd_Minus(dbg_info *db, ir_node *block,
                               ir_node *op, ir_mode *mode);

/** Constructor for a Mul node.
 *
 * @param   *db    A pointer for debug information.
 * @param   *block The IR block the node belongs to.
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_rd_Mul(dbg_info *db, ir_node *block,
                             ir_node *op1, ir_node *op2, ir_mode *mode);

/** Constructor for a Mulh node.
 *
 * @param   *db    A pointer for debug information.
 * @param   *block The IR block the node belongs to.
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_rd_Mulh(dbg_info *db, ir_node *block,
                              ir_node *op1, ir_node *op2, ir_mode *mode);

/** Constructor for a Quot node.
 *
 * @param   *db    A pointer for debug information.
 * @param   *block The IR block the node belongs to.
 * @param   *memop The store needed to model exceptions
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the result.
 * @param   state  The pinned state.
 */
FIRM_API ir_node *new_rd_Quot(dbg_info *db, ir_node *block, ir_node *memop,
                              ir_node *op1, ir_node *op2, ir_mode *mode,
                              op_pin_state state);

/** Constructor for a DivMod node.
 *
 * @param   *db    A pointer for debug information.
 * @param   *block The IR block the node belongs to.
 * @param   *memop The store needed to model exceptions
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the results.
 * @param   state  The pinned state.
 */
FIRM_API ir_node *new_rd_DivMod(dbg_info *db, ir_node *block, ir_node *memop,
                                ir_node *op1, ir_node *op2, ir_mode *mode,
                                op_pin_state state);

/** Constructor for a Div node.
 *
 * @param   *db    A pointer for debug information.
 * @param   *block The IR block the node belongs to.
 * @param   *memop The store needed to model exceptions
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the result.
 * @param   state  The pinned state.
 */
FIRM_API ir_node *new_rd_Div(dbg_info *db, ir_node *block, ir_node *memop,
                             ir_node *op1, ir_node *op2, ir_mode *mode,
                             op_pin_state state);

/** Constructor for a remainderless Div node.
 *
 * @param   *db    A pointer for debug information.
 * @param   *block The IR block the node belongs to.
 * @param   *memop The store needed to model exceptions
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the result.
 * @param   state  The pinned state.
 */
FIRM_API ir_node *new_rd_DivRL(dbg_info *db, ir_node *block, ir_node *memop,
                               ir_node *op1, ir_node *op2, ir_mode *mode,
                               op_pin_state state);

/** Constructor for a Mod node.
 *
 * @param   *db    A pointer for debug information.
 * @param   *block The IR block the node belongs to.
 * @param   *memop The store needed to model exceptions
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the result.
 * @param   state  The pinned state.
 */
FIRM_API ir_node *new_rd_Mod(dbg_info *db, ir_node *block, ir_node *memop,
                             ir_node *op1, ir_node *op2, ir_mode *mode,
                             op_pin_state state);

/** Constructor for a And node.
 *
 * @param   *db    A pointer for debug information.
 * @param   *block The IR block the node belongs to.
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_rd_And(dbg_info *db, ir_node *block,
                             ir_node *op1, ir_node *op2, ir_mode *mode);

/** Constructor for a Or node.
 *
 * @param   *db    A pointer for debug information.
 * @param   *block The IR block the node belongs to.
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_rd_Or(dbg_info *db, ir_node *block,
                            ir_node *op1, ir_node *op2, ir_mode *mode);

/** Constructor for a Eor node.
 *
 * @param   *db    A pointer for debug information.
 * @param   *block The IR block the node belongs to.
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the results.
 */
FIRM_API ir_node *new_rd_Eor(dbg_info *db, ir_node *block,
                             ir_node *op1, ir_node *op2, ir_mode *mode);

/** Constructor for a Not node.
 *
 * @param   *db    A pointer for debug information.
 * @param   *block The IR block the node belongs to.
 * @param   *op    The operand.
 * @param   *mode  The mode of the operand and the result.
 */
FIRM_API ir_node *new_rd_Not(dbg_info *db, ir_node *block, ir_node *op,
                             ir_mode *mode);

/** Constructor for a Cmp node.
 *
 * @param   *db    A pointer for debug information.
 * @param   *block The IR block the node belongs to.
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 */
FIRM_API ir_node *new_rd_Cmp(dbg_info *db, ir_node *block,
                             ir_node *op1, ir_node *op2);

/** Constructor for a Shl node.
 *
 * @param   *db    A pointer for debug information.
 * @param   *block The IR block the node belongs to.
 * @param   *op    The operand.
 * @param   *k     The number of bits to  shift the operand .
 * @param   *mode  The mode of the operand and the result.
 */
FIRM_API ir_node *new_rd_Shl(dbg_info *db, ir_node *block,
                             ir_node *op, ir_node *k, ir_mode *mode);

/** Constructor for a Shr node.
 *
 * @param   *db    A pointer for debug information.
 * @param   *block The IR block the node belongs to.
 * @param   *op    The operand.
 * @param   *k     The number of bits to shift the operand .
 * @param   *mode  The mode of the operand and the result.
 */
FIRM_API ir_node *new_rd_Shr(dbg_info *db, ir_node *block,
                             ir_node *op, ir_node *k, ir_mode *mode);

/** Constructor for a Shrs node.
 *
 * @param   *db    A pointer for debug information.
 * @param   *block The IR block the node belongs to.
 * @param   *op    The operand.
 * @param   *k     The number of bits to shift the operand.
 * @param   *mode  The mode of the operand and the result.
 */
FIRM_API ir_node *new_rd_Shrs(dbg_info *db, ir_node *block,
                              ir_node *op, ir_node *k, ir_mode *mode);

/** Constructor for a Rotl node.
 *
 * @param   *db    A pointer for debug information.
 * @param   *block The IR block the node belongs to.
 * @param   *op    The operand.
 * @param   *k     The number of bits to rotate the operand.
 * @param   *mode  The mode of the operand.
 */
FIRM_API ir_node *new_rd_Rotl(dbg_info *db, ir_node *block,
                              ir_node *op, ir_node *k, ir_mode *mode);


/** Constructor for a Conv node.
 *
 * @param   *db    A pointer for debug information.
 * @param   *block The IR block the node belongs to.
 * @param   *op    The operand.
 * @param   *mode  The mode of this the operand muss be converted .
 */
FIRM_API ir_node *new_rd_Conv(dbg_info *db, ir_node *block, ir_node *op,
                              ir_mode *mode);

/** Constructor for a strictConv node.
 *
 * @param   *db    A pointer for debug information.
 * @param   *block The IR block the node belongs to.
 * @param   *op    The operand.
 * @param   *mode  The mode of this the operand muss be converted .
 */
FIRM_API ir_node *new_rd_strictConv(dbg_info *db, ir_node *block,
                                    ir_node *op, ir_mode *mode);

/** Constructor for a Cast node.
 *
 * High level type cast.
 *
 * @param   *db    A pointer for debug information.
 * @param   *block The IR block the node belongs to.
 * @param   *op    The operand.
 * @param   *to_tp The type of this the operand muss be casted .
 */
FIRM_API ir_node *new_rd_Cast(dbg_info *db, ir_node *block,
                              ir_node *op, ir_type *to_tp);

/** Constructor for a Carry node.
 * Note: This node is not supported by the backends! Only use for program
 * analysis tasks.
 *
 * @param   *db    A pointer for debug information.
 * @param   *block The IR block the node belongs to.
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_rd_Carry(dbg_info *db, ir_node *block,
                               ir_node *op1, ir_node *op2, ir_mode *mode);

/** Constructor for a Borrow node.
 * Note: This node is not supported by the backends! Only use for program
 * analysis tasks.
 *
 * @param   *db    A pointer for debug information.
 * @param   *block The IR block the node belongs to.
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_rd_Borrow(dbg_info *db, ir_node *block,
                                ir_node *op1, ir_node *op2, ir_mode *mode);

/** Constructor for a Phi node.
 *
 * @param *db    A pointer for debug information.
 * @param *block The IR block the node belongs to.
 * @param arity  The number of predecessors
 * @param *in[]  Array with predecessors.  The constructor copies this array.
 * @param *mode  The mode of it's inputs and output.
 */
FIRM_API ir_node *new_rd_Phi(dbg_info *db, ir_node *block, int arity,
                             ir_node *in[], ir_mode *mode);

/** Constructor for a Load node.
 *
 * @param *db    A pointer for debug information.
 * @param *block The IR block the node belongs to.
 * @param *store The current memory
 * @param *adr   A pointer to the variable to be read in this memory.
 * @param *mode  The mode of the value to be loaded.
 * @param  flags Additional flags for alignment, volatility and pin state.
 */
FIRM_API ir_node *new_rd_Load(dbg_info *db, ir_node *block, ir_node *store,
                              ir_node *adr, ir_mode *mode, ir_cons_flags flags);

/** Constructor for a Store node.
 *
 * @param *db    A pointer for debug information.
 * @param *block The IR block the node belongs to.
 * @param *store The current memory
 * @param *adr   A pointer to the variable to be read in this memory.
 * @param *val   The value to write to this variable.
 * @param  flags Additional flags for alignment, volatility and pin state.
 */
FIRM_API ir_node *new_rd_Store(dbg_info *db, ir_node *block, ir_node *store,
                               ir_node *adr, ir_node *val, ir_cons_flags flags);

/** Constructor for a Alloc node.
 *
 * The Alloc node extends the memory by space for an entity of type alloc_type.
 *
 * @param *db         A pointer for debug information.
 * @param *block      The IR block the node belongs to.
 * @param *store      The memory which shall contain the new variable.
 * @param *count      The number of objects to allocate.
 * @param *alloc_type The type of the allocated variable.
 * @param where       Where to allocate the variable, either heap_alloc or stack_alloc.
 */
FIRM_API ir_node *new_rd_Alloc(dbg_info *db, ir_node *block, ir_node *store,
                               ir_node *count, ir_type *alloc_type,
                               ir_where_alloc where);

/** Constructor for a Free node.
 *
 * Frees the memory occupied by the entity pointed to by the pointer
 * arg.  Type indicates the type of the entity the argument points to.
 *
 * @param *db         A pointer for debug information.
 * @param *block      The IR block the node belongs to.
 * @param *store      The memory which shall contain the new variable.
 * @param *ptr        The pointer to the object to free.
 * @param *size       The number of objects of type free_type to free in a sequence.
 * @param *free_type  The type of the freed variable.
 * @param where       Where the variable was allocated, either heap_alloc or stack_alloc.
 */
FIRM_API ir_node *new_rd_Free(dbg_info *db, ir_node *block, ir_node *store,
                              ir_node *ptr, ir_node *size, ir_type *free_type,
                              ir_where_alloc where);

/** Constructor for a Sync node.
 *
 * Merges several memory values.  The node assumes that a variable
 * either occurs only in one of the memories, or it contains the same
 * value in all memories where it occurs.
 *
 * @param *db       A pointer for debug information.
 * @param *block    The IR block the node belongs to.
 * @param  arity    The number of memories to synchronize.
 * @param  *in[]    An array of pointers to nodes that produce an output of type
 *                  memory.  The constructor copies this array.
 */
FIRM_API ir_node *new_rd_Sync(dbg_info *db, ir_node *block, int arity,
                              ir_node *in[]);

/** Constructor for a Proj node.
 *
 * Projects a single value out of a tuple.  The parameter proj gives the
 * position of the value within the tuple.
 *
 * @param *db    A pointer for debug information.
 * @param arg    A node producing a tuple.  The node must have mode_T.
 * @param *mode  The mode of the value to project.
 * @param proj   The position of the value in the tuple.
 */
FIRM_API ir_node *new_rd_Proj(dbg_info *db, ir_node *arg, ir_mode *mode,
                              long proj);

/** Constructor for a defaultProj node.
 *
 * Represents the default control flow of a Switch-Cond node.
 *
 * @param *db       A pointer for debug information.
 * @param arg       A node producing a tuple.
 * @param max_proj  The end position of the value in the tuple.
 */
FIRM_API ir_node *new_rd_defaultProj(dbg_info *db, ir_node *arg, long max_proj);

/** Constructor for a Tuple node.
 *
 * This is an auxiliary node to replace a node that returns a tuple
 * without changing the corresponding Proj nodes.
 *
 * @param *db     A pointer for debug information.
 * @param *block  The IR block the node belongs to.
 * @param arity   The number of tuple elements.
 * @param *in[]   An array containing pointers to the nodes producing the tuple
 *                elements. The constructor copies this array.
 */
FIRM_API ir_node *new_rd_Tuple(dbg_info *db, ir_node *block,
                               int arity, ir_node *in[]);

/** Constructor for a Id node.
 *
 * This is an auxiliary node to replace a node that returns a single
 * value.
 *
 * @param *db     A pointer for debug information.
 * @param *block  The IR block the node belongs to.
 * @param *val    The value
 * @param *mode   The mode of *val.
 */
FIRM_API ir_node *new_rd_Id(dbg_info *db, ir_node *block,
                            ir_node *val, ir_mode *mode);

/** Constructor for a Confirm node.
 *
 * Specifies constraints for a value.  To support dataflow analyses.
 *
 * Example: If the value never exceeds '100' this is expressed by placing a
 * Confirm node val = new_d_Confirm(db, val, 100, '<=') on the dataflow edge.
 *
 * @param *db     A pointer for debug information.
 * @param *block  The IR block the node belong to.
 * @param *val    The value we express a constraint for
 * @param *bound  The value to compare against. Must be a firm node, typically a constant.
 * @param cmp     The compare operation.
 */
FIRM_API ir_node *new_rd_Confirm(dbg_info *db, ir_node *block,
                                 ir_node *val, ir_node *bound, pn_Cmp cmp);

/** Constructor for an Unknown node.
 *
 * Represents an arbitrary value.  Places the node in the start block.
 *
 * @param *db     A pointer for debug information.
 * @param *irg    The IR graph the node  belongs to.
 * @param *m      The mode of the unknown value.
 */
FIRM_API ir_node *new_rd_Unknown(dbg_info *db, ir_graph *irg, ir_mode *m);

/** Constructor for a Mux node.
 *
 * @param *db       A pointer for debug information.
 * @param *block    The block the node belong to.
 * @param *sel      The ir_node that calculates the boolean select.
 * @param *ir_true  The ir_node that calculates the true result.
 * @param *ir_false The ir_node that calculates the false result.
 * @param *mode     The mode of the node (and it_true and ir_false).
 */
FIRM_API ir_node *new_rd_Mux(dbg_info *db, ir_node *block, ir_node *sel,
                             ir_node *ir_false, ir_node *ir_true,
                             ir_mode *mode);

/** Constructor for a CopyB node.
 *
 * @param *db         A pointer for debug information.
 * @param *block      The block the node belong to.
 * @param *store      The current memory
 * @param *dst        The ir_node that represents the destination address.
 * @param *src        The ir_node that represents the source address.
 * @param *data_type  The type of the copied data
 */
FIRM_API ir_node *new_rd_CopyB(dbg_info *db, ir_node *block, ir_node *store,
                               ir_node *dst, ir_node *src, ir_type *data_type);

/** Constructor for a InstOf node.
 *
 * A High-Level Type check.
 *
 * @param   *db        A pointer for debug information.
 * @param   *block     The IR block the node belongs to.
 * @param   *store     The memory in which the object the entity should be selected
 *                     from is allocated.
 * @param   *objptr    A pointer to a object of a class type.
 * @param   *type      The type of which objptr must be.
 */
FIRM_API ir_node *new_rd_InstOf(dbg_info *db, ir_node *block, ir_node *store,
                                ir_node *objptr, ir_type *type);

/** Constructor for a Raise node.
 *
 * A High-Level Exception throw.
 *
 * @param *db    A pointer for debug information.
 * @param *block The IR block the node belongs to.
 * @param *store The current memory.
 * @param *obj   A pointer to the Except variable.
 */
FIRM_API ir_node *new_rd_Raise(dbg_info *db, ir_node *block, ir_node *store,
                               ir_node *obj);

/** Constructor for a Bound node.
 *
 * A High-Level bounds check. Checks whether lower <= idx && idx < upper.
 *
 * @param *db         A pointer for debug information.
 * @param *block      The block the node belong to.
 * @param *store      The current memory.
 * @param *idx        The ir_node that represents an index.
 * @param *lower      The ir_node that represents the lower bound for the index.
 * @param *upper      The ir_node that represents the upper bound for the index.
 */
FIRM_API ir_node *new_rd_Bound(dbg_info *db, ir_node *block,
                               ir_node *store, ir_node *idx, ir_node *lower,
                               ir_node *upper);

/** Constructor for a Pin node.
 *
 * @param *db         A pointer for debug information.
 * @param *block      The block the node belong to.
 * @param *node       The node which value should be pinned.
 */
FIRM_API ir_node *new_rd_Pin(dbg_info *db, ir_node *block, ir_node *node);

/** Constructor for an ASM pseudo node.
 *
 * @param *db         A pointer for debug information.
 * @param *block      The block the node belong to.
 * @param arity       The number of data inputs to the node.
 * @param *in         The array of length arity of data inputs.
 * @param *inputs     The array of length arity of input constraints.
 * @param n_outs      The number of data outputs to the node.
 * @param *outputs    The array of length n_outs of output constraints.
 * @param n_clobber   The number of clobbered registers.
 * @param *clobber    The array of length n_clobber of clobbered registers.
 * @param *asm_text   The assembler text.
 */
FIRM_API ir_node *new_rd_ASM(dbg_info *db, ir_node *block,
                            int arity, ir_node *in[], ir_asm_constraint *inputs,
                            int n_outs, ir_asm_constraint *outputs,
                            int n_clobber, ident *clobber[], ident *asm_text);

/*-------------------------------------------------------------------------*/
/* The raw interface without debug support                                 */
/*-------------------------------------------------------------------------*/

/** Constructor for a Block node.
 *
 * Constructs a mature block with the given predecessors.  Use Unknown
 * nodes as predecessors to construct a block if the number of
 * predecessors is known, but not the predecessors themselves.  This
 * constructor does not set current_block.  It not be used with
 * automatic Phi node construction.
 *
 *
 * @param irg    The IR graph the block belongs to.
 * @param arity  The number of control predecessors.
 * @param in[]   An array of control predecessors.  The length of
 *               the array must be 'arity'. The constructor copies this array.
 */
FIRM_API ir_node *new_r_Block(ir_graph *irg, int arity, ir_node *in[]);

/** Constructor for a Start node. */
FIRM_API ir_node *new_r_Start(ir_graph *irg);

/** Constructor for a End node. */
FIRM_API ir_node *new_r_End(ir_graph *irg, int arity, ir_node *in[]);

/** Constructor for a Jmp node.
 *
 * Jmp represents control flow to a single control successor.
 *
 * @param *block  The IR block the node belongs to.
 */
FIRM_API ir_node *new_r_Jmp(ir_node *block);

/** Constructor for an IJmp node.
 *
 * IJmp represents control flow to a single control successor not
 * statically known i.e. an indirect Jmp.
 *
 * @param *block  The IR block the node belongs to.
 * @param *tgt    The IR node representing the target address.
 */
FIRM_API ir_node *new_r_IJmp(ir_node *block, ir_node *tgt);

/** Constructor for a Cond node.
 *
 * If c is mode_b represents a conditional branch (if/else). If c is
 * mode_Is/mode_Iu (?) represents a switch.  (Allocates dense Cond
 * node, default Proj is 0.)
 *
 * This is not consistent:  Input to Cond is Is, Proj has as proj number
 * longs.
 *
 * @param *block The IR block the node belongs to.
 * @param *c     The conditions parameter.Can be of mode b or I_u.
 */
FIRM_API ir_node *new_r_Cond(ir_node *block, ir_node *c);

/** Constructor for a Return node.
 *
 * Returns the memory and zero or more return values.  Only node that
 * can end regular control flow.
 *
 * @param *block The IR block the node belongs to.
 * @param *store The state of memory.
 * @param arity  Number of array indices.
 * @param *in[]  Array with index inputs to the node. The constructor copies this array.
 */
FIRM_API ir_node *new_r_Return(ir_node *block, ir_node *store,
                               int arity, ir_node *in[]);

/** Constructor for a Const node.
 *
 * Adds the node to the start block.
 *
 * Constructor for a Const node. The constant represents a target
 * value.  Sets the type information to type_unknown.  (No more
 * supported: If tv is entity derives a somehow useful type.)
 * Derives mode from passed tarval.
 *
 * @param *irg   The IR graph the node  belongs to.
 * @param *con   Points to an entry in the constant table.
 */
FIRM_API ir_node *new_r_Const(ir_graph *irg, ir_tarval *con);

/** Constructor for a Const node.
 *
 * Adds the node to the start block.
 *
 * Constructor for a Const node. The constant represents a target
 * value.  Sets the type information to type_unknown.  (No more
 * supported: If tv is entity derives a somehow useful type.)
 *
 * @param *irg   The IR graph the node  belongs to.
 * @param *mode  The mode of the operands and the results.
 * @param value  A value from which the tarval is made.
 */
FIRM_API ir_node *new_r_Const_long(ir_graph *irg, ir_mode *mode, long value);

/** Constructor for a SymConst node.
 *
 *  This is the constructor for a symbolic constant.
 *    There are several kinds of symbolic constants:
 *    - symconst_type_tag   The symbolic constant represents a type tag.  The
 *                          type the tag stands for is given explicitly.
 *    - symconst_type_size  The symbolic constant represents the size of a type.
 *                          The type of which the constant represents the size
 *                          is given explicitly.
 *    - symconst_type_align The symbolic constant represents the alignment of a
 *                          type.  The type of which the constant represents the
 *                          size is given explicitly.
 *    - symconst_addr_ent   The symbolic constant represents the address of an
 *                          entity (variable or method).  The variable is given
 *                          explicitly by a firm entity.
 *    - symconst_ofs_ent    The symbolic constant represents the offset of an
 *                          entity in its owner type.
 *    - symconst_enum_const The symbolic constant is a enumeration constant of
 *                          an enumeration type.
 *
 *    Inputs to the node:
 *      No inputs except the block it belongs to.
 *    Outputs of the node.
 *      An unsigned integer (I_u) or a pointer (P).
 *
 *    Mention union in declaration so that the firmjni generator recognizes that
 *    it can not cast the argument to an int.
 *
 * @param *irg    The IR graph the node  belongs to.
 * @param mode    The mode for the SymConst.
 * @param value   A type, ident, entity or enum constant depending on the
 *                SymConst kind.
 * @param kind    The kind of the symbolic constant, see the list above
 */
FIRM_API ir_node *new_r_SymConst(ir_graph *irg, ir_mode *mode,
                                 union symconst_symbol value,
                                 symconst_kind kind);

/** Constructor for a simpleSel node.
 *
 *  This is a shortcut for the new_d_Sel() constructor.  To be used for
 *  Sel nodes that do not select from an array, i.e., have no index
 *  inputs.  It adds the two parameters 0, NULL.
 *
 * @param *block     The IR block the node belongs to.
 * @param *store     The memory in which the object the entity should be selected
 *                   from is allocated.
 * @param *objptr    The object from that the Sel operation selects a
 *                   single attribute out.
 * @param *ent       The entity to select.
 */
FIRM_API ir_node *new_r_simpleSel(ir_node *block, ir_node *store,
                                  ir_node *objptr, ir_entity *ent);

/** Constructor for a Sel node.
 *
 * The select node selects an entity (field or method) from an entity
 * with a compound type.  It explicitly specifies the entity selected.
 * Dynamically the node may select entities that overwrite the given
 * entity.  If the selected entity is an array element entity the Sel
 * node takes the required array indices as inputs.
 *
 * @param *block     The IR block the node belongs to.
 * @param *store     The memory in which the object the entity should be selected
 *                   from is allocated.
 * @param *objptr    A pointer to a compound entity the Sel operation selects a
 *                   single attribute from.
 * @param *n_index   The number of array indices needed to select an array element entity.
 * @param *index[]   If the compound entity is an array the indices of the selected
 *                   element entity.  The constructor copies this array.
 * @param *ent       The entity to select.
 */
FIRM_API ir_node *new_r_Sel(ir_node *block, ir_node *store,
                            ir_node *objptr, int n_index, ir_node *index[],
                            ir_entity *ent);

/** Constructor for a Call node.
 *
 * Represents all kinds of method and function calls.
 *
 * @param *block  The IR block the node belongs to.
 * @param *store  The actual store.
 * @param *callee A pointer to the called procedure.
 * @param arity   The number of procedure parameters.
 * @param *in[]   An array with the pointers to the parameters. The constructor copies this array.
 * @param *tp     Type information of the procedure called.
 */
FIRM_API ir_node *new_r_Call(ir_node *block, ir_node *store,
                             ir_node *callee, int arity, ir_node *in[],
                             ir_type *tp);

/** Constructor for a Builtin node.
 *
 * Represents a call of a backend-specific builtin..
 *
 * @param *block  The IR block the node belongs to.
 * @param *store  The actual store.
 * @param arity   The number of procedure parameters.
 * @param *in[]   An array with the pointers to the parameters. The constructor copies this array.
 * @param kind    The kind of the called builtin.
 * @param *tp     Type information of the procedure called.
 */
FIRM_API ir_node *new_r_Builtin(ir_node *block, ir_node *store,
                                int arity, ir_node *in[], ir_builtin_kind kind,
                                ir_type *tp);

/** Constructor for a Add node.
 *
 * @param *block The IR block the node belongs to.
 * @param *op1   The first operand.
 * @param *op2   The second operand.
 * @param *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_r_Add(ir_node *block, ir_node *op1, ir_node *op2,
                            ir_mode *mode);

/**
 * Constructor for a Sub node.
 *
 * @param *block The IR block the node belongs to.
 * @param *op1   The first operand.
 * @param *op2   The second operand.
 * @param *mode  The mode of the operands and the results.
 */
FIRM_API ir_node *new_r_Sub(ir_node *block, ir_node *op1, ir_node *op2,
                            ir_mode *mode);

/** Constructor for a Minus node.
 *
 * @param *block The IR block the node belongs to.
 * @param *op    The operand.
 * @param *mode  The mode of the operand and the result.
 */
FIRM_API ir_node *new_r_Minus(ir_node *block, ir_node *op, ir_mode *mode);

/** Constructor for a Mul node.
 *
 * @param *block The IR block the node belongs to.
 * @param *op1   The first operand.
 * @param *op2   The second operand.
 * @param *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_r_Mul(ir_node *block, ir_node *op1, ir_node *op2,
                            ir_mode *mode);

/** Constructor for a Mulh node.
 *
 * @param *block The IR block the node belongs to.
 * @param *op1   The first operand.
 * @param *op2   The second operand.
 * @param *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_r_Mulh(ir_node *block, ir_node *op1, ir_node *op2,
                             ir_mode *mode);

/** Constructor for a Quot node.
 *
 * @param *block The IR block the node belongs to.
 * @param *memop The store needed to model exceptions
 * @param *op1   The first operand.
 * @param *op2   The second operand.
 * @param *mode  The mode of the result.
 * @param state  The pinned state.
 */
FIRM_API ir_node *new_r_Quot(ir_node *block, ir_node *memop,
                             ir_node *op1, ir_node *op2, ir_mode *mode,
                             op_pin_state state);

/** Constructor for a DivMod node.
 *
 * @param *block The IR block the node belongs to.
 * @param *memop The store needed to model exceptions
 * @param *op1   The first operand.
 * @param *op2   The second operand.
 * @param *mode  The mode of the results.
 * @param state  The pinned state.
 */
FIRM_API ir_node *new_r_DivMod(ir_node *block, ir_node *memop,
                               ir_node *op1, ir_node *op2, ir_mode *mode,
                               op_pin_state state);

/** Constructor for a Div node.
 *
 * @param *block The IR block the node belongs to.
 * @param *memop The store needed to model exceptions
 * @param *op1   The first operand.
 * @param *op2   The second operand.
 * @param *mode  The mode of the result.
 * @param state  The pinned state.
 */
FIRM_API ir_node *new_r_Div(ir_node *block, ir_node *memop,
                            ir_node *op1, ir_node *op2, ir_mode *mode,
                            op_pin_state state);

/** Constructor for a remainderless Div node.
 *
 * @param *block The IR block the node belongs to.
 * @param *memop The store needed to model exceptions
 * @param *op1   The first operand.
 * @param *op2   The second operand.
 * @param *mode  The mode of the result.
 * @param state  The pinned state.
 */
FIRM_API ir_node *new_r_DivRL(ir_node *block, ir_node *memop,
                              ir_node *op1, ir_node *op2, ir_mode *mode,
                              op_pin_state state);

/** Constructor for a Mod node.
 *
 * @param *block The IR block the node belongs to.
 * @param *memop The store needed to model exceptions
 * @param *op1   The first operand.
 * @param *op2   The second operand.
 * @param *mode  The mode of the result.
 * @param state  The pinned state.
 */
FIRM_API ir_node *new_r_Mod(ir_node *block, ir_node *memop,
                            ir_node *op1, ir_node *op2, ir_mode *mode,
                            op_pin_state state);

/** Constructor for a And node.
 *
 * @param *block The IR block the node belongs to.
 * @param *op1   The first operand.
 * @param *op2   The second operand.
 * @param *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_r_And(ir_node *block, ir_node *op1, ir_node *op2,
                            ir_mode *mode);

/** Constructor for a Or node.
 *
 * @param *block The IR block the node belongs to.
 * @param *op1   The first operand.
 * @param *op2   The second operand.
 * @param *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_r_Or(ir_node *block, ir_node *op1, ir_node *op2,
                           ir_mode *mode);

/** Constructor for a Eor node.
 *
 * @param *block The IR block the node belongs to.
 * @param *op1   The first operand.
 * @param *op2   The second operand.
 * @param *mode  The mode of the operands and the results.
 */
FIRM_API ir_node *new_r_Eor(ir_node *block, ir_node *op1, ir_node *op2,
                            ir_mode *mode);

/** Constructor for a Not node.
 *
 * @param *block The IR block the node belongs to.
 * @param *op    The operand.
 * @param *mode  The mode of the operand and the result.
 */
FIRM_API ir_node *new_r_Not(ir_node *block, ir_node *op, ir_mode *mode);

/** Constructor for a Cmp node.
 *
 * @param *block The IR block the node belongs to.
 * @param *op1   The first operand.
 * @param *op2   The second operand.
 */
FIRM_API ir_node *new_r_Cmp(ir_node *block, ir_node *op1, ir_node *op2);

/** Constructor for a Shl node.
 *
 * @param   *block The IR block the node belongs to.
 * @param   *op    The operand.
 * @param   *k     The number of bits to  shift the operand .
 * @param   *mode  The mode of the operand and the result.
 */
FIRM_API ir_node *new_r_Shl(ir_node *block, ir_node *op, ir_node *k,
                            ir_mode *mode);

/** Constructor for a Shr node.
 *
 * @param *block The IR block the node belongs to.
 * @param *op    The operand.
 * @param *k     The number of bits to shift the operand .
 * @param *mode  The mode of the operand and the result.
 */
FIRM_API ir_node *new_r_Shr(ir_node *block, ir_node *op, ir_node *k,
                            ir_mode *mode);

/**
 * Constructor for a Shrs node.
 *
 * @param  *block The IR block the node belongs to.
 * @param  *op    The operand.
 * @param  *k     The number of bits to shift the operand.
 * @param  *mode  The mode of the operand and the result.
 */
FIRM_API ir_node *new_r_Shrs(ir_node *block, ir_node *op, ir_node *k,
                             ir_mode *mode);

/** Constructor for a Rotl node.
 *
 * @param *block The IR block the node belongs to.
 * @param *op    The operand.
 * @param *k     The number of bits to rotate the operand.
 * @param *mode  The mode of the operand.
 */
FIRM_API ir_node *new_r_Rotl(ir_node *block, ir_node *op, ir_node *k,
                             ir_mode *mode);

/** Constructor for a Conv node.
 *
 * @param *block The IR block the node belongs to.
 * @param *op    The operand.
 * @param *mode  The mode of this the operand muss be converted .
 */
FIRM_API ir_node *new_r_Conv(ir_node *block, ir_node *op, ir_mode *mode);

/** Constructor for a strict Conv node.
 *
 * @param *block The IR block the node belongs to.
 * @param *op    The operand.
 * @param *mode  The mode of this the operand muss be converted .
 */
FIRM_API ir_node *new_r_strictConv(ir_node *block, ir_node *op, ir_mode *mode);

/** Constructor for a Cast node.
 *
 * High level type cast
 *
 * @param *block The IR block the node belongs to.
 * @param *op    The operand.
 * @param *to_tp The type of this the operand muss be casted .
 */
FIRM_API ir_node *new_r_Cast(ir_node *block, ir_node *op, ir_type *to_tp);

/** Constructor for a Carry node.
 *
 * @param *block The IR block the node belongs to.
 * @param *op1   The first operand.
 * @param *op2   The second operand.
 * @param *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_r_Carry(ir_node *block, ir_node *op1, ir_node *op2,
                              ir_mode *mode);

/**
 * Constructor for a Borrow node.
 *
 * @param *block The IR block the node belongs to.
 * @param *op1   The first operand.
 * @param *op2   The second operand.
 * @param *mode  The mode of the operands and the results.
 */
FIRM_API ir_node *new_r_Borrow(ir_node *block, ir_node *op1, ir_node *op2,
                               ir_mode *mode);

/** Constructor for a Phi node.
 *
 * @param *block The IR block the node belongs to.
 * @param arity  The number of predecessors
 * @param *in[]  Array with predecessors. The constructor copies this array.
 * @param *mode  The mode of it's inputs and output.
 */
FIRM_API ir_node *new_r_Phi(ir_node *block, int arity, ir_node *in[],
                            ir_mode *mode);

/** Constructor for a Load node.
 *
 * @param *block The IR block the node belongs to.
 * @param *store The current memory
 * @param *adr   A pointer to the variable to be read in this memory.
 * @param *mode  The mode of the value to be loaded.
 * @param  flags Additional flags for alignment, volatility and pin state.
 */
FIRM_API ir_node *new_r_Load(ir_node *block, ir_node *store,
                             ir_node *adr, ir_mode *mode, ir_cons_flags flags);

/** Constructor for a Store node.
 *
 * @param *block The IR block the node belongs to.
 * @param *store The current memory
 * @param *adr   A pointer to the variable to be read in this memory.
 * @param *val   The value to write to this variable.
 * @param  flags Additional flags for alignment, volatility and pin state.
 */
FIRM_API ir_node *new_r_Store(ir_node *block, ir_node *store,
                              ir_node *adr, ir_node *val, ir_cons_flags flags);

/** Constructor for a Alloc node.
 *
 * The Alloc node extends the memory by space for an entity of type alloc_type.
 *
 * @param *block      The IR block the node belongs to.
 * @param *store      The memory which shall contain the new variable.
 * @param *count      The number of objects to allocate.
 * @param *alloc_type The type of the allocated variable.
 * @param where       Where to allocate the variable, either heap_alloc or stack_alloc.
 */
FIRM_API ir_node *new_r_Alloc(ir_node *block, ir_node *store,
                              ir_node *count, ir_type *alloc_type,
                              ir_where_alloc where);

/** Constructor for a Free node.
 *
 * Frees the memory occupied by the entity pointed to by the pointer
 * arg.  Type indicates the type of the entity the argument points to.
 *
 * @param *block      The IR block the node belongs to.
 * @param *store      The memory which shall contain the new variable.
 * @param *ptr        The pointer to the object to free.
 * @param *size       The number of objects of type free_type to free in a sequence.
 * @param *free_type  The type of the freed variable.
 * @param where       Where the variable was allocated, either heap_alloc or stack_alloc.
 */
FIRM_API ir_node *new_r_Free(ir_node *block, ir_node *store, ir_node *ptr,
                             ir_node *size, ir_type *free_type,
                             ir_where_alloc where);

/** Constructor for a Sync node.
 *
 * Merges several memory values.  The node assumes that a variable
 * either occurs only in one of the memories, or it contains the same
 * value in all memories where it occurs.
 *
 * @param *block   The IR block the node belongs to.
 * @param arity    The number of memories to synchronize.
 * @param *in[]    An array of pointers to nodes that produce an output of  type memory.
 *                 The constructor copies this array.
 */
FIRM_API ir_node *new_r_Sync(ir_node *block, int arity, ir_node *in[]);

/** Constructor for a Proj node.
 *
 * Projects a single value out of a tuple.  The parameter proj gives the
 * position of the value within the tuple.
 *
 * @param arg    A node producing a tuple.
 * @param mode   The mode of the value to project.
 * @param proj   The position of the value in the tuple.
 */
FIRM_API ir_node *new_r_Proj(ir_node *arg, ir_mode *mode, long proj);

/** Constructor for a defaultProj node.
 *
 * Represents the default control flow of a Switch-Cond node.
 *
 * @param arg       A node producing a tuple.
 * @param max_proj  The end  position of the value in the tuple.
 */
FIRM_API ir_node *new_r_defaultProj(ir_node *arg, long max_proj);


/** Constructor for a Tuple node.
 *
 * This is an auxiliary node to replace a node that returns a tuple
 * without changing the corresponding Proj nodes.
 *
 * @param *block  The IR block the node belongs to.
 * @param arity   The number of tuple elements.
 * @param *in[]   An array containing pointers to the nodes producing the tuple elements.
 *                The constructor copies this array.
 */
FIRM_API ir_node *new_r_Tuple(ir_node *block, int arity, ir_node *in[]);

/** Constructor for a Id node.
 *
 * This is an auxiliary node to replace a node that returns a single
 * value.
 *
 * @param *block  The IR block the node belongs to.
 * @param *val    The operand to Id.
 * @param *mode   The mode of *val.
 */
FIRM_API ir_node *new_r_Id(ir_node *block, ir_node *val, ir_mode *mode);

/** Constructor for a Bad node.
 *
 * Returns the unique Bad node of the graph.  The same as
 * get_irg_bad().
 *
 * @param *irg    The IR graph the node  belongs to.
 */
FIRM_API ir_node *new_r_Bad(ir_graph *irg);

/** Constructor for a Confirm node.
 *
 * Specifies constraints for a value.  To support dataflow analyses.
 *
 * Example: If the value never exceeds '100' this is expressed by placing a
 * Confirm node val = new_d_Confirm(db, val, 100, '<=') on the dataflow edge.
 *
 * @param *block  The IR block the node belong to.
 * @param *val    The value we express a constraint for
 * @param *bound  The value to compare against. Must be a firm node, typically a constant.
 * @param cmp     The compare operation.
 */
FIRM_API ir_node *new_r_Confirm(ir_node *block, ir_node *val, ir_node *bound,
                                pn_Cmp cmp);

/** Constructor for a Unknown node.
 *
 * Represents an arbitrary value.  Places the node in
 * the start block.
 *
 * @param *irg    The IR graph the node  belongs to.
 * @param *m      The mode of the unknown value.
 */
FIRM_API ir_node *new_r_Unknown(ir_graph *irg, ir_mode *m);

/** Constructor for a NoMem node.
 *
 * Returns the unique NoMem node of the graph.  The same as
 * get_irg_no_mem().
 *
 * @param *irg    The IR graph the node belongs to.
 */
FIRM_API ir_node *new_r_NoMem(ir_graph *irg);

/** Constructor for a Mux node.
 *
 * @param *block    The block the node belong to.
 * @param *sel      The ir_node that calculates the boolean select.
 * @param *ir_true  The ir_node that calculates the true result.
 * @param *ir_false The ir_node that calculates the false result.
 * @param *mode     The mode of the node (and it_true and ir_false).
 */
FIRM_API ir_node *new_r_Mux(ir_node *block, ir_node *sel,
                            ir_node *ir_false, ir_node *ir_true, ir_mode *mode);

/** Constructor for a CopyB node.
 *
 * @param *block      The block the node belong to.
 * @param *store      The current memory
 * @param *dst        The ir_node that represents the destination address.
 * @param *src        The ir_node that represents the source address.
 * @param *data_type  The type of the copied data
 */
FIRM_API ir_node *new_r_CopyB(ir_node *block, ir_node *store,
                              ir_node *dst, ir_node *src, ir_type *data_type);

/** Constructor for a InstOf node.
 *
 * A High-Level Type check.
 *
 * @param *block     The block the node belong to.
 * @param *store     The memory in which the object the entity should be selected
 *                   from is allocated.
 * @param *objptr    A pointer to a object of a class type.
 * @param *type      The type of which objptr must be.
 */
FIRM_API ir_node *new_r_InstOf(ir_node *block, ir_node *store,
                               ir_node *objptr, ir_type *type);

/** Constructor for a Raise node.
 *
 * A High-Level Exception throw.
 *
 * @param *block The IR block the node belongs to.
 * @param *store The current memory.
 * @param *obj   A pointer to the Except variable.
 */
FIRM_API ir_node *new_r_Raise(ir_node *block, ir_node *store, ir_node *obj);

/** Constructor for a Bound node.
 *
 * A High-Level bounds check. Checks whether lower <= idx && idx < upper.
 *
 * @param *block      The block the node belong to.
 * @param *store      The current memory.
 * @param *idx        The ir_node that represents an index.
 * @param *lower      The ir_node that represents the lower bound for the index.
 * @param *upper      The ir_node that represents the upper bound for the index.
 */
FIRM_API ir_node *new_r_Bound(ir_node *block, ir_node *store,
                              ir_node *idx, ir_node *lower, ir_node *upper);

/** Constructor for a Pin node.
 *
 * @param *block      The block the node belong to.
 * @param *node       The node which value should be pinned.
 */
FIRM_API ir_node *new_r_Pin(ir_node *block, ir_node *node);

/** Constructor for an ASM pseudo node.
 *
 * @param *block      The block the node belong to.
 * @param arity       The number of data inputs to the node.
 * @param *in         The array of length arity of data inputs.
 * @param *inputs     The array of length arity of input constraints.
 * @param n_outs      The number of data outputs to the node.
 * @param *outputs    The array of length n_outs of output constraints.
 * @param n_clobber   The number of clobbered registers.
 * @param *clobber    The array of length n_clobber of clobbered registers.
 * @param *asm_text   The assembler text.
 */
FIRM_API ir_node *new_r_ASM(ir_node *block,
                            int arity, ir_node *in[], ir_asm_constraint *inputs,
                            int n_outs, ir_asm_constraint *outputs,
                            int n_clobber, ident *clobber[], ident *asm_text);

/*-----------------------------------------------------------------------*/
/* The block oriented interface                                          */
/*-----------------------------------------------------------------------*/

/** Sets the current block in which the following constructors place the
 *  nodes they construct.
 *
 *  @param target  The new current block.
 */
FIRM_API void set_cur_block(ir_node *target);
FIRM_API void set_r_cur_block(ir_graph *irg, ir_node *target);

/** Returns the current block of the current graph. */
FIRM_API ir_node *get_cur_block(void);
FIRM_API ir_node *get_r_cur_block(ir_graph *irg);

/** Constructor for a Block node.
 *
 * Adds the block to the graph in current_ir_graph. Constructs a Block
 * with a fixed number of predecessors.
 *
 * @param *db    A Pointer for debug information.
 * @param arity  The number of control predecessors.
 * @param in[]   An array of control predecessors.  The length of
 *               the array must be 'arity'.
 */
FIRM_API ir_node *new_d_Block(dbg_info *db, int arity, ir_node *in[]);

/** Constructor for a Start node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param *db    A pointer for debug information.
 */
FIRM_API ir_node *new_d_Start(dbg_info *db);

/** Constructor for a End node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param *db     A pointer for debug information.
 */
FIRM_API ir_node *new_d_End(dbg_info *db, int arity, ir_node *in[]);

/** Constructor for a Jmp node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * Jmp represents control flow to a single control successor.
 *
 * @param *db     A pointer for debug information.
 */
FIRM_API ir_node *new_d_Jmp(dbg_info *db);

/** Constructor for an IJmp node.
 *
 * IJmp represents control flow to a single control successor not
 * statically known i.e. an indirect Jmp.
 *
 * @param *db     A pointer for debug information.
 * @param *tgt    The IR node representing the target address.
 */
FIRM_API ir_node *new_d_IJmp(dbg_info *db, ir_node *tgt);

/** Constructor for a Cond node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * If c is mode_b represents a conditional branch (if/else). If c is
 * mode_Is/mode_Iu (?) represents a switch.  (Allocates dense Cond
 * node, default Proj is 0.)
 *
 * This is not consistent:  Input to Cond is Is, Proj has as proj number
 * longs.
 *
 * @param *db    A pointer for debug information.
 * @param *c     The conditions parameter.Can be of mode b or I_u.
 */
FIRM_API ir_node *new_d_Cond(dbg_info *db, ir_node *c);

/** Constructor for a Return node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * Returns the memory and zero or more return values.  Only node that
 * can end regular control flow.
 *
 * @param *db    A pointer for debug information.
 * @param *store The state of memory.
 * @param arity  Number of array indices.
 * @param *in    Array with index inputs to the node.
 */
FIRM_API ir_node *new_d_Return(dbg_info *db, ir_node *store,
                               int arity, ir_node *in[]);

/** Constructor for a Const node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * Constructor for a Const node. The constant represents a target
 * value.  Sets the type information to type_unknown.  (No more
 * supported: If tv is entity derives a somehow useful type.)
 * Derives mode from passed tarval.
 *
 * @param *db    A pointer for debug information.
 * @param *con   Points to an entry in the constant table. This pointer is added
 *               to the attributes of the node.
 */
FIRM_API ir_node *new_d_Const(dbg_info *db, ir_tarval *con);

/**
 * @see new_rd_Const_long()
 *
 * @param *db    A pointer for debug information.
 * @param *mode  The mode of the operands and results.
 * @param value  A value from which the tarval is made.
 */
FIRM_API ir_node *new_d_Const_long(dbg_info *db, ir_mode *mode, long value);

/** Constructor for a SymConst node.
 *
 *  This is the constructor for a symbolic constant.
 *    There are several kinds of symbolic constants:
 *    - symconst_type_tag   The symbolic constant represents a type tag.  The
 *                          type the tag stands for is given explicitly.
 *    - symconst_type_size  The symbolic constant represents the size of a type.
 *                          The type of which the constant represents the size
 *                          is given explicitly.
 *    - symconst_type_align The symbolic constant represents the alignment of a
 *                          type.  The type of which the constant represents the
 *                          size is given explicitly.
 *    - symconst_addr_ent   The symbolic constant represents the address of an
 *                          entity (variable or method).  The variable is given
 *                          explicitly by a firm entity.
 *    - symconst_ofs_ent    The symbolic constant represents the offset of an
 *                          entity in its owner type.
 *    - symconst_enum_const The symbolic constant is a enumeration constant of
 *                          an enumeration type.
 *
 *    Inputs to the node:
 *      No inputs except the block it belongs to.
 *    Outputs of the node.
 *      An unsigned integer (I_u) or a pointer (P).
 *
 *    Mention union in declaration so that the firmjni generator recognizes that
 *    it can not cast the argument to an int.
 *
 * @param *db     A pointer for debug information.
 * @param mode    The mode for the SymConst.
 * @param value   A type, ident, entity or enum constant depending on the
 *                SymConst kind.
 * @param kind    The kind of the symbolic constant, see the list above
 */
FIRM_API ir_node *new_d_SymConst(dbg_info *db, ir_mode *mode,
                                 union symconst_symbol value,
                                 symconst_kind kind);

/** Constructor for a simpleSel node.
 *
 *  This is a shortcut for the new_d_Sel() constructor.  To be used for
 *  Sel nodes that do not select from an array, i.e., have no index
 *  inputs.  It adds the two parameters 0, NULL.
 *
 * @param   *db        A pointer for debug information.
 * @param   *store     The memory in which the object the entity should be
 *                     selected from is allocated.
 * @param   *objptr    The object from that the Sel operation selects a
 *                     single attribute out.
 * @param   *ent       The entity to select.
 */
FIRM_API ir_node *new_d_simpleSel(dbg_info *db, ir_node *store, ir_node *objptr,
                                  ir_entity *ent);

/** Constructor for a Sel node.
 *
 * The select node selects an entity (field or method) from an entity
 * with a compound type.  It explicitly specifies the entity selected.
 * Dynamically the node may select entities that overwrite the given
 * entity.  If the selected entity is an array element entity the Sel
 * node takes the required array indices as inputs.
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db        A pointer for debug information.
 * @param   *store     The memory in which the object the entity should be selected
 *                     from is allocated.
 * @param   *objptr    A pointer to a compound entity the Sel operation selects a
 *                     single attribute from.
 * @param   arity      The number of array indices needed to select an array element entity.
 * @param   *in[]      If the compound entity is an array the indices of the selected
 *                     element entity.  The constructor copies this array.
 * @param   *ent       The entity to select.
 */
FIRM_API ir_node *new_d_Sel(dbg_info *db, ir_node *store, ir_node *objptr,
                            int arity, ir_node *in[], ir_entity *ent);

/** Constructor for a Call node.
 *
 * Represents all kinds of method and function calls.
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db     A pointer for debug information.
 * @param   *store  The actual store.
 * @param   *callee A pointer to the called procedure.
 * @param   arity   The number of procedure parameters.
 * @param   *in[]   An array with the pointers to the parameters. The constructor copies this array.
 * @param   *tp     Type information of the procedure called.
 */
FIRM_API ir_node *new_d_Call(dbg_info *db, ir_node *store, ir_node *callee,
                             int arity, ir_node *in[], ir_type *tp);

/** Constructor for a Builtin node.
 *
 * Represents a call of a backend-specific builtin..
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db     A pointer for debug information.
 * @param   *store  The actual store.
 * @param   arity   The number of procedure parameters.
 * @param   *in[]   An array with the pointers to the parameters. The constructor copies this array.
 * @param   kind    The kind of the called builtin.
 * @param   *tp     Type information of the procedure called.
 */
FIRM_API ir_node *new_d_Builtin(dbg_info *db, ir_node *store,
                                int arity, ir_node *in[],
                                ir_builtin_kind kind, ir_type *tp);

/** Constructor for a Add node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db    A pointer for debug information.
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_d_Add(dbg_info *db, ir_node *op1, ir_node *op2,
                            ir_mode *mode);

/** Constructor for a Sub node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db    A pointer for debug information.
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_d_Sub(dbg_info *db, ir_node *op1, ir_node *op2,
                            ir_mode *mode);

/** Constructor for a Minus node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db    A pointer for debug information.
 * @param   *op    The operand .
 * @param   *mode  The mode of the operand and the result.
 */
FIRM_API ir_node *new_d_Minus(dbg_info *db, ir_node *op,  ir_mode *mode);

/** Constructor for a Mul node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db    A pointer for debug information.
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_d_Mul(dbg_info *db, ir_node *op1, ir_node *op2,
                            ir_mode *mode);

/** Constructor for a Mulh node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db    A pointer for debug information.
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_d_Mulh(dbg_info *db, ir_node *op1, ir_node *op2,
                             ir_mode *mode);

/** Constructor for a Quot node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db    A pointer for debug information.
 * @param   *memop The store needed to model exceptions
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the result.
 * @param   state  The pinned state.
 */
FIRM_API ir_node *new_d_Quot(dbg_info *db, ir_node *memop,
                             ir_node *op1, ir_node *op2, ir_mode *mode,
                             op_pin_state state);

/** Constructor for a DivMod node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db    A pointer for debug information.
 * @param   *memop The store needed to model exceptions
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the results.
 * @param   state  The pinned state.
 */
FIRM_API ir_node *new_d_DivMod(dbg_info *db, ir_node *memop, ir_node *op1,
                               ir_node *op2, ir_mode *mode, op_pin_state state);

/** Constructor for a Div node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db    A pointer for debug information.
 * @param   *memop The store needed to model exceptions
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the result.
 * @param   state  The pinned state.
 */
FIRM_API ir_node *new_d_Div(dbg_info *db, ir_node *memop, ir_node *op1,
                            ir_node *op2, ir_mode *mode, op_pin_state state);

/** Constructor for a remainderless Div node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db    A pointer for debug information.
 * @param   *memop The store needed to model exceptions
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the result.
 * @param   state  The pinned state.
 */
FIRM_API ir_node *new_d_DivRL(dbg_info *db, ir_node *memop,
                              ir_node *op1, ir_node *op2, ir_mode *mode,
                              op_pin_state state);

/** Constructor for a Mod node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db    A pointer for debug information.
 * @param   *memop The store needed to model exceptions
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the result.
 * @param   state  The pinned state.
 */
FIRM_API ir_node *new_d_Mod(dbg_info *db, ir_node *memop,
                            ir_node *op1, ir_node *op2, ir_mode *mode,
                            op_pin_state state);

/** Constructor for a And node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db    A pointer for debug information.
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_d_And(dbg_info *db, ir_node *op1, ir_node *op2,
                            ir_mode *mode);

/** Constructor for a Or node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db    A pointer for debug information.
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_d_Or(dbg_info *db, ir_node *op1, ir_node *op2,
                           ir_mode *mode);

/** Constructor for a Eor node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db    A pointer for debug information.
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the results.
 */
FIRM_API ir_node *new_d_Eor(dbg_info *db, ir_node *op1, ir_node *op2,
                            ir_mode *mode);

/** Constructor for a Not node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db    A pointer for debug information.
 * @param   *op    The operand.
 * @param   *mode  The mode of the operand and the result.
 */
FIRM_API ir_node *new_d_Not(dbg_info *db, ir_node *op, ir_mode *mode);

/** Constructor for a Shl node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db    A pointer for debug information.
 * @param   *op    The operand.
 * @param   *k     The number of bits to  shift the operand .
 * @param   *mode  The mode of the operand and the result.
 */
FIRM_API ir_node *new_d_Shl(dbg_info *db, ir_node *op, ir_node *k,
                            ir_mode *mode);

/** Constructor for a Shr node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db    A pointer for debug information.
 * @param   *op    The operand.
 * @param   *k     The number of bits to  shift the operand .
 * @param   *mode  The mode of the operand and the result.
 */
FIRM_API ir_node *new_d_Shr(dbg_info *db, ir_node *op, ir_node *k,
                            ir_mode *mode);

/** Constructor for a Shrs node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db    A pointer for debug information.
 * @param   *op    The operand.
 * @param   *k     The number of bits to  shift the operand .
 * @param   *mode  The mode of the operand and the result.
 */
FIRM_API ir_node *new_d_Shrs(dbg_info *db, ir_node *op, ir_node *k,
                             ir_mode *mode);

/** Constructor for a Rotl node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db    A pointer for debug information.
 * @param   *op    The operand.
 * @param   *k     The number of bits to rotate the operand.
 * @param   *mode  The mode of the operand.
 */
FIRM_API ir_node *new_d_Rotl(dbg_info *db, ir_node *op, ir_node *k,
                             ir_mode *mode);

/** Constructor for a Cmp node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db    A pointer for debug information.
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 */
FIRM_API ir_node *new_d_Cmp(dbg_info *db, ir_node *op1, ir_node *op2);

/** Constructor for a Conv node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db    A pointer for debug information.
 * @param   *op    The operand.
 * @param   *mode  The mode of this the operand muss be converted .
 */
FIRM_API ir_node *new_d_Conv(dbg_info *db, ir_node *op, ir_mode *mode);

/** Constructor for a strict Conv node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db    A pointer for debug information.
 * @param   *op    The operand.
 * @param   *mode  The mode of this the operand muss be converted .
 */
FIRM_API ir_node *new_d_strictConv(dbg_info *db, ir_node *op, ir_mode *mode);

/** Constructor for a Cast node.
 *
 * High level type cast
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db    A pointer for debug information.
 * @param   *op    The operand.
 * @param   *to_tp The type of this the operand muss be casted .
 */
FIRM_API ir_node *new_d_Cast(dbg_info *db, ir_node *op, ir_type *to_tp);

/** Constructor for a Carry node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db    A pointer for debug information.
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_d_Carry(dbg_info *db, ir_node *op1, ir_node *op2,
                              ir_mode *mode);

/** Constructor for a Borrow node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *db    A pointer for debug information.
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_d_Borrow(dbg_info *db, ir_node *op1, ir_node *op2,
                               ir_mode *mode);

/** Constructor for a Phi node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param *db    A pointer for debug information.
 * @param arity  The number of predecessors
 * @param *in    Array with predecessors
 * @param *mode  The mode of it's inputs and output.
 */
FIRM_API ir_node *new_d_Phi(dbg_info *db, int arity, ir_node *in[],
                            ir_mode *mode);

/** Constructor for a Load node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param *db    A pointer for debug information.
 * @param *store The current memory
 * @param *addr  A pointer to the variable to be read in this memory.
 * @param *mode  The mode of the value to be loaded.
 * @param  flags Additional flags for alignment, volatility and pin state.
 */
FIRM_API ir_node *new_d_Load(dbg_info *db, ir_node *store, ir_node *addr,
                             ir_mode *mode, ir_cons_flags flags);

/** Constructor for a Store node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param *db    A pointer for debug information.
 * @param *store The current memory
 * @param *addr  A pointer to the variable to be read in this memory.
 * @param *val   The value to write to this variable.
 * @param  flags Additional flags for alignment, volatility and pin state.
 */
FIRM_API ir_node *new_d_Store(dbg_info *db, ir_node *store, ir_node *addr,
                              ir_node *val, ir_cons_flags flags);

/** Constructor for a Alloc node.
 *
 * The Alloc node extends the memory by space for an entity of type alloc_type.
 * Adds the node to the block in current_ir_block.
 *
 * @param *db         A pointer for debug information.
 * @param *store      The memory which shall contain the new variable.
 * @param *count      The number of objects to allocate.
 * @param *alloc_type The type of the allocated variable.
 * @param where       Where to allocate the variable, either heap_alloc or stack_alloc.
 */
FIRM_API ir_node *new_d_Alloc(dbg_info *db, ir_node *store, ir_node *count,
                              ir_type *alloc_type, ir_where_alloc where);

 /** Constructor for a Free node.
 *
 * Frees the memory occupied by the entity pointed to by the pointer
 * arg.  Type indicates the type of the entity the argument points to.
 * Adds the node to the block in current_ir_block.
 *
 * @param *db         A pointer for debug information.
 * @param *store      The memory which shall contain the new variable.
 * @param *ptr        The pointer to the object to free.
 * @param *size       The number of objects of type free_type to free in a sequence.
 * @param *free_type  The type of the freed variable.
 * @param where       Where the variable was allocated, either heap_alloc or stack_alloc.
 */
FIRM_API ir_node *new_d_Free(dbg_info *db, ir_node *store, ir_node *ptr,
                             ir_node *size, ir_type *free_type,
                             ir_where_alloc where);

/** Constructor for a Sync node.
 *
 * Merges several memory values.  The node assumes that a variable
 * either occurs only in one of the memories, or it contains the same
 * value in all memories where it occurs.
 * Adds the node to the block in current_ir_block.
 *
 * @param *db       A pointer for debug information.
 * @param  arity    The number of memories to synchronize.
 * @param  **in     An array of pointers to nodes that produce an output of type
 *                  memory.  The constructor copies this array.
 */
FIRM_API ir_node *new_d_Sync(dbg_info *db, int arity, ir_node *in[]);

/** Constructor for a Proj node.
 *
 * Projects a single value out of a tuple.  The parameter proj gives the
 * position of the value within the tuple.
 * Adds the node to the block in current_ir_block.
 *
 * @param *db    A pointer for deubug information.
 * @param arg    A node producing a tuple.
 * @param *mode  The mode of the value to project.
 * @param proj   The position of the value in the tuple.
 */
FIRM_API ir_node *new_d_Proj(dbg_info *db, ir_node *arg, ir_mode *mode,
                             long proj);

/** Constructor for a defaultProj node.
 *
 * Represents the default control flow of a Switch-Cond node.
 * Adds the node to the block in current_ir_block.
 *
 * @param *db       A pointer for debug information.
 * @param arg       A node producing a tuple.
 * @param max_proj  The end  position of the value in the tuple.
 */
FIRM_API ir_node *new_d_defaultProj(dbg_info *db, ir_node *arg, long max_proj);

/** Constructor for a Tuple node.
 *
 * This is an auxiliary node to replace a node that returns a tuple
 * without changing the corresponding Proj nodes.
 * Adds the node to the block in current_ir_block.
 *
 * @param *db     A pointer for debug information.
 * @param arity   The number of tuple elements.
 * @param **in    An array containing pointers to the nodes producing the tuple elements.
 */
FIRM_API ir_node *new_d_Tuple(dbg_info *db, int arity, ir_node *in[]);

/** Constructor for a Id node.
 *
 * This is an auxiliary node to replace a node that returns a single
 * value. Adds the node to the block in current_ir_block.
 *
 * @param *db     A pointer for debug information.
 * @param *val    The operand to Id.
 * @param *mode   The mode of *val.
 */
FIRM_API ir_node *new_d_Id(dbg_info *db, ir_node *val, ir_mode *mode);

/** Constructor for a Confirm node.
 *
 * Constructor for a Confirm node. Adds the node to the block in current_ir_block.
 * Specifies constraints for a value.  To support dataflow analyses.
 *
 * Example: If the value never exceeds '100' this is expressed by placing a
 * Confirm node val = new_d_Confirm(db, val, 100, '<=') on the dataflow edge.
 *
 * @param *db     A pointer for debug information.
 * @param *val    The value we express a constraint for
 * @param *bound  The value to compare against. Must be a firm node, typically a constant.
 * @param cmp     The compare operation.
 */
FIRM_API ir_node *new_d_Confirm(dbg_info *db, ir_node *val, ir_node *bound,
                                pn_Cmp cmp);

/** Constructor for an Unknown node.
 *
 * Represents an arbitrary value.  Places the node in
 * the start block.
 *
 * @param *db     A pointer for debug information.
 * @param *m      The mode of the unknown value.
 */
FIRM_API ir_node *new_d_Unknown(dbg_info *db, ir_mode *m);

/** Constructor for a Mux node.
 *
 * @param *db       A pointer for debug information.
 * @param *sel      The ir_node that calculates the boolean select.
 * @param *ir_true  The ir_node that calculates the true result.
 * @param *ir_false The ir_node that calculates the false result.
 * @param *mode     The mode of the node (and it_true and ir_false).
 */
FIRM_API ir_node *new_d_Mux(dbg_info *db, ir_node *sel,
                            ir_node *ir_false, ir_node *ir_true, ir_mode *mode);

/** Constructor for a CopyB node.
 *
 * @param *db         A pointer for debug information.
 * @param *store      The current memory
 * @param *dst        The ir_node that represents the destination address.
 * @param *src        The ir_node that represents the source address.
 * @param *data_type  The type of the copied data
 */
FIRM_API ir_node *new_d_CopyB(dbg_info *db, ir_node *store, ir_node *dst,
                              ir_node *src, ir_type *data_type);

/** Constructor for a InstOf node.
 *
 * A High-Level Type check.
 *
 * @param   *db        A pointer for debug information.
 * @param   *store     The memory in which the object the entity should be selected
 *                     from is allocated.
 * @param   *objptr    A pointer to a object of a class type.
 * @param   *type      The type of which objptr must be.
 */
FIRM_API ir_node *new_d_InstOf(dbg_info *db, ir_node *store, ir_node *objptr,
                               ir_type *type);

/** Constructor for a Raise node.
 *
 * A High-Level Exception throw.
 *
 * @param *db    A pointer for debug information.
 * @param *store The current memory.
 * @param *obj   A pointer to the Except variable.
 */
FIRM_API ir_node *new_d_Raise(dbg_info *db, ir_node *store, ir_node *obj);

/** Constructor for a Bound node.
 *
 * A High-Level bounds check. Checks whether lower <= idx && idx < upper.
 *
 * @param *db         A pointer for debug information.
 * @param *store      The current memory
 * @param *idx        The ir_node that represents an index.
 * @param *lower      The ir_node that represents the lower bound for the index.
 * @param *upper      The ir_node that represents the upper bound for the index.
 */
FIRM_API ir_node *new_d_Bound(dbg_info *db, ir_node *store, ir_node *idx,
                              ir_node *lower, ir_node *upper);

/** Constructor for a Pin node.
 *
 * @param *db         A pointer for debug information.
 * @param *node       The node which value should be pinned.
 */
FIRM_API ir_node *new_d_Pin(dbg_info *db, ir_node *node);

/** Constructor for an ASM pseudo node.
 *
 * @param *db         A pointer for debug information.
 * @param arity       The number of data inputs to the node.
 * @param *in         The array of length arity of data inputs.
 * @param *inputs     The array of length arity of input constraints.
 * @param n_outs      The number of data outputs to the node.
 * @param *outputs    The array of length n_outs of output constraints.
 * @param n_clobber   The number of clobbered registers.
 * @param *clobber    The array of length n_clobber of clobbered registers.
 * @param *asm_text   The assembler text.
 */
FIRM_API ir_node *new_d_ASM(dbg_info *db, int arity, ir_node *in[],
                            ir_asm_constraint *inputs,
                            int n_outs, ir_asm_constraint *outputs,
                            int n_clobber, ident *clobber[], ident *asm_text);

/*-----------------------------------------------------------------------*/
/* The block oriented interface without debug support                    */
/*-----------------------------------------------------------------------*/

/** Constructor for a Block node.
 *
 * Constructor for a Block node. Adds the block to the graph in
 * current_ir_graph. Constructs a Block with a fixed number of predecessors.
 *
 * @param arity  The number of control predecessors.
 * @param in     An array of control predecessors.  The length of
 *               the array must be 'arity'.
 */
FIRM_API ir_node *new_Block(int arity, ir_node *in[]);

/** Constructor for a Start node.
 *
 * Adds the node to the block in current_ir_block.
 *
 */
FIRM_API ir_node *new_Start(void);

/** Constructor for an End node.
 *
 * Adds the node to the block in current_ir_block.
 */
FIRM_API ir_node *new_End(int arity, ir_node *in[]);

/** Constructor for a Jump node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * Jmp represents control flow to a single control successor.
 */
FIRM_API ir_node *new_Jmp(void);

/** Constructor for an IJmp node.
 *
 * IJmp represents control flow to a single control successor not
 * statically known i.e. an indirect Jmp.
 *
 * @param *tgt    The IR node representing the target address.
 */
FIRM_API ir_node *new_IJmp(ir_node *tgt);

/** Constructor for a Cond node.
 *
 * If c is mode_b represents a conditional branch (if/else). If c is
 * mode_Is/mode_Iu (?) represents a switch.  (Allocates dense Cond
 * node, default Proj is 0.). Adds the node to the block in current_ir_block.
 *
 * This is not consistent:  Input to Cond is Is, Proj has as proj number
 * longs.
 *
 *
 * @param *c     The conditions parameter.Can be of mode b or I_u.
 */
FIRM_API ir_node *new_Cond(ir_node *c);

/** Constructor for a Return node.
 *
 * Returns the memory and zero or more return values.  Only node that
 * can end regular control flow. Adds the node to the block in current_ir_block.
 *
 * @param *store The state of memory.
 * @param arity  Number of array indices.
 * @param *in    Array with index inputs to the node.
 */
FIRM_API ir_node *new_Return(ir_node *store, int arity, ir_node *in[]);

/** Constructor for a Const node.
 *
 * Constructor for a Const node. The constant represents a target
 * value.  Sets the type information to type_unknown.  (No more
 * supported: If tv is entity derives a somehow useful type.)
 * Adds the node to the block in current_ir_block.
 * Derives mode from passed tarval.
 *
 * @param *con   Points to an entry in the constant table. This pointer is
 *               added to the attributes of  the node.
 */
FIRM_API ir_node *new_Const(ir_tarval *con);

/**
 * Make a const from a long.
 * This is just convenience for the usual
 * <code>
 * new_Const(mode, tarval_from_long(mode, ...))
 * </code>
 * pain.
 * @param mode The mode for the const.
 * @param value The value of the constant.
 * @return A new const node.
 */
FIRM_API ir_node *new_Const_long(ir_mode *mode, long value);

/** Constructor for a SymConst node.
 *
 *  This is the constructor for a symbolic constant.
 *    There are several kinds of symbolic constants:
 *    - symconst_type_tag   The symbolic constant represents a type tag.  The
 *                          type the tag stands for is given explicitly.
 *    - symconst_type_size  The symbolic constant represents the size of a type.
 *                          The type of which the constant represents the size
 *                          is given explicitly.
 *    - symconst_type_align The symbolic constant represents the alignment of a
 *                          type.  The type of which the constant represents the
 *                          size is given explicitly.
 *    - symconst_addr_ent   The symbolic constant represents the address of an
 *                          entity (variable or method).  The variable is given
 *                          explicitly by a firm entity.
 *    - symconst_ofs_ent    The symbolic constant represents the offset of an
 *                          entity in its owner type.
 *    - symconst_enum_const The symbolic constant is a enumeration constant of
 *                          an enumeration type.
 *
 *    Inputs to the node:
 *      No inputs except the block it belongs to.
 *    Outputs of the node.
 *      An unsigned integer (I_u) or a pointer (P).
 *
 *    Mention union in declaration so that the firmjni generator recognizes that
 *    it can not cast the argument to an int.
 *
 * @param mode    The mode for the SymConst.
 * @param value   A type, ident, entity or enum constant depending on the
 *                SymConst kind.
 * @param kind    The kind of the symbolic constant, see the list above
 */
FIRM_API ir_node *new_SymConst(ir_mode *mode, union symconst_symbol value,
                               symconst_kind kind);

/** Constructor for a simpelSel node.
 *
 *  This is a shortcut for the new_Sel() constructor.  To be used for
 *  Sel nodes that do not select from an array, i.e., have no index
 *  inputs.  It adds the two parameters 0, NULL.
 *
 * @param   *store     The memory in which the object the entity should be selected from is allocated.
 * @param   *objptr    The object from that the Sel operation selects a single attribute out.
 * @param   *ent       The entity to select.
 */
FIRM_API ir_node *new_simpleSel(ir_node *store, ir_node *objptr,
                                ir_entity *ent);

/** Constructor for a Sel node.
 *
 * The select node selects an entity (field or method) from an entity
 * with a compound type.  It explicitly specifies the entity selected.
 * Dynamically the node may select entities that overwrite the given
 * entity.  If the selected entity is an array element entity the Sel
 * node takes the required array indices as inputs.
 * Adds the node to the block in current_ir_block.
 *
 * @param   *store     The memory in which the object the entity should be selected
 *                     from is allocated.
 * @param   *objptr    A pointer to a compound entity the Sel operation selects a
 *                     single attribute from.
 * @param   arity      The number of array indices needed to select an array element entity.
 * @param   *in[]      If the compound entity is an array the indices of the selected
 *                     element entity.  The constructor copies this array.
 * @param   *ent       The entity to select.
 */
FIRM_API ir_node *new_Sel(ir_node *store, ir_node *objptr,
                          int arity, ir_node *in[], ir_entity *ent);

/** Constructor for a Call node.
 *
 * Adds the node to the block in current_ir_block.
 * Represents all kinds of method and function calls.
 *
 * @param   *store  The actual store.
 * @param   *callee A pointer to the called procedure.
 * @param   arity   The number of procedure parameters.
 * @param   *in[]   An array with the pointers to the parameters. The constructor copies this array.
 * @param   *tp     Type information of the procedure called.
 */
FIRM_API ir_node *new_Call(ir_node *store, ir_node *callee,
                           int arity, ir_node *in[], ir_type *tp);

/** Constructor for a Builtin node.
 *
 * Represents a call of a backend-specific builtin..
 * Represents all kinds of method and function calls.
 *
 * @param   *store  The actual store.
 * @param   kind    The kind of the called builtin.
 * @param   arity   The number of procedure parameters.
 * @param   *in[]   An array with the pointers to the parameters. The constructor copies this array.
 * @param   *tp     Type information of the procedure called.
 */
FIRM_API ir_node *new_Builtin(ir_node *store, int arity, ir_node *in[],
                              ir_builtin_kind kind, ir_type *tp);

/** Constructor for a Add node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_Add(ir_node *op1, ir_node *op2, ir_mode *mode);

/** Constructor for a Sub node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_Sub(ir_node *op1, ir_node *op2, ir_mode *mode);

/** Constructor for a Minus node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *op    The operand .
 * @param   *mode  The mode of the operand and the result.
 */
FIRM_API ir_node *new_Minus(ir_node *op,  ir_mode *mode);

/**
 * Constructor for a Mul node. Adds the node to the block in current_ir_block.
 *
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_Mul(ir_node *op1, ir_node *op2, ir_mode *mode);

/**
 * Constructor for a Mulh node. Adds the node to the block in current_ir_block.
 *
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_Mulh(ir_node *op1, ir_node *op2, ir_mode *mode);

/** Constructor for a Quot node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *memop The store needed to model exceptions
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the result.
 * @param   state  The pinned state.
 */
FIRM_API ir_node *new_Quot(ir_node *memop, ir_node *op1, ir_node *op2,
                           ir_mode *mode, op_pin_state state);

/** Constructor for a DivMod node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *memop The store needed to model exceptions
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the results.
 * @param   state  The pinned state.
 */
FIRM_API ir_node *new_DivMod(ir_node *memop, ir_node *op1, ir_node *op2,
                             ir_mode *mode, op_pin_state state);

/** Constructor for a Div node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *memop The store needed to model exceptions
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the result.
 * @param   state  The pinned state.
 */
FIRM_API ir_node *new_Div(ir_node *memop, ir_node *op1, ir_node *op2,
                          ir_mode *mode, op_pin_state state);

/** Constructor for a remainderless Div node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *memop The store needed to model exceptions
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the result.
 * @param   state  The pinned state.
 */
FIRM_API ir_node *new_DivRL(ir_node *memop, ir_node *op1, ir_node *op2,
                            ir_mode *mode, op_pin_state state);

/** Constructor for a Mod node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *memop The store needed to model exceptions
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the result.
 * @param   state  The pinned state.
 */
FIRM_API ir_node *new_Mod(ir_node *memop, ir_node *op1, ir_node *op2,
                          ir_mode *mode, op_pin_state state);

/** Constructor for a And node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_And(ir_node *op1, ir_node *op2, ir_mode *mode);

/**
 * Constructor for a Or node. Adds the node to the block in current_ir_block.
 *
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_Or(ir_node *op1, ir_node *op2, ir_mode *mode);

/**
 * Constructor for a Eor node. Adds the node to the block in current_ir_block.
 *
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the results.
 */
FIRM_API ir_node *new_Eor(ir_node *op1, ir_node *op2, ir_mode *mode);

/** Constructor for a Not node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *op    The operand.
 * @param   *mode  The mode of the operand and the result.
 */
FIRM_API ir_node *new_Not(ir_node *op, ir_mode *mode);

/** Constructor for a Shl node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *op    The operand.
 * @param   *k     The number of bits to  shift the operand .
 * @param   *mode  The mode of the operand and the result.
 */
FIRM_API ir_node *new_Shl(ir_node *op, ir_node *k, ir_mode *mode);

/**
 * Constructor for a Shr node. Adds the node to the block in current_ir_block.
 *
 * @param   *op    The operand.
 * @param   *k     The number of bits to  shift the operand .
 * @param   *mode  The mode of the operand and the result.
 */
FIRM_API ir_node *new_Shr(ir_node *op, ir_node *k, ir_mode *mode);

/** Constructor for a Shrs node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *op    The operand.
 * @param   *k     The number of bits to  shift the operand .
 * @param   *mode  The mode of the operand and the result.
 */
FIRM_API ir_node *new_Shrs(ir_node *op, ir_node *k, ir_mode *mode);

/** Constructor for a Rotl node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *op    The operand.
 * @param   *k     The number of bits to rotate the operand.
 * @param   *mode  The mode of the operand.
 */
FIRM_API ir_node *new_Rotl(ir_node *op, ir_node *k, ir_mode *mode);

/** Constructor for a Cmp node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 */
FIRM_API ir_node *new_Cmp(ir_node *op1, ir_node *op2);

/** Constructor for a Conv node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *op          The operand.
 * @param   *mode        The mode of this the operand muss be converted.
 */
FIRM_API ir_node *new_Conv(ir_node *op, ir_mode *mode);

/** Constructor for a strict Conv node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *op          The operand.
 * @param   *mode        The mode of this the operand muss be converted.
 */
FIRM_API ir_node *new_strictConv(ir_node *op, ir_mode *mode);

/** Constructor for a Cast node.
 *
 * Adds the node to the block in current_ir_block.
 * High level type cast
 *
 * @param   *op    The operand.
 * @param   *to_tp The type of this the operand muss be casted .
 */
FIRM_API ir_node *new_Cast(ir_node *op, ir_type *to_tp);

/** Constructor for a Carry node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_Carry(ir_node *op1, ir_node *op2, ir_mode *mode);

/** Constructor for a Borrow node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param   *op1   The first operand.
 * @param   *op2   The second operand.
 * @param   *mode  The mode of the operands and the result.
 */
FIRM_API ir_node *new_Borrow(ir_node *op1, ir_node *op2, ir_mode *mode);

/** Constructor for a Phi node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param arity  The number of predecessors.
 * @param *in    Array with predecessors.
 * @param *mode  The mode of it's inputs and output.
 */
FIRM_API ir_node *new_Phi(int arity, ir_node *in[], ir_mode *mode);

/** Constructor for a Load node.
 *
 * @param *store  The current memory.
 * @param *addr   A pointer to the variable to be read in this memory.
 * @param *mode   The mode of the value to be loaded.
 * @param  flags  Additional flags for alignment, volatility and pin state.
 */
FIRM_API ir_node *new_Load(ir_node *store, ir_node *addr, ir_mode *mode,
                           ir_cons_flags flags);

/** Constructor for a Store node.
 *
 * @param *store  The current memory.
 * @param *addr   A pointer to the variable to be read in this memory.
 * @param *val    The value to write to this variable.
 * @param  flags  Additional flags for alignment, volatility and pin state.
 */
FIRM_API ir_node *new_Store(ir_node *store, ir_node *addr, ir_node *val,
                            ir_cons_flags flags);

/** Constructor for a Alloc node.
 *
 * The Alloc node extends the memory by space for an entity of type alloc_type.
 * Adds the node to the block in current_ir_block.
 *
 * @param *store      The memory which shall contain the new variable.
 * @param *count      The number of objects to allocate.
 * @param *alloc_type The type of the allocated variable.
 * @param where       Where to allocate the variable, either heap_alloc or stack_alloc.
 */
FIRM_API ir_node *new_Alloc(ir_node *store, ir_node *count, ir_type *alloc_type,
                            ir_where_alloc where);

/** Constructor for a Free node.
 *
 * Frees the memory occupied by the entity pointed to by the pointer
 * arg.  Type indicates the type of the entity the argument points to.
 * Adds the node to the block in current_ir_block.
 *
 * @param *store      The memory which shall contain the new variable.
 * @param *ptr        The pointer to the object to free.
 * @param *size       The number of objects of type free_type to free in a sequence.
 * @param *free_type  The type of the freed variable.
 * @param where       Where the variable was allocated, either heap_alloc or stack_alloc.
 */
FIRM_API ir_node *new_Free(ir_node *store, ir_node *ptr, ir_node *size,
                           ir_type *free_type, ir_where_alloc where);

/** Constructor for a Sync node.
 *
 * Merges several memory values.  The node assumes that a variable
 * either occurs only in one of the memories, or it contains the same
 * value in all memories where it occurs.
 * Adds the node to the block in current_ir_block.
 *
 * @param  arity    The number of memories to synchronize.
 * @param  **in     An array of pointers to nodes that produce an output of type
 *                  memory.  The constructor copies this array.
 */
FIRM_API ir_node *new_Sync(int arity, ir_node *in[]);

/** Constructor for a Proj node.
 *
 * Projects a single value out of a tuple.  The parameter proj gives the
 * position of the value within the tuple.
 * Adds the node to the block in current_ir_block.
 *
 * @param arg    A node producing a tuple.
 * @param *mode  The mode of the value to project.
 * @param proj   The position of the value in the tuple.
 */
FIRM_API ir_node *new_Proj(ir_node *arg, ir_mode *mode, long proj);

/** Constructor for a defaultProj node.
 *
 * Represents the default control flow of a Switch-Cond node.
 * Adds the node to the block in current_ir_block.
 *
 * @param arg       A node producing a tuple.
 * @param max_proj  The end  position of the value in the tuple.
 */
FIRM_API ir_node *new_defaultProj(ir_node *arg, long max_proj);

/** Constructor for a Tuple node.
 *
 * This is an auxiliary node to replace a node that returns a tuple
 * without changing the corresponding Proj nodes.
 * Adds the node to the block in current_ir_block.
 *
 * @param arity   The number of tuple elements.
 * @param **in    An array containing pointers to the nodes producing the tuple elements.
 */
FIRM_API ir_node *new_Tuple(int arity, ir_node *in[]);

/** Constructor for an Id node.
 *
 * This is an auxiliary node to replace a node that returns a single
 * value. Adds the node to the block in current_ir_block.
 *
 * @param *val    The operand to Id.
 * @param *mode   The mode of *val.
 */
FIRM_API ir_node *new_Id(ir_node *val, ir_mode *mode);

/** Constructor for a Bad node.
 *
 * Returns the unique Bad node of the graph.  The same as
 * get_irg_bad().
 */
FIRM_API ir_node *new_Bad(void);

/** Constructor for a Confirm node.
 *
 * Specifies constraints for a value.  To support dataflow analyses.
 * Adds the node to the block in current_ir_block.
 *
 * Example: If the value never exceeds '100' this is expressed by placing a
 * Confirm node val = new_d_Confirm(db, val, 100, '<=') on the dataflow edge.
 *
 * @param *val    The value we express a constraint for
 * @param *bound  The value to compare against. Must be a firm node, typically a constant.
 * @param cmp     The compare operation.
 */
FIRM_API ir_node *new_Confirm(ir_node *val, ir_node *bound, pn_Cmp cmp);

/** Constructor for an Unknown node.
 *
 * Represents an arbitrary value.  Places the node in
 * the start block.
 *
 * @param *m      The mode of the unknown value.
 */
FIRM_API ir_node *new_Unknown(ir_mode *m);

/** Constructor for a NoMem node.
 *
 * Returns the unique NoMem node of the graph.  The same as
 * get_irg_no_mem().
 */
FIRM_API ir_node *new_NoMem(void);

/** Constructor for a Mux node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param *sel      The ir_node that calculates the boolean select.
 * @param *ir_true  The ir_node that calculates the true result.
 * @param *ir_false The ir_node that calculates the false result.
 * @param *mode     The mode of the node (and it_true and ir_false).
 */
FIRM_API ir_node *new_Mux(ir_node *sel, ir_node *ir_false, ir_node *ir_true,
                          ir_mode *mode);

/** Constructor for a CopyB node.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param *store      The current memory
 * @param *dst        The ir_node that represents the destination address.
 * @param *src        The ir_node that represents the source address.
 * @param *data_type  The type of the copied data
 */
FIRM_API ir_node *new_CopyB(ir_node *store, ir_node *dst, ir_node *src,
                            ir_type *data_type);

/** Constructor for a InstOf node.
 *
 * A High-Level Type check.
 *
 * @param   *store     The memory in which the object the entity should be selected
 *                     from is allocated.
 * @param   *objptr    A pointer to a object of a class type.
 * @param   *type      The type of which objptr must be.
 */
FIRM_API ir_node *new_InstOf(ir_node *store, ir_node *objptr, ir_type *type);

/**Constructor for a Raise node.
 *
 * A High-Level Exception throw.
 *
 * @param *store The current memory.
 * @param *obj   A pointer to the Except variable.
 */
FIRM_API ir_node *new_Raise(ir_node *store, ir_node *obj);

/** Constructor for a Bound node.
 *
 * A High-Level bounds check. Checks whether lower <= idx && idx < upper.
 *
 * Adds the node to the block in current_ir_block.
 *
 * @param *store      The current memory
 * @param *idx        The ir_node that represents an index.
 * @param *lower      The ir_node that represents the lower bound for the index.
 * @param *upper      The ir_node that represents the upper bound for the index.
 */
FIRM_API ir_node *new_Bound(ir_node *store, ir_node *idx, ir_node *lower,
                            ir_node *upper);

/** Constructor for a Pin node.
 *
 * @param *node       The node which value should be pinned.
 */
FIRM_API ir_node *new_Pin(ir_node *node);

/** Constructor for an ASM pseudo node.
 *
 * @param arity       The number of data inputs to the node.
 * @param *in         The array of length arity of data inputs.
 * @param *inputs     The array of length arity of input constraints.
 * @param n_outs      The number of data outputs to the node.
 * @param *outputs    The array of length n_outs of output constraints.
 * @param n_clobber   The number of clobbered registers.
 * @param *clobber    The array of length n_clobber of clobbered registers.
 * @param *asm_text   The assembler text.
 */
FIRM_API ir_node *new_ASM(int arity, ir_node *in[], ir_asm_constraint *inputs,
                          int n_outs, ir_asm_constraint *outputs,
                          int n_clobber, ident *clobber[], ident *asm_text);

/**
 * @brief Constructor for a Dummy node.
 *
 * @param *db       debug info for the node
 * @param *mode     The mode of the node.
 * @param *irg      the graph to put the node into
 * @returns         the newly created note
 */
FIRM_API ir_node *new_rd_Dummy(dbg_info *db, ir_graph *irg, ir_mode *mode);

/**
 * @copybrief new_rd_Dummy()
 *
 * @param *mode     The mode of the node.
 * @param *irg      the graph to put the node into
 * @returns         the newly created note
 */
FIRM_API ir_node *new_r_Dummy(ir_graph *irg, ir_mode *mode);

/**
 * @copybrief new_rd_Dummy()
 *
 * @param *db       debug info for the node
 * @param *mode     The mode of the node.
 * @returns         the newly created note
 */
FIRM_API ir_node *new_d_Dummy(dbg_info *db, ir_mode *mode);

/**
 * @copybrief new_rd_Dummy()
 *
 * @param *mode     The mode of the node.
 * @returns         the newly created note
 */
FIRM_API ir_node *new_Dummy(ir_mode *mode);

/*---------------------------------------------------------------------*/
/* The comfortable interface.                                          */
/* Supports automatic Phi node construction.                           */
/* All routines of the block oriented interface except new_Block are   */
/* needed also.                                                        */
/*---------------------------------------------------------------------*/

/** Create an immature Block.
 *
 * An immature Block has an unknown number of predecessors.  Predecessors
 * can be added with add_immBlock_pred().  Once all predecessors are
 * added the block must be matured.
 *
 * Adds the block to the graph in current_ir_graph. Can be used with automatic
 * Phi node construction.
 * This constructor can only be used if the graph is in state_building.
 */
FIRM_API ir_node *new_d_immBlock(dbg_info *db);
FIRM_API ir_node *new_immBlock(void);
FIRM_API ir_node *new_r_immBlock(ir_graph *irg);
FIRM_API ir_node *new_rd_immBlock(dbg_info *db, ir_graph *irg);

/** Add a control flow edge to an immature block. */
FIRM_API void add_immBlock_pred(ir_node *immblock, ir_node *jmp);

/** Finalize a Block node, when all control flows are known. */
FIRM_API void mature_immBlock(ir_node *block);

/** Get the current value of a local variable.
 *
 * Use this function to obtain the last definition of the local variable
 * associated with pos.  Pos may not exceed the value passed as n_loc
 * to new_ir_graph.  This call automatically inserts Phi nodes.
 *
 * @param *db    A pointer for debug information.
 * @param  pos   The position/id of the local variable.
 * @param *mode  The mode of the value to get.
 */
FIRM_API ir_node *get_value(int pos, ir_mode *mode);
FIRM_API ir_node *get_r_value(ir_graph *irg, int pos, ir_mode *mode);

/**
 * Try to guess the mode of a local variable.
 * This is done by recursively going up the control flow graph until
 * we find a definition for the variable. The mode of the first found
 * definition is returned. NULL in case no definition is found.
 *
 * @param  pos   The position/id of the local variable.
 */
FIRM_API ir_mode *ir_guess_mode(int pos);

/** Remark a new definition of a variable.
 *
 * Use this function to remember a new definition of the value
 * associated with pos. Pos may not exceed the value passed as n_loc
 * to new_ir_graph.  This call is needed to automatically inserts Phi
 * nodes.
 *
 * @param  pos   The position/id of the local variable.
 * @param *value The new value written to the local variable.
 */
FIRM_API void set_value(int pos, ir_node *value);
FIRM_API void set_r_value(ir_graph *irg, int pos, ir_node *value);

/**
 * Find the value number for a node in the current block.
 *
 * @param value  the searched value
 *
 * @return the value number of the value or -1 if this value has
 * no value number in the current block.
 */
FIRM_API int find_value(ir_node *value);

/** Get the current memory state.
 *
 * Use this function to obtain the last definition of the memory
 * state.  This call automatically inserts Phi nodes for the memory
 * state value.
 */
FIRM_API ir_node *get_store(void);
FIRM_API ir_node *get_r_store(ir_graph *irg);

/** Remark a new definition of the memory state.
 *
 * Use this function to remember a new definition of the memory state.
 * This call is needed to automatically inserts Phi nodes.
 *
 * @param *store The new memory state.
 */
FIRM_API void set_store(ir_node *store);
FIRM_API void set_r_store(ir_graph *irg, ir_node *store);

/** keep this node alive even if End is not control-reachable from it
 *
 * @param ka The node to keep alive.
 */
FIRM_API void keep_alive(ir_node *ka);

/* --- initialize and finalize IR construction --- */

/** Puts the graph into state "phase_high" */
FIRM_API void irg_finalize_cons(ir_graph *irg);

/** Puts the program and all graphs into state phase_high.
 *
 * This also remarks, the construction of types is finished,
 * e.g., that no more subtypes will be added.  */
FIRM_API void irp_finalize_cons(void);

FIRM_API void ir_set_uninitialized_local_variable_func(
		uninitialized_local_variable_func_t *func);

#include "end.h"

#endif
