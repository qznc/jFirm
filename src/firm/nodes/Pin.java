/* Warning: Automatically generated file */
package firm.nodes;

import com.sun.jna.Pointer;

import firm.bindings.binding_ircons;
import firm.bindings.binding_irnode;
/* There are "unused" warnings in some classes,
	but suppressing these, emits warnings, because
	of useless suppress in others. Just ignore this! */

public class Pin extends Node {

	public Pin(Pointer ptr) {
		super(ptr);
	}

	
	
	public Node getOp() {
		return createWrapper(binding_irnode.get_Pin_op(ptr));
	}

	
	public void setOp(Node op) {
		binding_irnode.set_Pin_op(this.ptr, op.ptr);
	}
	

	

	

	public static final int pnMax = 0;

	
	public void accept(NodeVisitor visitor) {
		visitor.visit(this);
	} 
	
}