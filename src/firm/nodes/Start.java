/* Warning: Automatically generated file */
package firm.nodes;

import com.sun.jna.Pointer;

import firm.bindings.binding_ircons;
import firm.bindings.binding_irnode;
/* There are "unused" warnings in some classes,
	but suppressing these, emits warnings, because
	of useless suppress in others. Just ignore this! */

public class Start extends Node {

	public Start(Pointer ptr) {
		super(ptr);
	}

	

	

	

	
	/** control flow */
	public static final int pnXInitialExec = 0;
	
	/** initial memory */
	public static final int pnM = 1;
	
	/** frame base pointer */
	public static final int pnPFrameBase = 2;
	
	/** pointer to thread local storage segment */
	public static final int pnPTls = 3;
	
	/** function arguments */
	public static final int pnTArgs = 4;
	public static final int pnMax = 5;

	
	public void accept(NodeVisitor visitor) {
		visitor.visit(this);
	} 
	
}