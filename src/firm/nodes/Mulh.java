/* Warning: Automatically generated file */
package firm.nodes;

import com.sun.jna.Pointer;

import firm.bindings.binding_ircons;
import firm.bindings.binding_irnode;
/* There are "unused" warnings in some classes,
	but suppressing these, emits warnings, because
	of useless suppress in others. Just ignore this! */

public class Mulh extends Binop {

	public Mulh(Pointer ptr) {
		super(ptr);
	}

	
	@Override
	public Node getLeft() {
		return createWrapper(binding_irnode.get_Mulh_left(ptr));
	}

	@Override
	public void setLeft(Node left) {
		binding_irnode.set_Mulh_left(this.ptr, left.ptr);
	}
	
	@Override
	public Node getRight() {
		return createWrapper(binding_irnode.get_Mulh_right(ptr));
	}

	@Override
	public void setRight(Node right) {
		binding_irnode.set_Mulh_right(this.ptr, right.ptr);
	}
	

	

	

	public static final int pnMax = 0;

	
	public void accept(NodeVisitor visitor) {
		visitor.visit(this);
	} 
	
}